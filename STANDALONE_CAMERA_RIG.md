v1.2 — Standalone Camera + Monitor Rig

The previous demo mixed the robot, screen, sensors and GUI camera.
This version uses a separate model:

  camera_monitor_rig
    └── camera_mount
         ├── camera body
         ├── RGB camera
         ├── RGB-D camera
         ├── LiDAR
         └── virtual monitor (monitor_screen_visual)

The SensorMonitorSystem plugin is attached to the `camera_monitor_rig` model.
The user-facing Gazebo 3D camera is a completely separate GUI camera.

Thus:
  1. Gazebo's normal 3D camera lets you look at the whole scene.
  2. The physical monitor is part of the camera rig.
  3. The monitor shows RGB / depth / RGB-D / LiDAR.
  4. No GUI panel is used as the display.

Run:
  cd ~/gz_sensor_monitor_ws
  source /opt/ros/jazzy/setup.bash
  rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
  colcon build --symlink-install --packages-select gz_sensor_monitor
  source install/setup.bash
  export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

  gz sim --force-version 8 -r -v 4     ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf

Then:
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgb}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_depth}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgbd}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_lidar}"
