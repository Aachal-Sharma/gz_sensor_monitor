v1.3 CLEAN SIMPLE DEMO

This version resets the demo instead of stacking patches on previous worlds.

Design:
- Normal Gazebo Harmonic 3D GUI comes from the user's default GUI config.
- The SDF only adds the custom SensorMonitorGui plugin.
- A standalone `camera_monitor_rig` contains:
    * a separate RGB camera
    * a separate RGB-D camera
    * a separate LiDAR
    * a physical monitor
- The sensor camera is elevated and pitched slightly down to see the boxes.
- The monitor is a normal 3D object in the world.
- The SensorMonitorSystem plugin is attached to the rig model.

Important:
The normal Gazebo 3D camera is NOT forced to a custom pose.
Use the usual Gazebo mouse controls to orbit, pan and zoom.

Build:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

Run:
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf

Switch:
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgb}"
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_depth}"
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgbd}"
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_lidar}"
