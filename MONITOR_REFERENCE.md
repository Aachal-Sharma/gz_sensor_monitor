v2.1 — White front-facing monitor

The monitor is intentionally a simple, visible in-world monitor:
- white live display surface
- black bezel
- neck and base
- display faces the normal viewing direction
- no black trapezoid / edge-on slab

The display target is still:
  monitor_screen_visual

Sensor camera and all sensor topics are unchanged.

Rebuild:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

Run:
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

Then:
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"
