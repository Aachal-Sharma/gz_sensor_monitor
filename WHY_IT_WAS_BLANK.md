v2.2 — FINAL FIX

ROOT CAUSE FOUND:
The demo world had camera, RGB-D and GPU LiDAR <sensor> elements, but it did
NOT load the Gazebo Sim Sensors system:

  gz-sim-sensors-system
  gz::sim::systems::Sensors

Without this system, the sensors are declared in SDF but their rendering and
publication pipeline is not activated. The monitor plugin therefore correctly
loaded and attached its material, but it never received the first camera frame.

v2.2 adds:

<plugin filename="gz-sim-sensors-system"
        name="gz::sim::systems::Sensors">
  <render_engine>ogre2</render_engine>
</plugin>

The monitor is also moved beside the camera line-of-sight so it cannot block
the camera's view of the red and blue test boxes.

The display remains `monitor_screen_visual` and mode selection remains:
  front_rgb
  front_depth
  front_rgbd
  front_lidar

Build:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

Verify:
cd ~/gz_sensor_monitor_ws/src/gz_sensor_monitor
bash VERIFY_DEMO.sh

Run:
gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

Then:
ros2 topic pub --once /sensor_monitor/mode std_msgs/msg/String "{data: front_rgb}"

IMPORTANT LOG:
After startup you should now see:
  [GzSensorMonitor] first Gazebo RGB frame received from front_rgb

and then:
  [GzSensorMonitor GUI] first display frame received: 640x480

Only after those two lines should the live camera image appear on the monitor.
