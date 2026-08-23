v2.1.1 — Critical camera-feed screen-face fix

The monitor was receiving the runtime material, but the screen plane was
back-facing relative to the normal Gazebo viewing direction.

Previous:
  pitch = +1.5707963
  plane normal +Z -> +X

The monitor is at +X from the sensor/camera object and the user views it from
the -X side. Therefore the display must face -X.

Fixed:
  pitch = -1.5707963
  plane normal +Z -> -X

The screen target remains:
  monitor_screen_visual

No sensor topic, plugin name, frame topic, or source-selection logic was
changed.

Expected result in the white rectangle:
  front_rgb  -> actual camera image, including the red and blue boxes
  front_depth -> depth visualization
  front_rgbd -> RGB + depth side-by-side
  front_lidar -> radar visualization

Build:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

Run:
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

Then:
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"
