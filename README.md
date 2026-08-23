GzSensorMonitor

Real-Time Multi-Sensor Visualization Inside Gazebo Harmonic

GzSensorMonitor is a Gazebo Harmonic plugin for visualizing real-time sensor feedback directly inside the Gazebo simulation environment.

It is designed for robotics simulation, perception development, sensor debugging, demonstrations, and research where the user needs to observe both:

the simulated physical environment, and

the data being produced by the robot's sensors.

The plugin provides a virtual monitor inside the Gazebo world and can display different sensor modalities such as:

RGB camera

RGB-D camera

Depth

LiDAR

The display source can be changed while the simulation is running.

1. Motivation

In a conventional robotics simulation workflow, Gazebo is used to simulate the robot, sensors, environment, physics, and rendering. However, sensor feedback is often inspected through separate tools.

A typical workflow can look like:

                     Gazebo
                       |
              Robot + Sensors
                       |
                Sensor Topics
                       |
          +------------+------------+
          |                         |
         RViz                Image Viewer

For example, a developer may keep Gazebo open to observe the robot and environment while opening another window to inspect a camera image or another visualization tool to inspect LiDAR or depth data.

This works, but it creates an unnecessary separation between:

What exists in the simulated world

and

What the simulated robot actually senses

This becomes especially inconvenient during:

camera placement experiments

sensor orientation testing

field-of-view validation

obstacle detection

RGB-D perception

robotic manipulation

navigation

sensor debugging

demonstrations

teaching

embodied AI experiments

2. What GzSensorMonitor Provides

GzSensorMonitor brings the sensor visualization into the simulated environment.

Instead of requiring the user to leave Gazebo to inspect sensor feedback, a virtual monitor can be placed inside the Gazebo world.

The overall concept is:

                         Gazebo Simulation
                                |
                 +--------------+--------------+
                 |                             |
               Robot                         Sensors
                                               |
                             +-----------------+----------------+
                             |                 |                |
                            RGB              RGB-D            LiDAR
                             |                 |                |
                             +-----------------+----------------+
                                               |
                                        GzSensorMonitor
                                               |
                                               v
                                      +----------------+
                                      | Virtual Monitor|
                                      |                |
                                      | Live Feedback  |
                                      +----------------+

The monitor becomes another object in the simulated environment.

This means the user can simultaneously observe:

the robot

the simulated environment

obstacles

the sensor itself

the sensor's field of view

the sensor feedback

3. Main Idea

The central idea of the project is:

Bring sensor feedback closer to the simulated robot and simulated world.

For example, suppose a camera is mounted above the ground and points toward two obstacles.

The Gazebo world contains:

                  Camera
                    |
                    v

             +------------+
             |  Obstacle  |
             +------------+

                         +------------+
                         |  Obstacle  |
                         +------------+

The same camera produces an RGB image.

GzSensorMonitor displays that image on a virtual monitor:

        +--------------------------+
        |                          |
        |       Camera Feed        |
        |                          |
        |       Obstacle           |
        |                          |
        |                  Obstacle|
        |                          |
        +--------------------------+

Therefore the user can see both:

Physical simulation view
        +
Sensor's point of view

inside the same simulation.

4. Applications

4.1 Real-Time Camera Monitoring

The most direct application is displaying a simulated camera feed on a virtual monitor.

A Gazebo camera produces an image stream:

Camera
  |
  v
RGB Image
  |
  v
GzSensorMonitor
  |
  v
Virtual Monitor

This can be used to verify:

whether the camera sees the intended objects

whether obstacles are inside the field of view

whether the camera is too high or too low

whether the camera orientation is correct

whether objects are occluded

whether the simulated perception input looks correct

4.2 Camera Placement and Field-of-View Testing

Camera placement is critical in robotics.

A small change in camera pose can significantly change the information available to a perception system.

For example:

Camera position A

        Camera
           |
           v
     +-----------+
     | Obstacle  |
     +-----------+

After changing the camera pose:

Camera position B

             Camera
                |
                v
        +-----------+
        | Obstacle  |
        +-----------+

The monitor provides immediate visual feedback about the resulting camera view.

This is useful when designing:

