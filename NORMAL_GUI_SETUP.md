v2.5 — Normal Gazebo window

This version intentionally removes the Scene Manager and hides the custom
SensorMonitor GUI widget. The only visible GUI is Gazebo's normal MinimalScene
3D view.

The custom GUI library still loads invisibly because it is needed to copy the
live sensor frame onto the physical monitor visual.

World:
- 2 boxes
- invisible elevated camera sensor
- physical monitor in the world
- no visible camera body
- no Scene Manager panel
- no blank SensorMonitor panel

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
