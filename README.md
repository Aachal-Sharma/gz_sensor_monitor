# GzSensorMonitor

### Real-Time Sensor Feedback Inside Gazebo Harmonic

**GzSensorMonitor** is a Gazebo Harmonic plugin that allows real-time sensor feedback to be displayed directly inside the Gazebo simulation using a virtual monitor.

Instead of switching between Gazebo, RViz, image viewers, or other external visualization tools, sensor data can be viewed directly inside the simulated environment.

<p align="center">
  <img src="docs/images/gz_sensor_monitor_demo.png" width="850">
</p>

---

## 🎯 Why GzSensorMonitor?

Normally, when working with simulated sensors, the workflow looks like:

```text
              Gazebo
                |
        Robot + Sensors
                |
        Sensor Feedback
                |
       +--------+--------+
       |                 |
      RViz          Image Viewer

This means the simulation and sensor feedback are usually viewed separately.

With GzSensorMonitor:
                 Gazebo
                   |
            Robot + Sensors
                   |
                   v
            GzSensorMonitor
                   |
                   v
          ┌─────────────────┐
          │     Monitor     │
          │                 │
          │  Live Sensor    │
          │    Feedback     │
          └─────────────────┘


### ✨ Features
📷 RGB Camera visualization
🎥 RGB-D camera visualization
📏 Depth visualization
📡 LiDAR visualization
🖥️ Virtual monitor inside Gazebo
⚡ Real-time sensor updates
🔄 Runtime sensor switching
🧩 Gazebo GUI + System plugin architecture
🚫 No separate image viewer required
🤖 Designed for robotics simulation and perception development
🐧 ROS 2 Jazzy + Ubuntu 24.04
🌐 Gazebo Harmonic / gz-sim


Runtime Sensor Switching

One of the main features is the ability to change the displayed sensor while Gazebo is running.

For example:

RGB
 ↓
Depth
 ↓
RGB-D
 ↓
LiDAR
 ↓
RGB

The simulation does not need to be restarted for every sensor.

Select RGB
ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgb}"

Select Depth
ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_depth}"


Select LiDAR
ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_lidar}"



⚡ Real-Time Updates

GzSensorMonitor is designed for live sensor visualization.
The sensor data flow is:
Sensor Frame
     |
     v
Gazebo Sensor Callback
     |
     v
Frame Processing
     |
     v
Latest Display Frame
     |
     v
Gazebo GUI
     |
     v
Monitor Texture


The monitor prioritizes the latest available sensor frame.

If sensor data arrives faster than the renderer can display it, intermediate frames can be skipped instead of blocking the sensor pipeline.

This keeps the monitor responsive during simulation.


Architecture

GzSensorMonitor consists of two main components.

                    Gazebo Harmonic
                           |
             +-------------+-------------+
             |                           |
             v                           v
       System Plugin                GUI Plugin
             |                           |
             v                           v
       Sensor Data                  Rendering
             |                           |
             v                           v
       Frame Processing          Monitor Visual
             |                           |
             +-------------+-------------+
                           |
                           v
                    Live Sensor Feed


Requirements

Tested development environment:

Ubuntu 24.04
ROS 2 Jazzy
Gazebo Harmonic
C++17
CMake
colcon

This project targets modern Gazebo (gz-sim) and is not intended for Gazebo Classic.



## 📦 Installation

### 1. Create the workspace

```bash
mkdir -p ~/gz_sensor_monitor_ws/src

2. Clone the repository
cd ~/gz_sensor_monitor_ws/src

git clone https://github.com/Aachal-Sharma/gz_sensor_monitor.git


Source ROS 2 Jazzy
source /opt/ros/jazzy/setup.bash

## 🔨 Build

```bash
cd ~/gz_sensor_monitor_ws

source /opt/ros/jazzy/setup.bash

colcon build --symlink-install \
  --packages-select gz_sensor_monitor

source install/setup.bash

Configure Gazebo Plugin Paths

export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib


.

👨‍💻 Authors

Aachal Sharma
Rahul Gupta

📜 License

See the repository for license information.

⭐ Acknowledgement

GzSensorMonitor is developed around the Gazebo Harmonic and ROS 2 ecosystem for robotics simulation and sensor visualization.


<table>
  <tr>
    <td align="center">
      <img src="docs/images/rgb_feedback.png" width="100%">
      <br>
      <b>RGB Camera Feedback</b>
    </td>
    <td align="center">
      <img src="docs/images/rgbd_feedback.png" width="100%">
      <br>
      <b>RGB-D Camera Feedback</b>
    </td>
  </tr>
</table>
