#include "gz_sensor_monitor/sensor_monitor_gui.hpp"

#include <gz/common/Console.hh>
#include <gz/gui/Application.hh>
#include <gz/gui/GuiEvents.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/plugin/Register.hh>
#include <gz/rendering.hh>

namespace gz_sensor_monitor
{

SensorMonitorGui::SensorMonitorGui() = default;

SensorMonitorGui::~SensorMonitorGui() = default;

void SensorMonitorGui::LoadConfig(
  const tinyxml2::XMLElement *pluginElem)
{
  if (pluginElem)
  {
    if (const auto *e =
          pluginElem->FirstChildElement("visual_name"))
    {
      if (e->GetText())
        this->visualName_ = e->GetText();
    }

    if (const auto *e =
          pluginElem->FirstChildElement("frame_topic"))
    {
      if (e->GetText())
        this->frameTopic_ = e->GetText();
    }
  }

  this->subscribed_ =
    this->node_.Subscribe(
      this->frameTopic_,
      &SensorMonitorGui::OnFrame,
      this);

  if (!this->subscribed_)
  {
    gzerr << "[GzSensorMonitor GUI] "
          << "Failed to subscribe to "
          << this->frameTopic_ << std::endl;
    return;
  }

  gzmsg << "[GzSensorMonitor GUI] "
        << "Subscribed to "
        << this->frameTopic_ << std::endl;

  // Gazebo GUI plugins run inside the GUI process.
  // We install an event filter on the main window and perform
  // Ogre2 operations only from the Render event.
  auto *app = gz::gui::App();

  if (!app)
  {
    gzerr << "[GzSensorMonitor GUI] "
          << "gz::gui::App() is null." << std::endl;
    return;
  }

  auto *mainWindow =
    app->findChild<gz::gui::MainWindow *>();

  if (!mainWindow)
  {
    gzerr << "[GzSensorMonitor GUI] "
          << "MainWindow not found." << std::endl;
    return;
  }

  mainWindow->installEventFilter(this);

  gzmsg << "[GzSensorMonitor GUI] "
        << "Render event filter installed." << std::endl;
}

bool SensorMonitorGui::eventFilter(
  QObject *obj,
  QEvent *event)
{
  if (event &&
      event->type() ==
        gz::gui::events::Render::kType)
  {
    this->PerformRenderingOperations();
  }

  return QObject::eventFilter(obj, event);
}

void SensorMonitorGui::OnFrame(
  const gz::msgs::Image &msg)
{
  if (msg.width() == 0 ||
      msg.height() == 0 ||
      msg.data().empty())
  {
    return;
  }

  if (msg.pixel_format_type() !=
      gz::msgs::PixelFormatType::RGB_INT8)
  {
    gzerr << "[GzSensorMonitor GUI] "
          << "Frame is not RGB_INT8." << std::endl;
    return;
  }

  const size_t expected =
    static_cast<size_t>(msg.width()) *
    static_cast<size_t>(msg.height()) * 3u;

  if (msg.data().size() < expected)
    return;

  const unsigned int step =
    msg.step() > 0 ? msg.step() : msg.width() * 3u;

  cv::Mat rgb(
    static_cast<int>(msg.height()),
    static_cast<int>(msg.width()),
    CV_8UC3,
    const_cast<char *>(msg.data().data()),
    step);

  {
    std::lock_guard<std::mutex> lock(
      this->frameMutex_);

    this->pendingFrame_ = rgb.clone();
    this->dirty_ = true;
  }

  static std::atomic<bool> firstFrameLogged{false};
  if (!firstFrameLogged.exchange(true))
  {
    gzmsg << "[GzSensorMonitor GUI] first display frame received: "
          << msg.width() << "x" << msg.height()
          << " format=" << msg.pixel_format_type()
          << std::endl;
  }
}


gz::rendering::VisualPtr SensorMonitorGui::FindScreenVisual()
{
  if (!this->scene_)
    return nullptr;

  // First try the exact name.
  auto visual =
    this->scene_->VisualByName(this->visualName_);

  if (visual)
    return visual;

  // Gazebo Sim scopes visuals by their parent link / model, e.g.
  // screen_robot::base_link::monitor_screen_visual.
  // Search all scene-managed visuals and match the final component.
  const std::string suffix =
    "::" + this->visualName_;

  const unsigned int count =
    this->scene_->VisualCount();

  for (unsigned int i = 0; i < count; ++i)
  {
    auto candidate =
      this->scene_->VisualByIndex(i);

    if (!candidate)
      continue;

    const std::string name =
      candidate->Name();

    if (name == this->visualName_ ||
        (name.size() > suffix.size() &&
         name.compare(
           name.size() - suffix.size(),
           suffix.size(),
           suffix) == 0))
    {
      gzmsg << "[GzSensorMonitor GUI] Found visual ["
            << name << "] for requested visual ["
            << this->visualName_ << "]"
            << std::endl;

      return candidate;
    }
  }

  return nullptr;
}

void SensorMonitorGui::InitializeRendering()
{
  auto *engine =
    gz::rendering::engine("ogre2");

  if (!engine)
    return;

  if (engine->SceneCount() == 0)
    return;

  // Do not assume the first rendering scene is the Gazebo Sim scene.
  // Harmonic can expose more than one Ogre scene while the GUI is starting.
  // Search every initialized scene for the requested monitor visual.
  this->scene_.reset();
  this->visual_.reset();

  for (unsigned int si = 0; si < engine->SceneCount(); ++si)
  {
    auto candidateScene = engine->SceneByIndex(si);
    if (!candidateScene ||
        !candidateScene->IsInitialized() ||
        !candidateScene->RootVisual())
    {
      continue;
    }

    this->scene_ = candidateScene;
    this->visual_ = this->FindScreenVisual();

    if (this->visual_)
    {
      gzmsg << "[GzSensorMonitor GUI] Found monitor visual in render scene "
            << si << std::endl;
      break;
    }
  }

  if (!this->visual_)
  {
    gzdbg << "[GzSensorMonitor GUI] Waiting for visual ["
          << this->visualName_
          << "]; render scenes="
          << engine->SceneCount()
          << std::endl;
    return;
  }

  // A visual created from an SDF <material> may not expose a
  // render::Material through Visual::Material() in every Gazebo
  // Harmonic / Ogre2 scene lifecycle. Create an explicit runtime
  // material when the visual has none, then attach it to the visual.
  this->material_ = this->visual_->Material();

  if (!this->material_)
  {
    if (!this->scene_)
      return;

    this->material_ =
      this->scene_->CreateMaterial("GzSensorMonitorRuntimeMaterial");

    if (!this->material_)
    {
      gzerr << "[GzSensorMonitor GUI] "
            << "Failed to create runtime material."
            << std::endl;
      return;
    }

    // Make the screen behave like a self-lit display.
    this->material_->SetLightingEnabled(false);
    this->material_->SetDiffuse(1.0, 1.0, 1.0, 1.0);
    this->material_->SetAmbient(1.0, 1.0, 1.0, 1.0);
    this->material_->SetEmissive(1.0, 1.0, 1.0, 1.0);

    // This material is dedicated to the screen. Use unique=false so
    // `this->material_` remains the exact material instance assigned to the
    // visual. With unique=true Gazebo clones the material, and subsequent
    // SetTexture calls would modify the unused original instance.
    this->visual_->SetMaterial(this->material_, false);

    // Get the exact material attached to the visual.
    this->material_ = this->visual_->Material();

    gzmsg << "[GzSensorMonitor GUI] Created and attached "
          << "runtime material to visual ["
          << this->visual_->Name() << "]"
          << std::endl;
  }
  else
  {
    // Clone the existing screen material once, then assign the clone without
    // another clone so texture writes go to the material actually on screen.
    auto runtimeMaterial =
      this->material_->Clone("GzSensorMonitorRuntimeMaterial");

    if (!runtimeMaterial)
    {
      gzerr << "[GzSensorMonitor GUI] "
            << "Failed to clone screen material."
            << std::endl;
      return;
    }

    runtimeMaterial->SetLightingEnabled(false);
    runtimeMaterial->SetDiffuse(1.0, 1.0, 1.0, 1.0);
    runtimeMaterial->SetAmbient(1.0, 1.0, 1.0, 1.0);
    runtimeMaterial->SetEmissive(1.0, 1.0, 1.0, 1.0);

    this->visual_->SetMaterial(runtimeMaterial, false);
    this->material_ = this->visual_->Material();
  }

  this->visual_->SetVisible(true);
  this->visual_->SetGeometryMaterial(this->material_, false);

  this->initialized_ = true;

  gzmsg << "[GzSensorMonitor GUI] "
        << "Rendering initialized. Visual = "
        << this->visualName_
        << std::endl;

  gzmsg << "[GzSensorMonitor GUI] "
        << "Rendering initialized. Visual = "
        << this->visualName_
        << std::endl;
}

void SensorMonitorGui::PerformRenderingOperations()
{
  if (!this->initialized_)
  {
    this->InitializeRendering();

    if (!this->initialized_)
      return;
  }

  cv::Mat image;

  {
    std::lock_guard<std::mutex> lock(
      this->frameMutex_);

    if (!this->dirty_)
      return;

    image =
      this->pendingFrame_.clone();

    this->dirty_ = false;
  }

  if (!image.empty())
    this->SetTexture(image);
}

void SensorMonitorGui::SetTexture(
  const cv::Mat &image)
{
  if (!this->material_ ||
      image.empty())
  {
    return;
  }

  cv::Mat rgb;

  if (image.channels() == 3)
  {
    rgb = image;
  }
  else if (image.channels() == 4)
  {
    cv::cvtColor(
      image,
      rgb,
      cv::COLOR_BGRA2RGB);
  }
  else
  {
    cv::cvtColor(
      image,
      rgb,
      cv::COLOR_GRAY2RGB);
  }

  if (!rgb.isContinuous())
    rgb = rgb.clone();

  auto gzImage =
    std::make_shared<gz::common::Image>();

  gzImage->SetFromData(
    rgb.data,
    static_cast<unsigned int>(rgb.cols),
    static_cast<unsigned int>(rgb.rows),
    gz::common::Image::RGB_INT8);

  if (!gzImage->Valid())
    return;

  // Ogre2 caches textures by name. If we reuse the same name every frame,
  // the GPU texture is only uploaded once (first frame) and subsequent
  // SetTexture calls silently return the cached version — this is why the
  // display froze after the first frame. Using a unique name each time
  // forces Ogre2 to treat it as a new texture and upload fresh pixel data.
  const std::string texName =
    "gz_smr_" + std::to_string(this->frameCounter_);
  const std::string emissiveName =
    "gz_sme_" + std::to_string(this->frameCounter_);
  ++this->frameCounter_;

  this->material_->SetTexture(texName, gzImage);
  this->material_->SetEmissiveMap(emissiveName, gzImage);

  if (!this->material_->HasTexture())
  {
    gzerr << "[GzSensorMonitor GUI] Material still has no texture after "
          << "SetTexture()." << std::endl;
  }

  // Keep the live image visible even when the scene lighting is low.
  this->material_->SetEmissive(1.0, 1.0, 1.0, 1.0);
}

}  // namespace gz_sensor_monitor

GZ_ADD_PLUGIN(
  gz_sensor_monitor::SensorMonitorGui,
  gz::gui::Plugin
)
