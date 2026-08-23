#ifndef GZ_SENSOR_MONITOR__MONITOR_DISPLAY_GUI_HPP_
#define GZ_SENSOR_MONITOR__MONITOR_DISPLAY_GUI_HPP_

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

#include <opencv2/opencv.hpp>

#include <gz/common/Image.hh>
#include <gz/gui/Plugin.hh>
#include <gz/msgs/image.pb.h>
#include <gz/rendering/Material.hh>
#include <gz/rendering/Scene.hh>
#include <gz/rendering/Visual.hh>
#include <gz/transport/Node.hh>

namespace gz_sensor_monitor
{

class SensorMonitorGui final : public gz::gui::Plugin
{
public:
  SensorMonitorGui();
  ~SensorMonitorGui() override;

  void LoadConfig(
    const tinyxml2::XMLElement *pluginElem) override;

protected:
  bool eventFilter(
    QObject *obj,
    QEvent *event) override;

private:
  void InitializeRendering();
  gz::rendering::VisualPtr FindScreenVisual();
  void OnFrame(const gz::msgs::Image &msg);
  void PerformRenderingOperations();
  void SetTexture(const cv::Mat &image);

private:
  std::string visualName_{"monitor_screen_visual"};
  std::string frameTopic_{"/gz_sensor_monitor/frame"};

  gz::transport::Node node_;
  bool subscribed_{false};

  gz::rendering::ScenePtr scene_;
  gz::rendering::VisualPtr visual_;
  gz::rendering::MaterialPtr material_;

  cv::Mat pendingFrame_;
  std::mutex frameMutex_;

  bool dirty_{false};
  bool initialized_{false};

  // Monotonically increasing counter used to generate a unique texture
  // name for every frame. gz-rendering / Ogre2 caches textures by name,
  // so re-using the same name causes the GPU texture to never be updated
  // after the first upload — this counter bypasses that cache.
  uint64_t frameCounter_{0};
};

}  // namespace gz_sensor_monitor

#endif
