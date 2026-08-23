v2.9 — Robust two-sided box display

The render log proved the data pipeline works:
- camera RGB frame received
- GUI display frame received
- monitor visual found
- display -> front_rgb

The remaining black monitor was a geometry/orientation problem. The old
display was a one-sided plane while the monitor's visible large face was
oriented on the other axis.

v2.9 changes `monitor_screen_visual` into a thin BOX:
  size = 0.045 x 1.08 x 0.66

The large X-facing box faces are both textured by the same material, so the
live image remains visible from either side of the normal Gazebo 3D camera.
The bezel is also rebuilt around that X-facing display.

No sensor topic or rendering pipeline was changed.

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
