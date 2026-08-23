v2.6 — Harmonic render-scene lookup fix

Observed problem:
  [GzSensorMonitor GUI] Waiting for visual [monitor_screen_visual]...

The monitor exists in the simulation scene, but the GUI plugin was assuming
SceneByIndex(0) was always the populated Gazebo Sim render scene. Harmonic can
have multiple rendering scenes during GUI startup.

v2.6 searches every initialized rendering scene until it finds:
  monitor_screen_visual

Expected logs:
  Found monitor visual in render scene N
  Rendering initialized. Visual = monitor_screen_visual
  first display frame received: 640x480 format=...

The normal Gazebo GUI remains unchanged. The camera is still an invisible
sensor-only object and the physical monitor is still the only display.

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
