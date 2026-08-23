#!/usr/bin/env bash
set -euo pipefail
SDF="worlds/sensor_monitor_demo.sdf"
grep -q 'libGzSensorMonitorGui.so' "$SDF"
grep -q 'monitor_screen_visual' "$SDF"
echo "WORLD OK: GUI monitor plugin and monitor visual are present."
