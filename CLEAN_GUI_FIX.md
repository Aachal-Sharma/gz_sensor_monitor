v1.1.1 — CLEAN GAZEBO HARMONIC GUI

The screenshot showed:
- a large Scene Manager panel on the right
- an empty / duplicate side panel
- the 3D view not framed around the demo robot
- no obvious monitor in the initial camera view

Fixes:
1. <include_gui_default_plugins>false</include_gui_default_plugins>
   prevents the default GUI configuration from being merged with this SDF.
2. Only one explicit MinimalScene is used.
3. GzSceneManager is kept only as the client scene-population helper and
   reduced to a 1x1 floating widget.
4. The custom screen GUI plugin does not create a visible panel.
5. MinimalScene gets an explicit camera_pose aimed at the demo robot.
6. WorldControl starts the demo unpaused.

Gazebo documents <camera_pose>, <view_controller>, and the GUI policy for
overriding default plugins in the SDF.

Build:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

Run:
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

gz sim --force-version 8 -r -v 4 ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf

The initial Gazebo window should be a normal large 3D view without the
large Scene Manager panel covering the right side.
