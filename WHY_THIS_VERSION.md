v1.1 — Proper Harmonic monitor presentation

The previous demo had two separate problems visible in the user's screenshot:
1) The monitor surface was not visually obvious in the 3D scene.
2) The GzSceneManager GUI panel occupied a large part of the right side.

This version:
- uses a thin BOX as `monitor_screen_visual`, not a one-sided plane
- mounts it on the FRONT of the demo robot body (y = -0.455 m)
- uses a black bezel and rear plate
- keeps the live material target exactly `monitor_screen_visual`
- gives MinimalScene a useful starting camera
- reduces GzSceneManager to a 1x1 floating helper so it does not dominate
  the normal Gazebo Harmonic 3D GUI

This remains Gazebo Sim Harmonic (gz-sim8), Ogre2, ROS 2 Jazzy.

Build:
  cd ~/gz_sensor_monitor_ws
  source /opt/ros/jazzy/setup.bash
  rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
  colcon build --symlink-install --packages-select gz_sensor_monitor
  source install/setup.bash

Run:
  export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  gz sim --force-version 8 -v 4     $HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf

Switch:
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgb}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_depth}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgbd}"
  ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_lidar}"
