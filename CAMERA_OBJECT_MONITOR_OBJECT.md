v1.4 — Camera Object + Monitor Object

This is the intended demo architecture:

1. `sensor_camera_object`
   - separate physical camera object
   - RGB camera sensor
   - RGB-D camera sensor
   - LiDAR sensor

2. `sensor_monitor`
   - completely separate physical monitor object
   - `monitor_screen_visual` is the display surface
   - SensorMonitorSystem plugin attached to the monitor

3. Gazebo's normal 3D GUI remains the normal Gazebo Sim GUI.

The sensor camera is elevated (~1.55 m) and slightly pitched downward so
the two test boxes are visible in the camera feed.

The monitor is separate and shows whichever source is selected.

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

Switch monitor:
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgb}"
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_depth}"
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgbd}"
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_lidar}"
