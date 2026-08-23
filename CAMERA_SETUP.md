v2.4 — Invisible sensor camera

There is NO visible camera model. The camera is only a Gazebo sensor.
visualize=false. The sensor is mounted at z=2.20 m so it views the two boxes.
The normal Gazebo GUI / MinimalScene camera is not modified.

Run:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
gz sim --force-version 8 -r -v 4 ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

Then:
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"
