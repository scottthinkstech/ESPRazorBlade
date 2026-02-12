# Learnings

## 2026-02-12

- WIN: Phase 9 completed - Device publishes telemetry timeout configuration values to MQTT on connect
  - Topics: `<device-id>/config/telemetry/timeouts/wifi_rssi`, `/time_alive`, `/heap_memory`
  - Values published as retained messages for Phase 11 hot reload
  - Followed existing `publishBootTelemetry()` pattern with one-time flag
  - Updated topic structure to be more descriptive: `/config/telemetry/timeouts/` instead of `/config/timeout/`
- WIN: Moved all telemetry timeout configuration values to Configuration.h (WIFI_SIGNAL_INTERVAL_MS, TIME_ALIVE_INTERVAL_MS, FREE_HEAP_INTERVAL_MS)
- The #ifndef pattern in ESPRazorBlade.cpp provides defaults while allowing user customization
- NEXT: Phase 11 - Subscribe to config topics and hot-reload timeout values when external MQTT messages arrive

## 2025-02-11

- Default telemetry metrics (WiFi RSSI, time alive, reset reason) working; publishing to MQTT and Serial prints on publish
- NEXT: Add heap memory telemetry metric