mobile robot sensor configurations

robotic arm eye-in-hand cameras

autonomous navigation sensors

manipulation perception systems

inspection systems

4.3 RGB Perception Development

The RGB display can be used as a direct debugging interface for computer vision systems.

A typical pipeline may be:

RGB Camera
    |
    v
GzSensorMonitor
    |
    +----------------------+
    |                      |
    v                      v
Human Visualization    Perception Pipeline
                           |
                           +--> Object Detection
                           |
                           +--> Segmentation
                           |
                           +--> Tracking
                           |
                           +--> Scene Understanding

The developer can therefore compare:

Actual Gazebo scene
        vs.
Camera observation

4.4 RGB-D Perception

RGB-D cameras provide both appearance and depth.

The conceptual pipeline is:

                  RGB-D Camera
                       |
             +---------+---------+
             |                   |
          RGB Image         Depth Image
             |                   |
             +---------+---------+
                       |
                GzSensorMonitor
                       |
                       v
                 Virtual Monitor

RGB-D data is particularly useful for:

3D object perception

point-cloud generation

grasp estimation

manipulation

obstacle avoidance

spatial reasoning

depth-based segmentation

For a robotic manipulation system, for example:

RGB-D Camera
      |
      v
Object Detection
      |
      v
Segmentation
      |
      v
Depth Mask
      |
      v
Point Cloud
      |
      v
3D Perception
      |
      v
Grasp / Motion Planning

GzSensorMonitor provides a direct way to inspect the sensor input during this process.

4.5 Depth Visualization

Depth data is fundamentally different from RGB data.

An RGB image contains color information:

R G B

while a depth image represents distance from the sensor.

For example:

Camera
   |
   |---- Near object
   |
   |---------- Far object

The plugin can convert floating-point depth data such as:

R_FLOAT32

into a displayable depth visualization.

This is useful for checking:

depth sensor range

distance to obstacles

missing depth values

depth discontinuities

camera placement

RGB-D perception

4.6 LiDAR Visualization

A LiDAR sensor produces range measurements rather than a normal RGB image.

Conceptually:

                / | \
              /   |   \
            /     |     \
          /       |       \
        -------- ROBOT --------
          \       |       /
            \     |     /
              \   |   /
                \ | /

The monitor can be used to visualize LiDAR information in a form that can be inspected during simulation.

This is useful for:

obstacle detection

sensor coverage

blind-spot analysis

scan-range testing

navigation debugging

sensor orientation validation

4.7 In-Simulator Debugging

One of the main goals of the project is to reduce the need for multiple windows during sensor debugging.

Without GzSensorMonitor:

Gazebo
  +
RViz
  +
Image Viewer
  +
Terminal

With GzSensorMonitor:

                  Gazebo

        +-------------------------+
        |                         |
        |       Robot             |
        |                         |
        |      Sensors            |
        |                         |
        |             +---------+ |
        |             | Monitor | |
        |             |         | |
        |             | Sensor  | |
        |             | Feedback| |
        |             +---------+ |
        |                         |
        +-------------------------+

The objective is not to replace RViz.

RViz remains extremely useful for:

TF visualization

robot models

point clouds

maps

paths

navigation data

complex robotics visualization

GzSensorMonitor provides a different capability:

Sensor feedback as part of the simulated environment itself.

5. Supported Sensor Modes

The demonstration configuration uses logical sensor sources such as:

Mode

Purpose

front_rgb

RGB camera feedback

front_depth

Depth visualization

front_rgbd

RGB-D feedback

front_lidar

LiDAR visualization

The exact Gazebo topic names can be inspected with:

gz topic -l

6. Real-Time Mode Switching

A key feature is runtime sensor switching.

The simulation does not need to be restarted when changing the displayed sensor.

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

The mode is selected through:

/sensor_monitor/mode

using:

std_msgs/msg/String

RGB

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgb}"

Depth

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_depth}"

RGB-D

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgbd}"

LiDAR

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_lidar}"

The sensors can continue generating data while the displayed source changes.

7. Real-Time Display Architecture

The display path is designed around incoming sensor callbacks.

A traditional visualization approach could look like:

Sensor Frame
     |
     v
