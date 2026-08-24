GzSensorMonitor

Real-Time Sensor Visualization Inside Gazebo Harmonic

GzSensorMonitor is a Gazebo Harmonic plugin for visualizing simulated sensor data directly inside the Gazebo environment through an integrated virtual monitor.

It eliminates the need to switch between Gazebo, RViz, image viewers, or other external visualization tools when inspecting sensor output. Sensor data can be viewed, monitored, and switched at runtime while the simulation is running.

The project is designed for robotics simulation, perception development, sensor debugging, and research workflows using ROS 2 Jazzy, Gazebo Harmonic, and Ubuntu 24.04.

<p align="center">
  <img src="docs/images/depth_monitor.png" width="850" alt="GzSensorMonitor displaying depth sensor data inside Gazebo">
</p>---

Overview

When developing robotic systems in simulation, sensor data is typically generated inside Gazebo and then visualized using external applications.

A conventional workflow looks like this:

                         Gazebo
                           |
                    Robot + Sensors
                           |
                     Sensor Data
                           |
              +------------+------------+
              |                         |
             RViz                Image Viewer

Although this workflow is effective, it requires developers to move between multiple applications during simulation and debugging.

GzSensorMonitor provides an integrated alternative:

                         Gazebo
                           |
                    Robot + Sensors
                           |
                           v
                   GzSensorMonitor
                           |
                           v
              +-------------------------+
              |       Virtual Monitor   |
              |                         |
              |   Live Sensor Feedback  |
              |                         |
              +-------------------------+

Sensor feedback is rendered directly inside Gazebo, keeping the simulation and visualization workflow in one environment.

---

Key Features

- RGB Camera Visualization
- RGB-D Camera Visualization
- Depth Image Visualization
- LiDAR Visualization
- Virtual Sensor Monitor Inside Gazebo
- Real-Time Sensor Updates
- Runtime Sensor Switching
- Gazebo GUI Plugin Integration
- Gazebo System Plugin Integration
- Latest-Frame Display Strategy
- Non-Blocking Sensor Visualization
- ROS 2 Topic-Based Sensor Selection
- No External Image Viewer Required
- Designed for Robotics and Perception Development

---

Supported Environment

GzSensorMonitor is developed and tested with the following environment:

Component| Version
Operating System| Ubuntu 24.04
ROS 2| Jazzy
Gazebo| Harmonic
Gazebo Simulator| "gz-sim"
C++ Standard| C++17
Build System| CMake
Build Tool| colcon

«Important: GzSensorMonitor targets modern Gazebo ("gz-sim") and is not intended for Gazebo Classic.»

---

Architecture

GzSensorMonitor is implemented using two primary Gazebo components:

1. System Plugin — responsible for sensor data acquisition and frame processing.
2. GUI Plugin — responsible for rendering sensor data inside the Gazebo interface.

The overall data flow is:

                         Gazebo Harmonic
                                |
                +---------------+---------------+
                |                               |
                v                               v
         System Plugin                    GUI Plugin
                |                               |
                v                               v
         Sensor Callbacks                   Rendering
                |                               |
                v                               v
         Frame Processing              Monitor Visual
                |                               |
                +---------------+---------------+
                                |
                                v
                       Live Sensor Display

System Plugin

The System Plugin handles the sensor-side processing pipeline.

Its responsibilities include:

- Receiving sensor data from Gazebo.
- Processing incoming sensor frames.
- Managing the currently selected sensor.
- Maintaining the latest available frame.
- Supporting runtime sensor switching.
- Passing processed data to the visualization layer.

GUI Plugin

The GUI Plugin provides the visualization layer inside Gazebo.

Its responsibilities include:

- Creating and managing the virtual monitor.
- Rendering the latest sensor frame.
- Updating the monitor texture.
- Providing live visual feedback inside the Gazebo GUI.

---

Sensor Data Pipeline

The visualization pipeline is designed around the latest available sensor frame:

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

GzSensorMonitor prioritizes displaying the most recent sensor frame rather than processing every incoming frame sequentially.

If sensor data is produced faster than the GUI can render it, intermediate frames can be skipped.

This prevents the visualization pipeline from unnecessarily blocking sensor processing and helps maintain a responsive Gazebo interface during high-frequency simulation.

---

Runtime Sensor Switching

One of the main features of GzSensorMonitor is the ability to change the displayed sensor without restarting Gazebo.

For example:

RGB
 |
 v
Depth
 |
 v
RGB-D
 |
 v
LiDAR
 |
 v
RGB

This allows developers to inspect different sensor modalities during the same simulation run.

Select RGB Camera

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgb}"

Select Depth Camera

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_depth}"

Select LiDAR

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_lidar}"

The selected sensor mode is communicated through the ROS 2 topic:

/sensor_monitor/mode

with message type:

std_msgs/msg/String

---

Installation

Prerequisites

Before installing GzSensorMonitor, make sure the following are available:

- Ubuntu 24.04
- ROS 2 Jazzy
- Gazebo Harmonic
- "colcon"
- CMake
- C++17-compatible compiler
- Git

Make sure ROS 2 Jazzy is installed and available at:

/opt/ros/jazzy

---

1. Create a Workspace

Create a ROS 2 workspace:

mkdir -p ~/gz_sensor_monitor_ws/src

Navigate to the source directory:

cd ~/gz_sensor_monitor_ws/src

---

2. Clone the Repository

Clone the GzSensorMonitor repository:

git clone https://github.com/Aachal-Sharma/gz_sensor_monitor.git

Then navigate to the workspace:

cd ~/gz_sensor_monitor_ws

---

3. Source ROS 2 Jazzy

Before building the package, source ROS 2 Jazzy:

source /opt/ros/jazzy/setup.bash

---

