cmake_minimum_required(VERSION 3.16)

project(gz_sensor_monitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

find_package(gz-sim8 REQUIRED)
find_package(gz-rendering8 REQUIRED)
find_package(gz-common5 REQUIRED)
find_package(gz-gui8 REQUIRED)
find_package(gz-plugin2 REQUIRED)
find_package(gz-transport13 REQUIRED)
find_package(gz-msgs10 REQUIRED)

find_package(OpenCV REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

set(GZ_SENSOR_MONITOR_QRC
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/gz_sensor_monitor.qrc
)

qt5_add_resources(
  GZ_SENSOR_MONITOR_RCC
  ${GZ_SENSOR_MONITOR_QRC}
)

# ============================================================
# Gazebo Sim System plugin
# ============================================================

add_library(
  GzSensorMonitorSystem
  SHARED
  src/sensor_monitor_system.cpp
)

target_compile_features(
  GzSensorMonitorSystem PRIVATE cxx_std_17
)

target_include_directories(
  GzSensorMonitorSystem PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
)

ament_target_dependencies(
  GzSensorMonitorSystem
  rclcpp
  sensor_msgs
  std_msgs
)

target_link_libraries(
  GzSensorMonitorSystem
  gz-sim8::gz-sim8
  gz-common5::gz-common5
  gz-plugin2::gz-plugin2
  gz-transport13::gz-transport13
  gz-msgs10::gz-msgs10
  ${OpenCV_LIBRARIES}
)

# ============================================================
# Gazebo GUI plugin
# ============================================================

add_library(
  GzSensorMonitorGui
  SHARED
  src/sensor_monitor_gui.cpp
  ${GZ_SENSOR_MONITOR_RCC}
)

target_compile_features(
  GzSensorMonitorGui PRIVATE cxx_std_17
)

target_include_directories(
  GzSensorMonitorGui PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
)

ament_target_dependencies(
  GzSensorMonitorGui
  rclcpp
  sensor_msgs
  std_msgs
)

target_link_libraries(
  GzSensorMonitorGui
  gz-gui8::gz-gui8
  gz-rendering8::gz-rendering8
  gz-common5::gz-common5
  gz-plugin2::gz-plugin2
  gz-transport13::gz-transport13
  gz-msgs10::gz-msgs10
  ${OpenCV_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
)

# ============================================================
# Install
# ============================================================

install(
  TARGETS
    GzSensorMonitorSystem
    GzSensorMonitorGui
  LIBRARY DESTINATION lib
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  DIRECTORY worlds/
  DESTINATION share/${PROJECT_NAME}/worlds
)

install(
  DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

install(
  DIRECTORY qml/
  DESTINATION share/${PROJECT_NAME}/qml
)

ament_package()

target_include_directories(GzSensorMonitorSystem PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(GzSensorMonitorGui PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
