v1.5 — Runtime texture pointer fix

ROOT CAUSE:
Gazebo Rendering Visual::SetMaterial(material, true) clones the material when
`unique=true`. The previous plugin stored the original material pointer and
then called SetTexture() on it. The visual was displaying the cloned material,
so the live texture was written to the wrong material instance.

FIX:
- create/clone a dedicated runtime material
- attach it with SetMaterial(runtimeMaterial, false)
- refresh the material pointer using visual_->Material()
- call SetGeometryMaterial(..., false)
- verify HasTexture() after SetTexture()
- log the first frame received by the GUI plugin

Gazebo Rendering's Visual API documents that SetMaterial(..., unique=true)
clones the supplied material. Therefore the screen must use unique=false when
the plugin intends to mutate the same material after attachment.

Build:
cd ~/gz_sensor_monitor_ws
source /opt/ros/jazzy/setup.bash
rm -rf build/gz_sensor_monitor install/gz_sensor_monitor
colcon build --symlink-install --packages-select gz_sensor_monitor
source install/setup.bash

Run:
export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

gz sim --force-version 8 -r -v 4   ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf

Expected:
[GzSensorMonitor GUI] first display frame received: 640x480 ...
and no:
Material still has no texture after SetTexture().

Then:
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgb}"
