#!/usr/bin/env bash
set -euo pipefail
test -f include/gz_sensor_monitor/screen_types.hpp
test -f include/gz_sensor_monitor/sensor_monitor_system.hpp
test -f include/gz_sensor_monitor/sensor_monitor_gui.hpp
test ! -d include/gz_sensor_screen
grep -q 'CMAKE_CURRENT_SOURCE_DIR}/include' CMakeLists.txt
echo "HEADERS OK: all v2.0.3 headers are in the expected include path."
