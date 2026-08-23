v2.0.5 — Monitor orientation / geometry fix

The previous screen was built as a box thin in X and wide in Y, while it was
placed at negative Y. That made the monitor appear sideways / like an odd slab
from the normal Gazebo view.

This version:
- makes the display thin in Y and wide in X
- makes the top/bottom bezel wide in X
- places left/right bezel on X edges
- keeps `monitor_screen_visual` as the texture target
- leaves the camera object and sensor topics unchanged

So the monitor is now a normal landscape rectangle facing the Gazebo user's
negative-Y viewing side.

Build:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

Then:
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"
