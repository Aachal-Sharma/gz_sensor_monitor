# Simple camera -> monitor demo

This is intentionally a minimal Gazebo Harmonic demo:

- one empty floor
- exactly two colored obstacle boxes
- one elevated camera object
- one separate monitor
- camera looks forward at the two boxes
- monitor is beside the camera and faces the user
- `front_rgb` shows the actual camera image
- `front_depth` shows depth
- `front_rgbd` shows RGB + depth
- `front_lidar` shows radar

The Gazebo Sensors system is included, so the camera actually renders.

## Build

cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

## Camera mode

ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"

## Other modes

ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_depth}"
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgbd}"
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_lidar}"
