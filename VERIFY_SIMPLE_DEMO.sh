#!/usr/bin/env bash
set -euo pipefail
SDF="worlds/sensor_monitor_demo.sdf"
grep -q 'gz-sim-sensors-system' "$SDF"
grep -q 'name="camera_object"' "$SDF"
grep -q 'name="obstacle_red"' "$SDF"
grep -q 'name="obstacle_blue"' "$SDF"
grep -q 'name="monitor_object"' "$SDF"
grep -q 'monitor_screen_visual' "$SDF"
grep -q 'libGzSensorMonitorSystem.so' "$SDF"
grep -q 'libGzSensorMonitorGui.so' "$SDF"
grep -q 'sensor_camera/image_raw' "$SDF"
echo "SIMPLE DEMO OK: 1 elevated camera + 2 obstacles + 1 monitor + live-feed plugins."
