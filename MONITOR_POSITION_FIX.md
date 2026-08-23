v1.0.10 Monitor Position Fix

The previous monitor was placed at x=0 while the robot body is 1.2 m long,
so the vertical screen was inside the body and could not be seen.

This version:
- moves monitor_screen_visual to x=0.675 m, outside the body
- makes the plane vertical using normal 1 0 0
- moves the bezel to surround the display
- adds a rear plate and a small status indicator
- keeps the plugin target exactly: monitor_screen_visual

Rebuild:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

Run:
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
gz sim --force-version 8 -v 4 ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf

The monitor should now be physically outside the robot body and visible in the
3D world. The live texture is still applied only to monitor_screen_visual.