Cache
     |
     v
Periodic Update
     |
     v
Display

This introduces an additional update loop that can limit display responsiveness.

GzSensorMonitor instead uses a callback-driven path:

Sensor Frame
     |
     v
Gazebo Sensor Callback
     |
     v
Frame Conversion
     |
     v
Display Update
     |
     v
Virtual Monitor

The design prioritizes:

Fresh data and responsiveness over guaranteed display of every incoming frame.

If the rendering side cannot keep up with the sensor stream, dropping intermediate frames is acceptable for a real-time monitor.

The goal is to avoid blocking the sensor pipeline simply because the monitor cannot render every frame.

8. RGB Data Flow

The RGB pipeline can be represented as:

             Gazebo Camera
                    |
                    v
             Gazebo Image
                    |
                    v
           GzSensorMonitor
                    |
            +-------+-------+
            |               |
            v               v
        Frame Cache     Display Path
                            |
                            v
                     Virtual Monitor

The plugin receives the Gazebo image message, converts it into a displayable representation, and sends the latest frame to the monitor.

9. Depth Data Flow

The depth pipeline is:

            Gazebo Depth Sensor
                    |
                    v
             Depth Image
                    |
                    v
              R_FLOAT32
                    |
                    v
           Depth Conversion
                    |
                    v
           Displayable Frame
                    |
                    v
             Virtual Monitor

The implementation handles floating-point depth data rather than assuming that depth is an ordinary 8-bit RGB image.

10. RGB-D Data Flow

RGB-D requires two related streams:

              RGB-D Sensor
                   |
          +--------+--------+
          |                 |
          v                 v
       RGB Frame        Depth Frame
          |                 |
          +--------+--------+
                   |
                   v
             GzSensorMonitor
                   |
                   v
             RGB-D Display

The plugin maintains the latest available data from the relevant streams and uses it for visualization.

11. LiDAR Data Flow

The LiDAR pipeline is:

              Gazebo LiDAR
                    |
                    v
             Laser Scan Data
                    |
                    v
           GzSensorMonitor
                    |
                    v
          Visualization Frame
                    |
                    v
             Virtual Monitor

12. Virtual Monitor Concept

The monitor is a Gazebo visual rather than a separate operating-system window.

Conceptually:

Gazebo Model
     |
     +--- Monitor Link
              |
              +--- Monitor Visual
                       |
                       +--- Runtime Material
                                |
                                +--- Sensor Texture

The GUI plugin finds the monitor visual and attaches a runtime rendering material/texture.

This is what allows the sensor data to appear on an actual object inside the simulated world.

13. System Plugin and GUI Plugin

The project is divided into two major responsibilities.

13.1 System Plugin

The system-side plugin is responsible for sensor data processing.

Main responsibilities:

sensor subscriptions

RGB frame reception

depth frame reception

LiDAR frame reception

sensor data conversion

frame caching

mode selection

display frame generation

Main source:

src/sensor_monitor_system.cpp

Header:

include/gz_sensor_monitor/sensor_monitor_system.hpp

13.2 GUI Plugin

The GUI-side plugin is responsible for the actual Gazebo rendering.

Main responsibilities:

finding the monitor visual

initializing rendering

creating the runtime display material

attaching the texture to the monitor

receiving display frames

updating the monitor texture

Main source:

src/sensor_monitor_gui.cpp

Header:

include/gz_sensor_monitor/sensor_monitor_gui.hpp

14. Gazebo Rendering Architecture

The monitor uses Gazebo's rendering system.

The conceptual pipeline is:

Sensor Data
     |
     v
System Plugin
     |
     v
Display Frame
     |
     v
GUI Plugin
     |
     v
Gazebo Rendering
     |
     v
Monitor Visual

The sensor data is therefore not simply printed in a terminal or displayed in a separate application.

It becomes a texture associated with a Gazebo visual.

15. Example Demo Environment

The demonstration world is intended to provide a simple environment for testing the plugin.

The environment contains:

a Gazebo scene

a sensor object

camera sensors

RGB-D sensing

LiDAR

obstacles

a virtual monitor

GzSensorMonitor system plugin

GzSensorMonitor GUI plugin

