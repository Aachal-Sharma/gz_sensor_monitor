# Virtual Monitor Screen

This demo now has a visible monitor-style display surface in the Gazebo world.

The live texture is rendered onto:
  visual name="monitor_screen_visual"

The surrounding bezel is static:
  screen_bezel_top
  screen_bezel_bottom
  screen_bezel_left
  screen_bezel_right
  screen_stand

The bezel is intentionally separate from `monitor_screen_visual`; the plugin updates
only `monitor_screen_visual`, so the black monitor frame remains visible.

Modes:
  front_rgb
  front_depth
  front_rgbd
  front_lidar

Build:
  cd ~/gz_sensor_monitor_ws
  source /opt/ros/jazzy/setup.bash
  rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
  colcon build --symlink-install --packages-select gz_sensor_monitor
  source install/setup.bash

Run:
  export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  gz sim --force-version 8 -v 4     ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf

Then:
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgb}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_depth}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgbd}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_lidar}"
