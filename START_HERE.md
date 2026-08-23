# gz_sensor_monitor v2.0.1

IMPORTANT:
Do NOT build this package inside ~/gz_sensor_screen_ws.
Use a NEW workspace because the old workspace still contains the previous
gz_sensor_screen package and old libraries.

1) Create the workspace:
  rm -rf ~/gz_sensor_monitor_ws
  mkdir -p ~/gz_sensor_monitor_ws/src

2) Extract this package so that:
  ~/gz_sensor_monitor_ws/src/gz_sensor_monitor/

3) Verify source:
  cd ~/gz_sensor_monitor_ws/src/gz_sensor_monitor
  bash VERIFY_SOURCE.sh

4) Build:
  cd ~/gz_sensor_monitor_ws
  source /opt/ros/jazzy/setup.bash
  rm -rf build install log
  colcon build --symlink-install --packages-select gz_sensor_monitor

5) Source:
  source ~/gz_sensor_monitor_ws/install/setup.bash

6) Paths:
  export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

7) Run:
  gz sim --force-version 8 -r -v 4     ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

8) Switch:
  ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"
  ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_depth}"
  ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgbd}"
  ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_lidar}"
