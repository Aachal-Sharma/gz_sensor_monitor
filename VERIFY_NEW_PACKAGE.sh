#!/usr/bin/env bash
set -euo pipefail

SDF="$HOME/gz_sensor_monitor_ws/install/gz_sensor_monitor/share/gz_sensor_monitor/worlds/sensor_monitor_demo.sdf"

echo "Checking: $SDF"

test -f "$SDF"
! grep -q "screen_robot" "$SDF"
! grep -q "camera_monitor_rig" "$SDF"
grep -q "sensor_camera_object" "$SDF"
grep -q "sensor_monitor_object" "$SDF"
grep -q "monitor_screen_visual" "$SDF"
test "$(grep -c 'libGzSensorMonitorSystem.so' "$SDF")" -eq 1

echo "CLEAN: no old screen_robot, one new monitor system, correct visual."
