v2.8 — Screen was inside the monitor body

Root cause from the rendered screenshot:
- monitor body front surface: local Y = -0.625 m
- screen plane was at local Y = -0.061 m
Therefore the screen plane was physically INSIDE the monitor body, so the
visible black face occluded it. The feed was already reaching the GUI, which
is why the frame-received log was present.

Fix:
- screen plane moved to Y=-0.635 (just in front of body)
- all bezel pieces moved to Y=-0.650
- screen orientation unchanged: it faces the normal viewing direction

No camera, sensor, topic, or texture code changed.

Rebuild:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"