The sensor object is kept separate from the monitor concept.

The sensor object represents the simulated hardware.

The monitor represents the display used to visualize its output.

Conceptually:

             Sensor Object
                  |
          +-------+-------+
          |       |       |
        RGB     RGB-D   LiDAR
          |       |       |
          +-------+-------+
                  |
                  v
           GzSensorMonitor
                  |
                  v
           Monitor Object

16. Sensor Object vs Monitor

The project intentionally separates:

Sensor

from:

Monitor

The sensor object can contain multiple sensor plugins.

For example:

Sensor Object
     |
     +--- RGB Camera
     |
     +--- RGB-D Camera
     |
     +--- LiDAR

The monitor is independent:

Monitor Object
     |
     +--- Display Visual

This makes the architecture extensible.

Additional sensors can be added to the sensor object without changing the basic monitor concept.

17. Requirements

The project targets the modern Gazebo stack.

Recommended environment:

Ubuntu 24.04
ROS 2 Jazzy
Gazebo Harmonic

The project is designed for gz-sim / modern Gazebo rather than Gazebo Classic.

18. Dependencies

The project uses components from:

Gazebo Sim / gz-sim

Gazebo Transport

Gazebo Messages

Gazebo Rendering

Gazebo GUI

ROS 2

OpenCV

CMake

colcon

The exact package dependencies are declared in:

package.xml

and build configuration is defined in:

CMakeLists.txt

19. Installation

Create a workspace:

mkdir -p ~/gz_sensor_monitor_ws/src

Go to the source directory:

cd ~/gz_sensor_monitor_ws/src

Clone the repository:

git clone https://github.com/Aachal-Sharma/gz_sensor_monitor.git

Source ROS 2 Jazzy:

source /opt/ros/jazzy/setup.bash

20. Build

Go to the workspace root:

cd ~/gz_sensor_monitor_ws

Build:

colcon build --symlink-install \
  --packages-select gz_sensor_monitor

After a successful build:

source install/setup.bash

21. Gazebo Plugin Paths

Set the system plugin path:

export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

Set the GUI plugin path:

export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib

These variables allow Gazebo to locate the compiled GzSensorMonitor libraries.

22. Running the Demo

Run the included demonstration world:

gz sim --force-version 8 -r -v 4 \
~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf

The exact installed world path can also be inspected with:

find ~/gz_sensor_monitor_ws/install/gz_sensor_monitor \
  -name "sensor_monitor_demo.sdf"

23. Checking Sensor Topics

List Gazebo topics:

gz topic -l

Typical camera-related topics can include:

/camera/image_raw
/camera/rgbd/image
/camera/rgbd/depth_image
/camera/rgbd/points

The exact names depend on the sensor configuration in the SDF world.

24. Checking RGB Data

For an RGB camera topic:

gz topic -e -t /camera/image_raw

This can be used to verify that the camera is publishing data.

25. Checking RGB-D Data

For RGB-D RGB data:

gz topic -e -t /camera/rgbd/image

For depth:

gz topic -e -t /camera/rgbd/depth_image

For point cloud:

gz topic -e -t /camera/rgbd/points

26. Checking LiDAR

For a LiDAR scan:

gz topic -e -t /camera/lidar/scan

The exact topic depends on the sensor configuration.

27. Testing RGB

Start Gazebo and select:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgb}"

The monitor should display the RGB camera feedback.

28. Testing RGB-D

Select:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgbd}"

The monitor should display the RGB-D visualization.

This is useful for verifying that the RGB-D sensor is producing data and that the monitor is receiving it.

29. Testing Depth

Select:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_depth}"

The plugin converts the incoming depth data into a displayable representation.

30. Testing LiDAR

Select:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_lidar}"

The monitor should switch to the LiDAR visualization.

31. Switching Sensors Without Restarting

A typical test sequence is:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgb}"

then:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_depth}"

then:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_rgbd}"

then:

ros2 topic pub --once /sensor_monitor/mode \
std_msgs/msg/String "{data: front_lidar}"

The purpose of this mechanism is to let the user inspect different sensor modalities from the same monitor.

32. Performance Design

The monitor is intended for interactive real-time visualization.

