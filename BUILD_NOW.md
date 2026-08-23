v2.0.3 — FINAL HEADER LAYOUT FIX

The previous package still contained the three original headers under:
  include/gz_sensor_screen/

The C++ sources include:
  gz_sensor_monitor/sensor_monitor_system.hpp
  gz_sensor_monitor/sensor_monitor_gui.hpp
  gz_sensor_monitor/screen_types.hpp

This version moves all three to exactly:
  include/gz_sensor_monitor/

No `include/gz_sensor_screen` directory remains.

Do not build this inside ~/gz_sensor_screen_ws.

Clean new workspace:
  rm -rf ~/gz_sensor_monitor_ws
  mkdir -p ~/gz_sensor_monitor_ws/src

Extract this package to:
  ~/gz_sensor_monitor_ws/src/gz_sensor_monitor/

Verify:
  cd ~/gz_sensor_monitor_ws/src/gz_sensor_monitor
  bash VERIFY_SOURCE.sh
  bash VERIFY_HEADERS.sh

Build:
  cd ~/gz_sensor_monitor_ws
  source /opt/ros/jazzy/setup.bash
  rm -rf build install log
  colcon build --symlink-install --packages-select gz_sensor_monitor

Run:
  source install/setup.bash
  export GZ_SIM_SYSTEM_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  export GZ_GUI_PLUGIN_PATH=$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/lib
  gz sim --force-version 8 -r -v 4     ~/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf
