v2.0.4 — GUI plugin was missing from the world

Your runtime log proves:
- SensorMonitorSystem loaded.
- Native Gazebo subscriptions are OK.
- BUT there is NO line `Loaded plugin [libGzSensorMonitorGui.so]`.
The GUI log stops after SceneManager/3D view, so there is no GUI component
subscribed to `/sensor_monitor/frame` and no rendering-side material updater.

This version explicitly adds:
  <plugin filename="libGzSensorMonitorGui.so"
          name="gz_sensor_monitor::SensorMonitorGui">
    <visual_name>monitor_screen_visual</visual_name>
    <frame_topic>/sensor_monitor/frame</frame_topic>
  </plugin>

Use the same new workspace:
  ~/gz_sensor_monitor_ws

After replacing src/gz_sensor_monitor:

cd ~/gz_sensor_monitor_ws/src/gz_sensor_monitor
bash VERIFY_SOURCE.sh
bash VERIFY_HEADERS.sh
bash VERIFY_WORLD.sh

cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build install log
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

gz sim --force-version 8 -r -v 4   $HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

Expected GUI log:
Loaded plugin [libGzSensorMonitorGui.so]
[GzSensorMonitor GUI] Subscribed to /sensor_monitor/frame
Found visual [sensor_monitor_object::monitor_link::monitor_screen_visual]
Rendering initialized. Visual = monitor_screen_visual
