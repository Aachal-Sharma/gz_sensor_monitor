# GzSensorMonitor — Gazebo Harmonic / ROS 2 Jazzy

A generic virtual screen for a Gazebo robot.

The screen can display any number of configured sources:

- RGB camera image
- depth image
- RGB-D (RGB + colorized depth side-by-side)
- 2D LiDAR radar plot
- additional RGB / depth / LiDAR topics without C++ changes

## 1. Source configuration

Inside the System plugin:

```xml
<sources>

  <source>
    <name>front_rgb</name>
    <type>rgb</type>
    <topic>/camera/image_raw</topic>
  </source>

  <source>
    <name>front_depth</name>
    <type>depth</type>
    <topic>/camera/depth/image_raw</topic>
    <depth_min>0.2</depth_min>
    <depth_max>5.0</depth_max>
  </source>

  <source>
    <name>front_rgbd</name>
    <type>rgbd</type>
    <topic>/camera/image_raw</topic>
    <depth_topic>/camera/depth/image_raw</depth_topic>
    <depth_min>0.2</depth_min>
    <depth_max>5.0</depth_max>
  </source>

  <source>
    <name>front_lidar</name>
    <type>lidar</type>
    <topic>/lidar/scan</topic>
  </source>

</sources>
```

You can add as many `<source>` blocks as required.

## 2. Switch source at runtime

The control topic is:

```text
/screen/mode
```

Examples:

```bash
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgb}"
```

```bash
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_depth}"
```

```bash
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_rgbd}"
```

```bash
ros2 topic pub --once /screen/mode std_msgs/msg/String "{data: front_lidar}"
```

The value is the `<name>` of the source, not the type.

## 3. Build

```bash
cd ~/gz_sensor_monitor_ws

source /opt/ros/jazzy/setup.bash

rm -rf build/gz_sensor_monitor
rm -rf install/gz_sensor_monitor

colcon build --symlink-install --packages-select gz_sensor_monitor
```

Then:

```bash
source install/setup.bash
```

## 4. Plugin paths

```bash
export GZ_SIM_SYSTEM_PLUGIN_PATH=$GZ_SIM_SYSTEM_PLUGIN_PATH:$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

export GZ_GUI_PLUGIN_PATH=$GZ_GUI_PLUGIN_PATH:$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
```

## 5. Run demo

```bash
gz sim -v 4 \
  ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_screen_demo.sdf
```

## 6. ros_gz_bridge

The plugin itself subscribes to ROS 2 topics using rclcpp. Therefore the sensor topics must exist in ROS 2.

If Gazebo sensors are Gazebo Transport topics, bridge them with ros_gz_bridge.

Example config:

```yaml
- ros_topic_name: "/camera/image_raw"
  gz_topic_name: "/camera/image_raw"
  ros_type_name: "sensor_msgs/msg/Image"
  gz_type_name: "gz.msgs.Image"
  direction: GZ_TO_ROS

- ros_topic_name: "/camera/depth/image_raw"
  gz_topic_name: "/camera/depth/image_raw"
  ros_type_name: "sensor_msgs/msg/Image"
  gz_type_name: "gz.msgs.Image"
  direction: GZ_TO_ROS

- ros_topic_name: "/lidar/scan"
  gz_topic_name: "/lidar/scan"
  ros_type_name: "sensor_msgs/msg/LaserScan"
  gz_type_name: "gz.msgs.LaserScan"
  direction: GZ_TO_ROS
```

Run:

```bash
ros2 run ros_gz_bridge parameter_bridge \
  --ros-args \
  -p config_file:=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/config/bridge.yaml
```

## 7. Plugin architecture

```text
Gazebo sensor
     |
     | ros_gz_bridge
     v
ROS 2 topic
     |
     v
GzSensorMonitorSystem
     |
     +---- RGB
     +---- Depth
     +---- RGB-D
     +---- LiDAR
     |
     v
OpenCV 640x480 frame
     |
     v
Gazebo Transport /gz_sensor_monitor/frame
     |
     v
GzSensorMonitorGui
     |
     v
Ogre2 material texture
     |
     v
monitor_screen_visual inside Gazebo world
```

The server System performs sensor conversion at the configured update rate. The GUI plugin receives the generated frame and changes the material texture from the Gazebo GUI render event.

## 8. Important

The SDF visual must be named exactly:

```xml
<visual name="monitor_screen_visual">
```

or the plugin parameter must be changed:

```xml
<visual_name>your_visual_name</visual_name>
```

For a robot model, `monitor_screen_visual` should be a visual inside one of the robot's links.

The demo world is only a test world. For your existing humanoid / robot SDF, keep your existing sensors and simply add the screen visual and the plugin configuration.
