#ifndef GZ_SENSOR_MONITOR__MONITOR_TYPES_HPP_
#define GZ_SENSOR_MONITOR__MONITOR_TYPES_HPP_

#include <string>

namespace gz_sensor_monitor
{

enum class SourceType
{
  IMAGE,
  DEPTH,
  RGBD,
  LIDAR,
  UNKNOWN
};

struct SourceConfig
{
  std::string name;
  SourceType type{SourceType::UNKNOWN};

  // Main topic:
  // IMAGE/DEPTH/LIDAR -> sensor topic
  // RGBD -> RGB image topic
  std::string topic;

  // Only used by RGBD.
  std::string depthTopic;

  double depthMin{0.2};
  double depthMax{5.0};
};

inline SourceType SourceTypeFromString(const std::string &type)
{
  if (type == "image" || type == "rgb" || type == "camera")
    return SourceType::IMAGE;

  if (type == "depth")
    return SourceType::DEPTH;

  if (type == "rgbd")
    return SourceType::RGBD;

  if (type == "lidar" || type == "laser" || type == "laser_scan")
    return SourceType::LIDAR;

  return SourceType::UNKNOWN;
}

inline std::string SourceTypeToString(SourceType type)
{
  switch (type)
  {
    case SourceType::IMAGE: return "image";
    case SourceType::DEPTH: return "depth";
    case SourceType::RGBD: return "rgbd";
    case SourceType::LIDAR: return "lidar";
    default: return "unknown";
  }
}

}  // namespace gz_sensor_monitor

#endif
