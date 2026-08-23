#include "gz_sensor_monitor/sensor_monitor_system.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>
#include <gz/msgs/laserscan.pb.h>

namespace gz_sensor_monitor
{

SensorMonitorSystem::SensorMonitorSystem()
{
  this->lastUpdate_ = std::chrono::steady_clock::now();
}

SensorMonitorSystem::~SensorMonitorSystem()
{
  this->StopRos();
}

std::string SensorMonitorSystem::GetString(
  const std::shared_ptr<const sdf::Element> &sdf,
  const std::string &name,
  const std::string &fallback) const
{
  return sdf->HasElement(name) ? sdf->Get<std::string>(name) : fallback;
}

double SensorMonitorSystem::GetDouble(
  const std::shared_ptr<const sdf::Element> &sdf,
  const std::string &name,
  double fallback) const
{
  return sdf->HasElement(name) ? sdf->Get<double>(name) : fallback;
}

void SensorMonitorSystem::Configure(
  const gz::sim::Entity &,
  const std::shared_ptr<const sdf::Element> &sdf,
  gz::sim::EntityComponentManager &,
  gz::sim::EventManager &)
{
  this->visualName_ = this->GetString(sdf, "visual_name", "monitor_screen_visual");
  this->modeTopic_ = this->GetString(sdf, "mode_topic", "/screen/mode");
  this->frameTopic_ = this->GetString(
    sdf, "frame_topic", "/gz_sensor_monitor/frame");
  if (sdf->HasElement("screen_width"))
    this->width_ = sdf->Get<int>("screen_width");

  if (sdf->HasElement("screen_height"))
    this->height_ = sdf->Get<int>("screen_height");

  this->updateRate_ = this->GetDouble(sdf, "update_rate", 10.0);
  this->depthMin_ = this->GetDouble(sdf, "depth_min", 0.2);
  this->depthMax_ = this->GetDouble(sdf, "depth_max", 5.0);

  if (this->width_ < 32) this->width_ = 32;
  if (this->height_ < 32) this->height_ = 32;
  if (this->updateRate_ <= 0.0) this->updateRate_ = 10.0;

  // ----------------------------------------------------------
  // Parse:
  //
  // <sources>
  //   <source>...</source>
  //   <source>...</source>
  // </sources>
  // ----------------------------------------------------------
  if (sdf->HasElement("sources"))
  {
    // sdformat14 exposes GetElement() as non-const.
    auto sdfMutable =
      std::const_pointer_cast<sdf::Element>(sdf);

    auto sources =
      sdfMutable->GetElement("sources");
    auto source = sources->GetElement("source");

    while (source)
    {
      SourceConfig cfg;

      if (source->HasElement("name"))
        cfg.name = source->Get<std::string>("name");

      if (source->HasElement("type"))
        cfg.type = SourceTypeFromString(
          source->Get<std::string>("type"));

      if (source->HasElement("topic"))
        cfg.topic = source->Get<std::string>("topic");

      if (source->HasElement("depth_topic"))
        cfg.depthTopic = source->Get<std::string>("depth_topic");

      cfg.depthMin = this->depthMin_;
      cfg.depthMax = this->depthMax_;

      if (source->HasElement("depth_min"))
        cfg.depthMin = source->Get<double>("depth_min");

      if (source->HasElement("depth_max"))
        cfg.depthMax = source->Get<double>("depth_max");

      if (cfg.name.empty())
      {
        gzerr << "[GzSensorMonitor] Source without <name> ignored.\n";
      }
      else if (cfg.topic.empty())
      {
        gzerr << "[GzSensorMonitor] Source [" << cfg.name
              << "] has no <topic>; ignored.\n";
      }
      else if (cfg.type == SourceType::UNKNOWN)
      {
        gzerr << "[GzSensorMonitor] Source [" << cfg.name
              << "] has unknown <type>; ignored.\n";
      }
      else if (cfg.type == SourceType::RGBD &&
               cfg.depthTopic.empty())
      {
        gzerr << "[GzSensorMonitor] RGBD source [" << cfg.name
              << "] needs <depth_topic>; ignored.\n";
      }
      else
      {
        this->sources_[cfg.name] = cfg;

        gzmsg << "[GzSensorMonitor] source="
              << cfg.name
              << " type=" << SourceTypeToString(cfg.type)
              << " topic=" << cfg.topic;

        if (!cfg.depthTopic.empty())
          gzmsg << " depth_topic=" << cfg.depthTopic;

        gzmsg << std::endl;
      }

      source = source->GetNextElement("source");
    }
  }

  if (this->sources_.empty())
  {
    gzerr << "[GzSensorMonitor] No valid sources configured.\n";
  }

  // Prefer RGB as the initial screen mode when it is configured.
  if (this->sources_.count("front_rgb"))
    this->selectedSource_ = "front_rgb";
  else if (!this->sources_.empty())
    this->selectedSource_ = this->sources_.begin()->first;

  // Gazebo transport topic used between server and GUI.
  this->framePub_ =
    this->gzNode_.Advertise<gz::msgs::Image>(this->frameTopic_);

  this->StartRos();
  this->configured_ = true;

  gzmsg << "[GzSensorMonitor] configured with "
        << this->sources_.size()
        << " source(s), update rate "
        << this->updateRate_ << " Hz.\n";
}

void SensorMonitorSystem::StartRos()
{
  if (!rclcpp::ok())
  {
    int argc = 0;
    char **argv = nullptr;
    rclcpp::init(argc, argv);
  }

  this->node_ =
    std::make_shared<rclcpp::Node>("gz_sensor_monitor_system");

  this->modeSub_ =
    this->node_->create_subscription<std_msgs::msg::String>(
      this->modeTopic_,
      rclcpp::QoS(10),
      std::bind(
        &SensorMonitorSystem::ModeCallback,
        this,
        std::placeholders::_1));

  // ROS 2 subscriptions are kept for real robots / bridged topics.
  for (const auto &[name, cfg] : this->sources_)
  {
    if (cfg.type == SourceType::IMAGE)
      this->AddImageSubscription(cfg);
    else if (cfg.type == SourceType::DEPTH)
      this->AddDepthSubscription(cfg);
    else if (cfg.type == SourceType::RGBD)
    {
      this->AddImageSubscription(cfg);

      SourceConfig depthCfg = cfg;
      depthCfg.name = cfg.name + "__depth";
      depthCfg.type = SourceType::DEPTH;
      depthCfg.topic = cfg.depthTopic;
      this->AddDepthSubscription(depthCfg);
    }
    else if (cfg.type == SourceType::LIDAR)
      this->AddLidarSubscription(cfg);
  }

  // Gazebo Sim sensors publish native gz::msgs directly. Subscribe to those
  // topics as well, so the demo works without ros_gz_bridge.
  for (const auto &[name, cfg] : this->sources_)
  {
    bool ok = false;

    if (cfg.type == SourceType::IMAGE)
    {
      std::function<void(const gz::msgs::Image &)> cb =
        [this, name](const gz::msgs::Image &msg)
        {
          this->GzImageCallback(name, msg);
        };
      ok = this->gzNode_.Subscribe<gz::msgs::Image>(cfg.topic, cb);
    }
    else if (cfg.type == SourceType::DEPTH)
    {
      std::function<void(const gz::msgs::Image &)> cb =
        [this, name](const gz::msgs::Image &msg)
        {
          this->GzDepthCallback(name, msg);
        };
      ok = this->gzNode_.Subscribe<gz::msgs::Image>(cfg.topic, cb);
    }
    else if (cfg.type == SourceType::RGBD)
    {
      std::function<void(const gz::msgs::Image &)> rgbCb =
        [this, name](const gz::msgs::Image &msg)
        {
          this->GzImageCallback(name, msg);
        };
      std::function<void(const gz::msgs::Image &)> depthCb =
        [this, name](const gz::msgs::Image &msg)
        {
          this->GzDepthCallback(name + "__depth", msg);
        };

      const bool rgbOk =
        this->gzNode_.Subscribe<gz::msgs::Image>(cfg.topic, rgbCb);
      const bool depthOk =
        this->gzNode_.Subscribe<gz::msgs::Image>(cfg.depthTopic, depthCb);
      ok = rgbOk && depthOk;
    }
    else if (cfg.type == SourceType::LIDAR)
    {
      std::function<void(const gz::msgs::LaserScan &)> cb =
        [this, name](const gz::msgs::LaserScan &msg)
        {
          this->GzLidarCallback(name, msg);
        };
      ok = this->gzNode_.Subscribe<gz::msgs::LaserScan>(cfg.topic, cb);
    }

    if (ok)
    {
      gzmsg << "[GzSensorMonitor] native Gazebo subscription OK: "
            << name << " -> " << cfg.topic << std::endl;
    }
    else
    {
      gzerr << "[GzSensorMonitor] native Gazebo subscription FAILED: "
            << name << " -> " << cfg.topic << std::endl;
    }
  }

  this->executor_ =
    std::make_shared<rclcpp::executors::SingleThreadedExecutor>();

  this->executor_->add_node(this->node_);
  this->running_ = true;

  this->rosThread_ = std::thread([this]()
  {
    while (this->running_ && rclcpp::ok())
    {
      this->executor_->spin_some(
        std::chrono::milliseconds(5));
    }
  });
}

void SensorMonitorSystem::StopRos()
{
  this->running_ = false;

  if (this->rosThread_.joinable())
    this->rosThread_.join();

  this->modeSub_.reset();
  this->imageSubs_.clear();
  this->lidarSubs_.clear();

  if (this->executor_ && this->node_)
    this->executor_->remove_node(this->node_);

  this->executor_.reset();
  this->node_.reset();
}

void SensorMonitorSystem::AddImageSubscription(
  const SourceConfig &source)
{
  auto sub =
    this->node_->create_subscription<sensor_msgs::msg::Image>(
      source.topic,
      rclcpp::SensorDataQoS(),
      [this, name = source.name](
        const sensor_msgs::msg::Image::SharedPtr msg)
      {
        this->ImageCallback(name, msg);
      });

  this->imageSubs_.push_back(sub);
}

void SensorMonitorSystem::AddDepthSubscription(
  const SourceConfig &source)
{
  auto sub =
    this->node_->create_subscription<sensor_msgs::msg::Image>(
      source.topic,
      rclcpp::SensorDataQoS(),
      [this, name = source.name](
        const sensor_msgs::msg::Image::SharedPtr msg)
      {
        this->DepthCallback(name, msg);
      });

  this->imageSubs_.push_back(sub);
}

void SensorMonitorSystem::AddLidarSubscription(
  const SourceConfig &source)
{
  auto sub =
    this->node_->create_subscription<sensor_msgs::msg::LaserScan>(
      source.topic,
      rclcpp::SensorDataQoS(),
      [this, name = source.name](
        const sensor_msgs::msg::LaserScan::SharedPtr msg)
      {
        this->LidarCallback(name, msg);
      });

  this->lidarSubs_.push_back(sub);
}


void SensorMonitorSystem::GzImageCallback(
  const std::string &name,
  const gz::msgs::Image &msg)
{
  cv::Mat image = this->ConvertGzColorImage(msg);
  if (image.empty())
    return;

  {
    std::lock_guard<std::mutex> lock(this->dataMutex_);
    this->rgbFrames_[name] = {image, true};
  }

  static std::mutex reportMutex;
  static std::map<std::string, bool> reported;
  std::lock_guard<std::mutex> reportLock(reportMutex);
  if (!reported[name])
  {
    reported[name] = true;
    gzmsg << "[GzSensorMonitor] first Gazebo RGB frame received from "
          << name << std::endl;
  }
}

void SensorMonitorSystem::GzDepthCallback(
  const std::string &name,
  const gz::msgs::Image &msg)
{
  auto it = this->sources_.find(
    name == "" ? name :
    (name.size() > 7 && name.rfind("__depth") == name.size() - 7
      ? name.substr(0, name.size() - 7) : name));

  double minDepth = this->depthMin_;
  double maxDepth = this->depthMax_;
  if (it != this->sources_.end())
  {
    minDepth = it->second.depthMin;
    maxDepth = it->second.depthMax;
  }

  cv::Mat image = this->ConvertGzDepthImage(msg, minDepth, maxDepth);
  if (image.empty())
  {
    gzerr << "[GzSensorMonitor] depth frame conversion failed for "
          << name << " format="
          << gz::msgs::PixelFormatType_Name(msg.pixel_format_type())
          << " size=" << msg.width() << "x" << msg.height()
          << " step=" << msg.step() << std::endl;
    return;
  }

  {
    std::lock_guard<std::mutex> lock(this->dataMutex_);
    this->depthFrames_[name] = {image, true};
  }

  static std::mutex depthReportMutex;
  static std::map<std::string, bool> depthReported;
  std::lock_guard<std::mutex> reportLock(depthReportMutex);

  if (!depthReported[name])
  {
    depthReported[name] = true;
    gzmsg << "[GzSensorMonitor] first Gazebo DEPTH frame received from "
          << name << std::endl;
  }
}

void SensorMonitorSystem::GzLidarCallback(
  const std::string &name,
  const gz::msgs::LaserScan &msg)
{
  cv::Mat radar = this->MakeGzLidarRadar(msg);
  if (radar.empty())
    return;

  std::lock_guard<std::mutex> lock(this->dataMutex_);
  this->gzLidarFrames_[name] = radar;

  static std::map<std::string, bool> reported;
  if (!reported[name])
  {
    reported[name] = true;
    gzmsg << "[GzSensorMonitor] first Gazebo LiDAR frame received from "
          << name << std::endl;
  }
}

cv::Mat SensorMonitorSystem::ConvertGzColorImage(
  const gz::msgs::Image &msg) const
{
  if (msg.width() == 0 || msg.height() == 0 || msg.data().empty())
    return {};

  const int width = static_cast<int>(msg.width());
  const int height = static_cast<int>(msg.height());
  const int step = msg.step() > 0 ? static_cast<int>(msg.step()) : width * 3;
  const std::string format = gz::msgs::PixelFormatType_Name(msg.pixel_format_type());

  cv::Mat out;
  const unsigned char *data =
    reinterpret_cast<const unsigned char *>(msg.data().data());

  try
  {
    if (format == "RGB_INT8" || format == "RGB_UINT8")
    {
      cv::Mat rgb(height, width, CV_8UC3,
                  const_cast<unsigned char *>(data), step);
      cv::cvtColor(rgb, out, cv::COLOR_RGB2BGR);
    }
    else if (format == "BGR_INT8" || format == "BGR_UINT8")
    {
      cv::Mat bgr(height, width, CV_8UC3,
                  const_cast<unsigned char *>(data), step);
      out = bgr.clone();
    }
    else if (format == "L_INT8" || format == "L_UINT8")
    {
      cv::Mat gray(height, width, CV_8UC1,
                   const_cast<unsigned char *>(data), step);
      cv::cvtColor(gray, out, cv::COLOR_GRAY2BGR);
    }
    else
    {
      // Gazebo camera images in the demo are RGB_INT8. If a format name
      // isn't recognized, fall back to the common 3-channel layout.
      cv::Mat rgb(height, width, CV_8UC3,
                  const_cast<unsigned char *>(data), step);
      cv::cvtColor(rgb, out, cv::COLOR_RGB2BGR);
    }
  }
  catch (const std::exception &e)
  {
    gzerr << "[GzSensorMonitor] Gazebo image conversion error: "
          << e.what() << std::endl;
    return {};
  }

  cv::resize(out, out, cv::Size(this->width_, this->height_),
             0, 0, cv::INTER_LINEAR);
  return out;
}

cv::Mat SensorMonitorSystem::ConvertGzDepthImage(
  const gz::msgs::Image &msg,
  double minDepth,
  double maxDepth) const
{
  if (msg.width() == 0 || msg.height() == 0 || msg.data().empty())
    return {};

  if (maxDepth <= minDepth)
    maxDepth = minDepth + 1.0;

  const int width = static_cast<int>(msg.width());
  const int height = static_cast<int>(msg.height());
  const int bpp =
    std::max(1, static_cast<int>(msg.step()) / std::max(1, width));

  const int step =
    msg.step() > 0 ? static_cast<int>(msg.step()) : width * bpp;

  const std::string format =
    gz::msgs::PixelFormatType_Name(msg.pixel_format_type());

  const unsigned char *data =
    reinterpret_cast<const unsigned char *>(msg.data().data());

  cv::Mat depthMeters;

  try
  {
    // Gazebo RGB-D depth messages are commonly R_FLOAT32 / FLOAT32.
    if (format.find("FLOAT32") != std::string::npos ||
        bpp >= 4)
    {
      const size_t required =
        static_cast<size_t>(step) * static_cast<size_t>(height);

      if (msg.data().size() < required)
        return {};

      cv::Mat d(
        height, width, CV_32FC1,
        const_cast<unsigned char *>(data), step);

      d.copyTo(depthMeters);

      static std::atomic<bool> loggedFloat{false};
      if (!loggedFloat.exchange(true))
        gzmsg << "[GzSensorMonitor] depth format=" << format
              << " bpp=" << bpp << " using FLOAT32" << std::endl;
    }
    else if (format.find("UINT16") != std::string::npos ||
             bpp == 2)
    {
      const size_t required =
        static_cast<size_t>(step) * static_cast<size_t>(height);

      if (msg.data().size() < required)
        return {};

      cv::Mat d(
        height, width, CV_16UC1,
        const_cast<unsigned char *>(data), step);

      d.convertTo(depthMeters, CV_32FC1, 0.001);

      static std::atomic<bool> loggedU16{false};
      if (!loggedU16.exchange(true))
        gzmsg << "[GzSensorMonitor] depth format=" << format
              << " bpp=" << bpp << " using UINT16(mm)" << std::endl;
    }
    else if (bpp == 1)
    {
      const size_t required =
        static_cast<size_t>(step) * static_cast<size_t>(height);

      if (msg.data().size() < required)
        return {};

      cv::Mat d(
        height, width, CV_8UC1,
        const_cast<unsigned char *>(data), step);

      d.convertTo(depthMeters, CV_32FC1);

      static std::atomic<bool> loggedU8{false};
      if (!loggedU8.exchange(true))
        gzmsg << "[GzSensorMonitor] depth format=" << format
              << " bpp=" << bpp << " using UINT8" << std::endl;
    }
    else
    {
      gzerr << "[GzSensorMonitor] Unsupported Gazebo depth format="
            << format << " step=" << step
            << " width=" << width << std::endl;
      return {};
    }
  }
  catch (const std::exception &e)
  {
    gzerr << "[GzSensorMonitor] depth conversion error: "
          << e.what() << std::endl;
    return {};
  }

  // Sanitize NaN / inf and clamp to requested physical range.
  cv::Mat finite = cv::Mat::zeros(
    depthMeters.size(), CV_8UC1);

  for (int y = 0; y < depthMeters.rows; ++y)
  {
    const float *row = depthMeters.ptr<float>(y);
    unsigned char *mask = finite.ptr<unsigned char>(y);

    for (int x = 0; x < depthMeters.cols; ++x)
    {
      const float v = row[x];
      mask[x] =
        (std::isfinite(v) && v >= 0.01f) ? 255 : 0;
    }
  }

  depthMeters.setTo(
    static_cast<float>(maxDepth),
    finite == 0);

  cv::min(
    depthMeters,
    static_cast<float>(maxDepth),
    depthMeters);

  cv::max(
    depthMeters,
    static_cast<float>(minDepth),
    depthMeters);

  cv::Mat normalized;
  depthMeters.convertTo(
    normalized,
    CV_8UC1,
    255.0 / (maxDepth - minDepth),
    -minDepth * 255.0 / (maxDepth - minDepth));

  // Keep depth visually unmistakable.
  cv::Mat color;
  cv::applyColorMap(
    normalized, color, cv::COLORMAP_JET);

  cv::resize(
    color, color,
    cv::Size(this->width_, this->height_),
    0, 0, cv::INTER_NEAREST);

  cv::putText(
    color, "DEPTH",
    cv::Point(15, 30),
    cv::FONT_HERSHEY_SIMPLEX,
    0.75,
    cv::Scalar(255, 255, 255),
    2,
    cv::LINE_AA);

  std::ostringstream rangeText;
  rangeText << std::fixed << std::setprecision(1)
            << minDepth << "m - " << maxDepth << "m";

  cv::putText(
    color, rangeText.str(),
    cv::Point(15, this->height_ - 15),
    cv::FONT_HERSHEY_SIMPLEX,
    0.5,
    cv::Scalar(255, 255, 255),
    1,
    cv::LINE_AA);

  return color;
}

cv::Mat SensorMonitorSystem::MakeGzLidarRadar(
  const gz::msgs::LaserScan &scan) const
{
  cv::Mat image(this->height_, this->width_, CV_8UC3,
                cv::Scalar(0, 0, 0));
  const cv::Point center(this->width_ / 2, this->height_ / 2);
  double maxRange = scan.range_max();
  if (!std::isfinite(maxRange) || maxRange <= 0.0)
    maxRange = 10.0;

  const double radius =
    0.42 * static_cast<double>(std::min(this->width_, this->height_));
  const double scale = radius / maxRange;

  for (int i = 1; i <= 4; ++i)
  {
    cv::circle(image, center,
               static_cast<int>(radius * i / 4.0),
               cv::Scalar(0, 70, 0), 1);
  }
  cv::line(image,
           cv::Point(center.x - static_cast<int>(radius), center.y),
           cv::Point(center.x + static_cast<int>(radius), center.y),
           cv::Scalar(0, 70, 0), 1);
  cv::line(image,
           cv::Point(center.x, center.y - static_cast<int>(radius)),
           cv::Point(center.x, center.y + static_cast<int>(radius)),
           cv::Scalar(0, 70, 0), 1);

  cv::circle(image, center, 6, cv::Scalar(255, 255, 255), -1);

  double angle = scan.angle_min();
  for (int i = 0; i < scan.ranges_size(); ++i)
  {
    const double range = scan.ranges(i);
    if (std::isfinite(range) &&
        range >= scan.range_min() && range <= scan.range_max())
    {
      const double d = range * scale;
      const double forward = std::cos(angle) * d;
      const double left = std::sin(angle) * d;
      const int px = center.x + static_cast<int>(left);
      const int py = center.y - static_cast<int>(forward);
      if (px >= 0 && px < this->width_ &&
          py >= 0 && py < this->height_)
      {
        cv::circle(image, cv::Point(px, py), 2,
                   cv::Scalar(0, 255, 0), -1);
      }
    }
    angle += scan.angle_step();
  }

  cv::putText(image, "LIDAR", cv::Point(15, 30),
              cv::FONT_HERSHEY_SIMPLEX, 0.75,
              cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
  std::ostringstream text;
  text << "MAX " << std::fixed << std::setprecision(1)
       << maxRange << " m";
  cv::putText(image, text.str(), cv::Point(15, this->height_ - 15),
              cv::FONT_HERSHEY_SIMPLEX, 0.5,
              cv::Scalar(180, 180, 180), 1, cv::LINE_AA);
  return image;
}

void SensorMonitorSystem::ModeCallback(
  const std_msgs::msg::String::SharedPtr msg)
{
  if (!msg)
    return;

  std::string requested = msg->data;

  if (this->sources_.find(requested) == this->sources_.end())
  {
    gzerr << "[GzSensorMonitor] Unknown source [" << requested << "].\n";
    gzmsg << "[GzSensorMonitor] Available sources:\n";

    for (const auto &[name, cfg] : this->sources_)
      gzmsg << "  " << name << " [" << SourceTypeToString(cfg.type)
            << "]\n";

    return;
  }

  {
    std::lock_guard<std::mutex> lock(this->modeMutex_);
    this->selectedSource_ = requested;
  }

  // Force an immediate publish on the next PostUpdate tick so the
  // screen switches without waiting for the rate-limit interval.
  this->forcePublish_ = true;

  const auto &cfg = this->sources_.at(requested);
  gzmsg << "[GzSensorMonitor] display -> "
        << requested
        << " ("
        << SourceTypeToString(cfg.type)
        << ")"
        << "\n";
}

void SensorMonitorSystem::ImageCallback(
  const std::string &name,
  const sensor_msgs::msg::Image::SharedPtr msg)
{
  if (!msg)
    return;

  cv::Mat image = this->ConvertColorImage(*msg);
  if (image.empty())
    return;

  {
    std::lock_guard<std::mutex> lock(this->dataMutex_);
    this->rgbFrames_[name] = {image, true};
  }

  static std::atomic<bool> reported{false};
  if (!reported.exchange(true))
    gzmsg << "[GzSensorMonitor] first native Gazebo RGB frame received from "
          << name << std::endl;
}

void SensorMonitorSystem::DepthCallback(
  const std::string &name,
  const sensor_msgs::msg::Image::SharedPtr msg)
{
  if (!msg)
    return;

  auto it = this->sources_.find(name);
  double minDepth = this->depthMin_;
  double maxDepth = this->depthMax_;

  if (it != this->sources_.end())
  {
    minDepth = it->second.depthMin;
    maxDepth = it->second.depthMax;
  }

  cv::Mat image =
    this->ConvertDepthImage(*msg, minDepth, maxDepth);

  if (image.empty())
    return;

  std::lock_guard<std::mutex> lock(this->dataMutex_);
  this->depthFrames_[name] = {image, true};
}

void SensorMonitorSystem::LidarCallback(
  const std::string &name,
  const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
  if (!msg)
    return;

  std::lock_guard<std::mutex> lock(this->dataMutex_);
  this->lidarFrames_[name] = msg;
}

cv::Mat SensorMonitorSystem::ConvertColorImage(
  const sensor_msgs::msg::Image &msg) const
{
  if (msg.width == 0 || msg.height == 0)
    return {};

  cv::Mat out;

  try
  {
    if (msg.encoding == "rgb8")
    {
      cv::Mat rgb(
        static_cast<int>(msg.height),
        static_cast<int>(msg.width),
        CV_8UC3,
        const_cast<unsigned char *>(msg.data.data()),
        msg.step);

      cv::cvtColor(rgb, out, cv::COLOR_RGB2BGR);
    }
    else if (msg.encoding == "bgr8")
    {
      cv::Mat bgr(
        static_cast<int>(msg.height),
        static_cast<int>(msg.width),
        CV_8UC3,
        const_cast<unsigned char *>(msg.data.data()),
        msg.step);

      out = bgr.clone();
    }
    else if (msg.encoding == "rgba8")
    {
      cv::Mat rgba(
        static_cast<int>(msg.height),
        static_cast<int>(msg.width),
        CV_8UC4,
        const_cast<unsigned char *>(msg.data.data()),
        msg.step);

      cv::cvtColor(rgba, out, cv::COLOR_RGBA2BGR);
    }
    else if (msg.encoding == "bgra8")
    {
      cv::Mat bgra(
        static_cast<int>(msg.height),
        static_cast<int>(msg.width),
        CV_8UC4,
        const_cast<unsigned char *>(msg.data.data()),
        msg.step);

      cv::cvtColor(bgra, out, cv::COLOR_BGRA2BGR);
    }
    else if (msg.encoding == "mono8")
    {
      cv::Mat mono(
        static_cast<int>(msg.height),
        static_cast<int>(msg.width),
        CV_8UC1,
        const_cast<unsigned char *>(msg.data.data()),
        msg.step);

      cv::cvtColor(mono, out, cv::COLOR_GRAY2BGR);
    }
    else
    {
      gzerr << "[GzSensorMonitor] Unsupported image encoding: "
            << msg.encoding << "\n";
      return {};
    }
  }
  catch (const std::exception &e)
  {
    gzerr << "[GzSensorMonitor] Image conversion error: "
          << e.what() << "\n";
    return {};
  }

  cv::resize(
    out, out, cv::Size(this->width_, this->height_),
    0, 0, cv::INTER_LINEAR);

  return out;
}

cv::Mat SensorMonitorSystem::ConvertDepthImage(
  const sensor_msgs::msg::Image &msg,
  double minDepth,
  double maxDepth) const
{
  if (msg.width == 0 || msg.height == 0)
    return {};

  if (maxDepth <= minDepth)
    maxDepth = minDepth + 1.0;

  cv::Mat depthMeters;

  try
  {
    if (msg.encoding == "32FC1")
    {
      cv::Mat depth(
        static_cast<int>(msg.height),
        static_cast<int>(msg.width),
        CV_32FC1,
        const_cast<unsigned char *>(msg.data.data()),
        msg.step);

      depth.copyTo(depthMeters);
    }
    else if (msg.encoding == "16UC1")
    {
      cv::Mat depth(
        static_cast<int>(msg.height),
        static_cast<int>(msg.width),
        CV_16UC1,
        const_cast<unsigned char *>(msg.data.data()),
        msg.step);

      depth.convertTo(depthMeters, CV_32FC1, 0.001);
    }
    else
    {
      gzerr << "[GzSensorMonitor] Unsupported depth encoding: "
            << msg.encoding << "\n";
      return {};
    }
  }
  catch (...)
  {
    return {};
  }

  cv::Mat normalized;

  cv::Mat finiteMask =
    depthMeters == depthMeters;  // removes NaNs

  depthMeters.setTo(maxDepth, ~finiteMask);

  cv::min(depthMeters, maxDepth, depthMeters);
  cv::max(depthMeters, minDepth, depthMeters);

  depthMeters.convertTo(
    normalized,
    CV_8UC1,
    255.0 / (maxDepth - minDepth),
    -minDepth * 255.0 / (maxDepth - minDepth));

  cv::Mat color;
  cv::applyColorMap(normalized, color, cv::COLORMAP_JET);

  cv::resize(
    color, color, cv::Size(this->width_, this->height_),
    0, 0, cv::INTER_NEAREST);

  cv::putText(
    color, "DEPTH", cv::Point(15, 30),
    cv::FONT_HERSHEY_SIMPLEX, 0.75,
    cv::Scalar(255, 255, 255), 2, cv::LINE_AA);

  return color;
}

cv::Mat SensorMonitorSystem::MakeRgbd(
  const cv::Mat &rgb,
  const cv::Mat &depth) const
{
  if (rgb.empty() || depth.empty())
    return {};

  const int halfWidth = this->width_ / 2;

  cv::Mat left, right;

  cv::resize(
    rgb, left, cv::Size(halfWidth, this->height_));

  cv::resize(
    depth, right, cv::Size(halfWidth, this->height_));

  cv::Mat output(
    this->height_, this->width_, CV_8UC3);

  left.copyTo(
    output(cv::Rect(0, 0, halfWidth, this->height_)));

  right.copyTo(
    output(cv::Rect(halfWidth, 0, halfWidth, this->height_)));

  cv::line(
    output,
    cv::Point(halfWidth, 0),
    cv::Point(halfWidth, this->height_),
    cv::Scalar(255, 255, 255), 2);

  cv::putText(
    output, "RGB", cv::Point(15, 30),
    cv::FONT_HERSHEY_SIMPLEX, 0.7,
    cv::Scalar(255, 255, 255), 2);

  cv::putText(
    output, "DEPTH", cv::Point(halfWidth + 15, 30),
    cv::FONT_HERSHEY_SIMPLEX, 0.7,
    cv::Scalar(255, 255, 255), 2);

  return output;
}

cv::Mat SensorMonitorSystem::MakeLidarRadar(
  const sensor_msgs::msg::LaserScan &scan) const
{
  const int w = this->width_;
  const int h = this->height_;

  cv::Mat image(
    h, w, CV_8UC3, cv::Scalar(0, 0, 0));

  const cv::Point center(w / 2, h / 2);

  double maxRange = scan.range_max;
  if (!std::isfinite(maxRange) || maxRange <= 0.0)
    maxRange = 10.0;

  const double radius =
    0.42 * static_cast<double>(std::min(w, h));

  const double scale = radius / maxRange;

  // Rings
  for (int i = 1; i <= 4; ++i)
  {
    int r = static_cast<int>(radius * i / 4.0);
    cv::circle(
      image, center, r,
      cv::Scalar(0, 70, 0), 1);
  }

  // Crosshair
  cv::line(
    image,
    cv::Point(center.x - radius, center.y),
    cv::Point(center.x + radius, center.y),
    cv::Scalar(0, 70, 0), 1);

  cv::line(
    image,
    cv::Point(center.x, center.y - radius),
    cv::Point(center.x, center.y + radius),
    cv::Scalar(0, 70, 0), 1);

  // Robot
  cv::circle(
    image, center, 6,
    cv::Scalar(255, 255, 255), -1);

  // Heading
  cv::line(
    image,
    center,
    cv::Point(
      center.x,
      center.y - static_cast<int>(radius * 0.12)),
    cv::Scalar(255, 255, 255), 2);

  double angle = scan.angle_min;

  for (const float range : scan.ranges)
  {
    if (!std::isfinite(range) ||
        range < scan.range_min ||
        range > scan.range_max)
    {
      angle += scan.angle_increment;
      continue;
    }

    const double d = range * scale;

    // LaserScan: x forward, y left.
    const double forward = std::cos(angle) * d;
    const double left = std::sin(angle) * d;

    const int px =
      center.x + static_cast<int>(left);

    const int py =
      center.y - static_cast<int>(forward);

    if (px >= 0 && px < w && py >= 0 && py < h)
    {
      cv::circle(
        image,
        cv::Point(px, py),
        2,
        cv::Scalar(0, 255, 0),
        -1);
    }

    angle += scan.angle_increment;
  }

  cv::putText(
    image, "LIDAR", cv::Point(15, 30),
    cv::FONT_HERSHEY_SIMPLEX, 0.75,
    cv::Scalar(255, 255, 255), 2, cv::LINE_AA);

  std::ostringstream text;
  text << "MAX " << std::fixed << std::setprecision(1)
       << scan.range_max << " m";

  cv::putText(
    image, text.str(),
    cv::Point(15, h - 15),
    cv::FONT_HERSHEY_SIMPLEX, 0.5,
    cv::Scalar(180, 180, 180), 1, cv::LINE_AA);

  return image;
}

void SensorMonitorSystem::PublishFrame(
  const cv::Mat &image)
{
  if (image.empty() || !this->framePub_)
    return;

  cv::Mat rgb;

  if (image.channels() == 3)
    cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
  else if (image.channels() == 4)
    cv::cvtColor(image, rgb, cv::COLOR_BGRA2RGB);
  else
    cv::cvtColor(image, rgb, cv::COLOR_GRAY2RGB);

  if (!rgb.isContinuous())
    rgb = rgb.clone();

  gz::msgs::Image msg;

  msg.set_width(
    static_cast<unsigned int>(rgb.cols));

  msg.set_height(
    static_cast<unsigned int>(rgb.rows));

  msg.set_pixel_format_type(
    gz::msgs::PixelFormatType::RGB_INT8);

  msg.set_step(
    static_cast<unsigned int>(rgb.step));

  msg.set_data(
    reinterpret_cast<const char *>(rgb.data),
    rgb.total() * rgb.elemSize());

  this->framePub_.Publish(msg);
}

void SensorMonitorSystem::PostUpdate(
  const gz::sim::UpdateInfo &info,
  const gz::sim::EntityComponentManager &)
{
  if (!this->configured_)
    return;

  // Use wall-clock time for rate limiting so that pausing/unpausing
  // simulation does not cause a burst of frames.
  const auto now = std::chrono::steady_clock::now();

  const double elapsed =
    std::chrono::duration<double>(
      now - this->lastUpdate_).count();

  const bool forced = this->forcePublish_.exchange(false);

  // Skip this tick unless the rate interval has elapsed OR a source
  // switch was requested (forced publish for instant response).
  if (!forced && elapsed < 1.0 / this->updateRate_)
    return;

  this->lastUpdate_ = now;

  std::string selected;

  {
    std::lock_guard<std::mutex> lock(this->modeMutex_);
    selected = this->selectedSource_;
  }

  auto sourceIt = this->sources_.find(selected);

  if (sourceIt == this->sources_.end())
    return;

  const SourceConfig &source = sourceIt->second;

  cv::Mat output;

  {
    std::lock_guard<std::mutex> lock(this->dataMutex_);

    if (source.type == SourceType::IMAGE)
    {
      auto it = this->rgbFrames_.find(source.name);
      if (it == this->rgbFrames_.end() || !it->second.valid)
        return;
      output = it->second.image.clone();
    }
    else if (source.type == SourceType::DEPTH)
    {
      auto it = this->depthFrames_.find(source.name);
      if (it == this->depthFrames_.end() || !it->second.valid)
        return;
      output = it->second.image.clone();
    }
    else if (source.type == SourceType::RGBD)
    {
      auto rgbIt = this->rgbFrames_.find(source.name);
      auto depthIt =
        this->depthFrames_.find(source.name + "__depth");

      if (rgbIt == this->rgbFrames_.end() ||
          depthIt == this->depthFrames_.end() ||
          !rgbIt->second.valid ||
          !depthIt->second.valid)
        return;

      output = this->MakeRgbd(
        rgbIt->second.image,
        depthIt->second.image);
    }
    else if (source.type == SourceType::LIDAR)
    {
      auto gzIt = this->gzLidarFrames_.find(source.name);
      if (gzIt != this->gzLidarFrames_.end() && !gzIt->second.empty())
      {
        output = gzIt->second.clone();
      }
      else
      {
        auto it = this->lidarFrames_.find(source.name);
        if (it == this->lidarFrames_.end() || !it->second)
          return;
        output = this->MakeLidarRadar(*it->second);
      }
    }
  }

  if (!output.empty())
    this->PublishFrame(output);
}

}  // namespace gz_sensor_monitor

GZ_ADD_PLUGIN(
  gz_sensor_monitor::SensorMonitorSystem,
  gz::sim::System,
  gz::sim::ISystemConfigure,
  gz::sim::ISystemPostUpdate
)