The design therefore favors:

Latest frame
     >
Every frame

in terms of priority.

For example, if a camera produces:

Frame 1
Frame 2
Frame 3
Frame 4
Frame 5

but the renderer cannot display every frame, it is preferable to display:

Frame 1
Frame 3
Frame 5

rather than blocking the sensor callback while waiting for the renderer.

This approach is useful for interactive simulation because the displayed information remains recent.

33. Debugging

If the monitor is not displaying a sensor stream, debug the pipeline from the sensor upward.

Step 1: Check Gazebo topics

gz topic -l

Step 2: Check whether the sensor publishes

For RGB:

gz topic -e -t /camera/image_raw

For depth:

gz topic -e -t /camera/rgbd/depth_image

For LiDAR:

gz topic -e -t /camera/lidar/scan

If there is no data here, the problem is on the sensor/world side rather than the monitor.

Step 3: Check the plugin logs

The system plugin reports sensor reception and selected display mode.

Messages can indicate events such as:

first Gazebo RGB frame received
first Gazebo DEPTH frame received
first Gazebo LiDAR frame received
display -> front_rgb

These messages help determine which stage of the pipeline is working.

Step 4: Check the monitor visual

The GUI plugin searches for the configured monitor visual.

A typical message is:

Found visual [sensor_monitor_object::monitor_link::monitor_screen_visual]

If the visual cannot be found, check the SDF model and visual name.

34. Common Problems

Problem: Monitor is black

Possible causes:

monitor visual was not found

GUI plugin did not initialize

sensor data is not arriving

display topic is not receiving frames

runtime material was not created

Check the Gazebo GUI logs.

Problem: RGB works but depth does not

Check the depth topic:

gz topic -l | grep depth

Then:

gz topic -e -t <depth-topic>

Verify that the sensor is publishing floating-point depth data.

Problem: Mode changes but display does not update

Check:

ros2 topic echo /sensor_monitor/mode

Then verify that the selected sensor is actually publishing.

Problem: Camera topic exists but has no data

A topic appearing in:

gz topic -l

does not by itself guarantee that frames are being generated.

Check:

gz topic -e -t <camera-topic>

35. Project Structure

gz_sensor_monitor/
│
├── CMakeLists.txt
├── package.xml
├── README.md
│
├── include/
│   └── gz_sensor_monitor/
│       ├── screen_types.hpp
│       ├── sensor_monitor_gui.hpp
│       └── sensor_monitor_system.hpp
│
├── src/
│   ├── sensor_monitor_system.cpp
│   └── sensor_monitor_gui.cpp
│
├── qml/
│   ├── gz_sensor_monitor.qrc
│   └── libGzSensorMonitorGui.so.qml
│
├── config/
│   └── bridge.yaml
│
└── worlds/
    └── sensor_monitor_demo.sdf

36. Source Code Responsibilities

sensor_monitor_system.cpp

Contains the server-side Gazebo system plugin.

It handles:

sensor discovery/configuration

Gazebo transport subscriptions

RGB processing

depth processing

LiDAR processing

frame storage

mode callbacks

display-frame generation

sensor_monitor_gui.cpp

Contains the Gazebo GUI plugin.

It handles:

GUI initialization

render scene access

monitor visual discovery

runtime material creation

texture updates

display rendering

sensor_monitor_system.hpp

Defines the system plugin class and its internal interfaces.

sensor_monitor_gui.hpp

Defines the GUI plugin class and rendering interfaces.

screen_types.hpp

Contains types shared by the monitor/display implementation.

sensor_monitor_demo.sdf

Contains the demonstration simulation environment and plugin configuration.

37. Design Advantages

37.1 No Separate Sensor Window

Sensor feedback can be viewed from inside Gazebo.

37.2 Same Simulation Context

The sensor output and physical scene are visible together.

37.3 Easier Sensor Debugging

Camera and sensor problems can be identified visually.

37.4 Multiple Sensor Modalities

The same monitor can be used for multiple sensor types.

37.5 Runtime Switching

The displayed source can change without restarting the simulation.

37.6 Extensible Architecture

New visualization modes can be added later.

38. Research Applications

