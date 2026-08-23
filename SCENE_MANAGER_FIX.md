v2.7 — Restore GzSceneManager as a hidden scene-population helper

Root cause:
The GUI showed:
  render scenes=1
  Waiting for visual [monitor_screen_visual]...

The rendering engine scene existed, but the requested visual was not present
in the client render scene. In Gazebo Sim Harmonic, GzSceneManager is the GUI
plugin that updates the client-side 3D scene from the Entity-Component-Manager
scene information.

v2.7 restores GzSceneManager but minimizes its GUI widget to 1x1 and removes
its title bar, so the normal Gazebo 3D view remains the visible interface.

Expected startup:
  Loaded plugin [GzSceneManager]
  Loaded plugin [libGzSensorMonitorGui.so]
  Found monitor visual in render scene N
  Rendering initialized. Visual = monitor_screen_visual

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
