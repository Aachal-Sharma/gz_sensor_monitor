#!/usr/bin/env bash
set -euo pipefail
SDF="worlds/sensor_monitor_demo.sdf"
grep -q 'gz-sim-sensors-system' "$SDF"
grep -q 'libGzSensorMonitorSystem.so' "$SDF"
grep -q 'libGzSensorMonitorGui.so' "$SDF"
grep -q 'monitor_screen_visual' "$SDF"
grep -q '/sensor_camera/image_raw' "$SDF"
echo "DEMO OK: Gazebo Sensors system + monitor system + GUI + camera topic are configured."
