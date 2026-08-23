#ifndef GZ_SENSOR_MONITOR__MONITOR_DISPLAY_SYSTEM_HPP_
#define GZ_SENSOR_MONITOR__MONITOR_DISPLAY_SYSTEM_HPP_

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <std_msgs/msg/string.hpp>

#include <gz/msgs/image.pb.h>
#include <gz/msgs/laserscan.pb.h>
#include <gz/sim/System.hh>
#include <gz/transport/Node.hh>

#include "gz_sensor_monitor/screen_types.hpp"

namespace gz_sensor_monitor
{

class SensorMonitorSystem final :
  public gz::sim::System,
  public gz::sim::ISystemConfigure,
  public gz::sim::ISystemPostUpdate
{
public:
  SensorMonitorSystem();
  ~SensorMonitorSystem() override;

  void Configure(
    const gz::sim::Entity &entity,
    const std::shared_ptr<const sdf::Element> &sdf,
    gz::sim::EntityComponentManager &ecm,
    gz::sim::EventManager &eventMgr) override;

  void PostUpdate(
    const gz::sim::UpdateInfo &info,
    const gz::sim::EntityComponentManager &ecm) override;

private:
  struct ImageFrame
  {
    cv::Mat image;
    bool valid{false};
  };

  void StartRos();
  void StopRos();

  void AddImageSubscription(const SourceConfig &source);
  void AddDepthSubscription(const SourceConfig &source);
  void AddLidarSubscription(const SourceConfig &source);

  void ImageCallback(
    const std::string &name,
    const sensor_msgs::msg::Image::SharedPtr msg);

  void DepthCallback(
    const std::string &name,
    const sensor_msgs::msg::Image::SharedPtr msg);

  void LidarCallback(
    const std::string &name,
    const sensor_msgs::msg::LaserScan::SharedPtr msg);

  // Native Gazebo Transport callbacks. Gazebo Sim sensors publish
  // gz::msgs directly; these are used even when ros_gz_bridge is absent.
  void GzImageCallback(
    const std::string &name,
    const gz::msgs::Image &msg);

  void GzDepthCallback(
    const std::string &name,
    const gz::msgs::Image &msg);

  void GzLidarCallback(
    const std::string &name,
    const gz::msgs::LaserScan &msg);

  cv::Mat ConvertGzColorImage(
    const gz::msgs::Image &msg) const;

  cv::Mat ConvertGzDepthImage(
    const gz::msgs::Image &msg,
    double minDepth,
    double maxDepth) const;

  cv::Mat MakeGzLidarRadar(
    const gz::msgs::LaserScan &scan) const;

  void ModeCallback(
    const std_msgs::msg::String::SharedPtr msg);

  cv::Mat ConvertColorImage(
    const sensor_msgs::msg::Image &msg) const;

  cv::Mat ConvertDepthImage(
    const sensor_msgs::msg::Image &msg,
    double minDepth,
    double maxDepth) const;

  cv::Mat MakeRgbd(
    const cv::Mat &rgb,
    const cv::Mat &depth) const;

  cv::Mat MakeLidarRadar(
    const sensor_msgs::msg::LaserScan &scan) const;

  void PublishFrame(const cv::Mat &image);


  std::string GetString(
    const std::shared_ptr<const sdf::Element> &sdf,
    const std::string &name,
    const std::string &fallback) const;

  double GetDouble(
    const std::shared_ptr<const sdf::Element> &sdf,
    const std::string &name,
    double fallback) const;

private:
  std::string visualName_{"monitor_screen_visual"};
  std::string modeTopic_{"/screen/mode"};
  std::string frameTopic_{"/gz_sensor_monitor/frame"};

  int width_{640};
  int height_{480};
  double updateRate_{10.0};
  double depthMin_{0.2};
  double depthMax_{5.0};

  std::map<std::string, SourceConfig> sources_;

  std::string selectedSource_;

  std::shared_ptr<rclcpp::Node> node_;
  std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> executor_;
  std::thread rosThread_;
  std::atomic<bool> running_{false};

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr modeSub_;

  std::vector<rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr>
    imageSubs_;

  std::vector<rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr>
    lidarSubs_;

  gz::transport::Node gzNode_;
  gz::transport::Node::Publisher framePub_;

  mutable std::mutex modeMutex_;
  mutable std::mutex dataMutex_;

  // Set to true when source switches so next PostUpdate publishes immediately
  // without waiting for the rate-limit interval to expire.
  std::atomic<bool> forcePublish_{false};

  std::map<std::string, ImageFrame> rgbFrames_;
  std::map<std::string, ImageFrame> depthFrames_;
  std::map<std::string, sensor_msgs::msg::LaserScan::SharedPtr> lidarFrames_;
  std::map<std::string, cv::Mat> gzLidarFrames_;

  std::chrono::steady_clock::time_point lastUpdate_;
  bool configured_{false};
};

}  // namespace gz_sensor_monitor

#endif
