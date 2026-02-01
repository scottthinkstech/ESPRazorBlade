# Changelog

All notable changes to the ESPRazorBlade library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-beta] - 2026-02-13

### Added

#### Core Connectivity
- Resilient WiFi connection with automatic reconnection on disconnect
- MQTT broker connection with automatic reconnection and keepalive
- MQTT authentication support (username and password)
- RTOS-based non-blocking operation for both WiFi and MQTT
- Thread-safe MQTT operations with mutex protection

#### Telemetry System
- Built-in telemetry metrics published automatically:
  - WiFi signal strength (RSSI)
  - Device uptime (time alive)
  - Free heap memory
  - Boot reset reason (published once on startup)
- Custom telemetry callback registration system
  - Users can register up to 10 custom telemetry callbacks
  - Each callback has configurable publish interval
  - Automatic execution and publishing to MQTT
- Configurable telemetry intervals via Configuration.h:
  - `WIFI_SIGNAL_INTERVAL_MS`
  - `TIME_ALIVE_INTERVAL_MS`
  - `FREE_HEAP_INTERVAL_MS`

#### Hot Configuration
- Runtime configuration updates via MQTT
- Device subscribes to configuration topics on connect:
  - `<device-id>/config/telemetry/timeouts/wifi_rssi`
  - `<device-id>/config/telemetry/timeouts/time_alive`
  - `<device-id>/config/telemetry/timeouts/heap_memory`
- Immediate metric publish when configuration changes
- Configuration value validation (1000ms - 24 hours)
- Initial configuration values published to MQTT on first connect

#### Public API
- `begin()` - Initialize WiFi and MQTT connections
- `publish()` - Publish messages to MQTT (string, int, float, long)
- `registerTelemetry()` - Register custom telemetry callbacks
- `isWiFiConnected()` - Check WiFi connection status
- `isMQTTConnected()` - Check MQTT connection status
- `getIPAddress()` - Get current WiFi IP address

#### Examples
- `Basic_Usage` - Simple WiFi and MQTT connection with built-in telemetry
- `MQTT_With_Auth` - MQTT connection with username/password authentication
- `Basic_MQTT_NoAuth` - Basic MQTT without authentication

### Known Limitations

- **Security**: No TLS/SSL support yet - MQTT credentials transmitted in plaintext
- **Testing**: Tested on ESP32-C3, ESP32-C6, and classic ESP32 (see Tested Hardware below)
- **Memory**: Requires sufficient heap for WiFi/MQTT operations (typically 30KB+ free)
- **Configuration**: Hot config updates only support telemetry timeout intervals
- **Telemetry**: Maximum 10 custom telemetry callbacks (plus 3 built-in)
- **MQTT**: Uses default QoS 0 for most messages
- **Reconnection**: WiFi reconnection attempts every 5 seconds on disconnect
- **Topics**: Fixed topic pattern `<device-id>/telemetry/<metric>` for built-in metrics

### Notes

This is a beta release intended for testing and feedback. The API may change before 1.0.0 release.

**Tested Hardware:**
- ESP32-C3 (Seeed XIAO ESP32-C3) — compile, upload, WiFi, MQTT, reconnection
- ESP32-C6 (Seeed XIAO ESP32-C6) — compile, upload, WiFi, MQTT, reconnection
- Adafruit ESP32 Feather Huzzah (classic dual-core) — compile, upload, WiFi, MQTT, reconnection
- SparkFun ESP32-C6 Thing Plus — compile only (physical test pending)

**Dependencies:**
- ArduinoMqttClient library

**Security Recommendations:**
- Use MQTT authentication (username/password) in production
- Do not expose MQTT broker to public internet without proper security
- Keep Configuration.h files out of version control (add to .gitignore)
- TLS/SSL support planned for future release