Build

Build the package using "colcon":

cd ~/gz_sensor_monitor_ws

source /opt/ros/jazzy/setup.bash

colcon build --symlink-install \
  --packages-select gz_sensor_monitor

After a successful build, source the workspace:

source install/setup.bash

---

Gazebo Plugin Configuration

GzSensorMonitor contains both a Gazebo System Plugin and a Gazebo GUI Plugin.

The corresponding plugin paths must therefore be available to Gazebo.

System Plugin Path

export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

GUI Plugin Path

export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

For convenience, these paths can be added permanently to your shell configuration:

echo 'export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib' >> ~/.bashrc

echo 'export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib' >> ~/.bashrc

Reload the shell configuration:

source ~/.bashrc

You can verify the configured paths with:

echo $GZ_SIM_SYSTEM_PLUGIN_PATH
echo $GZ_GUI_PLUGIN_PATH

---

Usage

After building the package and configuring the plugin paths, launch your Gazebo Harmonic simulation with the required sensors and GzSensorMonitor configuration.

Once the plugin is loaded, the virtual monitor provides live sensor visualization directly inside the Gazebo GUI.

Sensor selection can be changed while the simulation is running using the ROS 2 topic:

/sensor_monitor/mode

For example:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgb}"

To display the depth sensor:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_depth}"

To display LiDAR:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_lidar}"

---

Example Sensor Modes

Sensor| Mode
RGB Camera| "front_rgb"
Depth Camera| "front_depth"
LiDAR| "front_lidar"

The available sensor names depend on the sensors configured in the Gazebo simulation.

---

Performance Considerations

GzSensorMonitor is designed to provide responsive real-time visualization without unnecessarily blocking the sensor pipeline.

Instead of requiring every incoming frame to be rendered, the monitor maintains the latest available frame.

Conceptually:

Sensor publishes frames:

Frame 1
Frame 2
Frame 3
Frame 4
Frame 5
   |
   v

GUI is currently rendering Frame 3

Instead of waiting for:
Frame 4 -> Frame 5

the monitor can update directly to:

Frame 5

This latest-frame strategy is particularly useful for high-frequency sensors where the simulation can generate data faster than the GUI can display it.

The goal is to keep the visualization responsive while preserving access to the most recent sensor information.

---

Use Cases

GzSensorMonitor can be used in a variety of robotics and simulation workflows, including:

Robotics Simulation

Visualize simulated robot sensors directly inside the simulation environment.

Perception Development

Inspect RGB, depth, RGB-D, and LiDAR data while developing perception pipelines.

Sensor Debugging

Quickly verify whether simulated sensors are producing the expected output.

Autonomous Robotics

Monitor sensor feedback while testing navigation, localization, mapping, and perception systems.

Computer Vision

Inspect camera and depth data without opening an external image visualization application.

Research and Education

Use an integrated sensor visualization workflow for robotics research, demonstrations, and teaching.

---

Project Structure

A typical project structure is:

gz_sensor_monitor/
├── docs/
│   └── images/
│       └── depth_monitor.png
├── include/
├── src/
├── CMakeLists.txt
├── package.xml
└── README.md

The exact source structure may evolve as the project develops.

---

Troubleshooting

Plugin Not Found

If Gazebo cannot find the plugin, verify that the plugin paths are correctly configured:

echo $GZ_SIM_SYSTEM_PLUGIN_PATH
echo $GZ_GUI_PLUGIN_PATH

They should point to:

~/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

If necessary, source the workspace again:

source ~/gz_sensor_monitor_ws/install/setup.bash

---

Build Issues

Make sure ROS 2 Jazzy is sourced before building:

source /opt/ros/jazzy/setup.bash

Then rebuild:

cd ~/gz_sensor_monitor_ws

colcon build --symlink-install \
  --packages-select gz_sensor_monitor

After building:

source install/setup.bash

---

Sensor Mode Does Not Change

Verify that the ROS 2 topic exists:

ros2 topic list | grep sensor_monitor

Check the topic type:

ros2 topic type /sensor_monitor/mode

The expected type is:

std_msgs/msg/String

You can also inspect the topic:

ros2 topic echo /sensor_monitor/mode

---

Development

GzSensorMonitor is intended to serve as a foundation for integrated sensor visualization inside Gazebo Harmonic.

Potential future improvements may include:

- Additional sensor types
- Multiple simultaneous monitors
- Improved visualization controls
- Sensor configuration through the GUI
- Additional rendering modes
- Image processing options
- Visualization overlays
- Extended ROS 2 integration
- Performance optimizations

---

Contributing

Contributions are welcome.

If you would like to improve GzSensorMonitor, you can:

1. Fork the repository.
2. Create a feature branch.
3. Implement your changes.
4. Test the changes with ROS 2 Jazzy and Gazebo Harmonic.
5. Commit your changes.
6. Open a pull request.

Bug reports, feature requests, documentation improvements, and code contributions are all appreciated.

---

Authors

Aachal Sharma
Rahul Gupta

---

Acknowledgements

GzSensorMonitor is developed around the Gazebo Harmonic and ROS 2 ecosystems with the goal of simplifying sensor visualization and improving the development workflow for robotics simulation and perception applications.

---

License
Apache-2.0 — Free to use, modify, distribute.
and this project is released under the **MIT License**.

---
Citation

If you use GzSensorMonitor in your research, project, publication, or other academic work, please cite:

```bibtex
@software{sharma_gz_sensor_monitor,
  author  = {Aachal Sharma and Rahul Gupta},
  title   = {GzSensorMonitor: Real-Time Sensor Feedback Inside Gazebo Harmonic},
  year    = {2026},
  url     = {https://github.com/Aachal-Sharma/gz_sensor_monitor}
}
