v1.1.2 — Camera framing fix

The screenshot showed an empty floor because the previous MinimalScene camera
orientation was pointing away from the robot. This version aims the initial
camera at the world origin where the demo robot is located.

New initial camera:
  position = (3.2, -4.8, 2.4)
  rotation = roll 0, pitch -0.32, yaw 2.16 rad

The virtual monitor remains an in-world visual (`monitor_screen_visual`), not a GUI panel.

Run exactly:
  cd ~/gz_sensor_monitor_ws
  source /opt/ros/jazzy/setup.bash
  rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
  colcon build --symlink-install --packages-select gz_sensor_monitor
  source install/setup.bash
  export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  gz sim --force-version 8 -r -v 4     ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf
