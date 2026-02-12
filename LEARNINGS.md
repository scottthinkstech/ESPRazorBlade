# Learnings

## 2026-02-12

- WIN: Moved all telemetry timeout configuration values to Configuration.h (WIFI_SIGNAL_INTERVAL_MS, TIME_ALIVE_INTERVAL_MS, FREE_HEAP_INTERVAL_MS)
- The #ifndef pattern in ESPRazorBlade.cpp provides defaults while allowing user customization
- NEXT: Users can now customize telemetry intervals by editing Configuration.h

## 2025-02-11

- Default telemetry metrics (WiFi RSSI, time alive, reset reason) working; publishing to MQTT and Serial prints on publish
- NEXT: Add heap memory telemetry metric
