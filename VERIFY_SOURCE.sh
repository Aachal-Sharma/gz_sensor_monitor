#!/usr/bin/env bash
set -euo pipefail

test -f qml/gz_sensor_monitor.qrc
test -f qml/libGzSensorMonitorGui.so.qml
test ! -e qml/gz_sensor_screen.qrc
test ! -e qml/libGzSensorScreenGui.so.qml

grep -q 'qml/gz_sensor_monitor.qrc' CMakeLists.txt
echo "SOURCE OK: QRC and QML filenames match CMake and plugin namespace."