GzSensorMonitor can be useful in research involving:

Autonomous Navigation

Camera / LiDAR
       |
       v
Perception
       |
       v
Obstacle Detection
       |
       v
Planning
       |
       v
Control

Robotic Manipulation

RGB-D Camera
       |
       v
Object Detection
       |
       v
Segmentation
       |
       v
Point Cloud
       |
       v
Grasp Estimation
       |
       v
Manipulation

Embodied AI

Simulated World
       |
       v
Robot
       |
       v
Sensors
       |
       v
Perception
       |
       v
Reasoning
       |
       v
Action

The monitor provides a direct visualization of the sensory input available to the robot.

39. Potential Extensions

The current architecture can be extended to support additional information.

Possible future modes include:

point clouds

semantic segmentation

object detection

bounding boxes

depth overlays

occupancy grids

navigation maps

thermal images

radar

event-camera data

robot state

joint state visualization

sensor health

FPS

latency

multiple simultaneous monitors

For example, a future object-detection mode could display:

+--------------------------------+
|        Camera Feedback         |
|                                |
|       +-------------+          |
|       |   Bottle    |          |
|       +-------------+          |
|          confidence: 0.94      |
|                                |
+--------------------------------+

A future semantic mode could display:

+--------------------------------+
|       Semantic Scene           |
|                                |
|   Table       Object           |
|                                |
|       Robot                    |
|                                |
+--------------------------------+

40. Development Philosophy

The project follows three main principles.

1. Keep the feedback close to the simulation

Sensor data should be easy to inspect without leaving the simulated world.

2. Prefer responsive visualization

The latest sensor information is more useful for interactive debugging than guaranteed rendering of every frame.

3. Keep the architecture extensible

The monitor should not be limited to one camera type.

The same concept can eventually support many sensor and perception outputs.

41. Example End-to-End Pipeline

A complete robotics perception experiment can look like:

                    Gazebo World
                         |
                         v
                      Robot
                         |
             +-----------+-----------+
             |           |           |
           RGB         RGB-D       LiDAR
             |           |           |
             |           |           |
             +-----------+-----------+
                         |
                         v
                 GzSensorMonitor
                         |
                         v
                  Virtual Monitor
                         |
                         v
                Human Observation

At the same time, the same sensor data can be consumed by a robotics algorithm:

Sensor
   |
   +----------------------+
   |                      |
   v                      v
GzSensorMonitor      Perception System
                           |
                           v
                     Decision / Planning
                           |
                           v
                         Robot

This means the monitor is a visualization/debugging layer and does not need to replace the normal perception pipeline.

42. Why This Plugin?

GzSensorMonitor is useful when the objective is not only:

"Can my sensor publish data?"

but also:

"Can I immediately see what this sensor sees while looking at the simulated robot and environment?"

That distinction is important for robotics development.

The plugin turns sensor visualization into a part of the simulated environment rather than treating it only as an external debugging task.

43. Summary

GzSensorMonitor provides an integrated way to visualize sensor feedback inside Gazebo Harmonic.

It supports the visualization of:

RGB
Depth
RGB-D
LiDAR

and provides:

Real-time display
Runtime mode switching
Virtual monitor
Gazebo-native visualization
Sensor debugging
Camera testing
Perception development

The core concept is:

                 SIMULATED WORLD
                       |
              +--------+--------+
              |                 |
            Robot             Sensors
                                |
                    +-----------+-----------+
                    |           |           |
                   RGB         RGB-D       LiDAR
                    |           |           |
                    +-----------+-----------+
                                |
                                v
                         GzSensorMonitor
                                |
                                v
                       VIRTUAL MONITOR
                                |
                                v
                      LIVE SENSOR FEEDBACK

The result is a more integrated simulation workflow:

        Robot + World + Sensors + Sensor Feedback
                         |
                         v
                    One Gazebo
                    Environment

44. Repository

GitHub:

https://github.com/Aachal-Sharma/gz_sensor_monitor

45. Authors

Aachal Sharma & Rahul Gupta (Both are Equially contributer)

Developed for:

Gazebo Harmonic

ROS 2 Jazzy

Robotics simulation

Real-time sensor visualization
