# ESPRazorBlade

Lightweight Arduino library for ESP32 devices to broadcast device telemetry and sensor data via MQTT.

## Features

### Core Connectivity
- **Easy WiFi Configuration**: Simple WiFi setup via `Configuration.h`
- **Resilient WiFi**: Automatic reconnect with retry and status checks
- **MQTT Connectivity**: ArduinoMqttClient-based connect/reconnect with keepalive polling
- **MQTT Auth Support**: Optional username/password configuration
- **Thread-Safe Publish**: Mutex-protected publish helpers for string/int/float/long
- **Connection Status APIs**: `isWiFiConnected()`, `isMQTTConnected()`, `getIPAddress()`
- **RTOS-Based**: FreeRTOS tasks keep networking non-blocking

### Built-in Telemetry
- **Default System Metrics**: Automatic publishing of device health data
  - WiFi signal strength (RSSI)
  - Time alive (uptime in HH:MM:SS format)
  - Free heap memory
  - Reset reason (published once on boot)
  - Device status ("online" with retained flag)
- **Custom Telemetry Callbacks**: Register your own interval-based callbacks that publish automatically
- **Configurable Intervals**: Set telemetry publish intervals via `Configuration.h`

### Runtime Configuration
- **Hot Config Updates**: Change telemetry intervals without restarting the device
- **MQTT Config Topics**: Subscribe to `<device-id>/config/telemetry/timeouts/*` for real-time updates
- **Immediate Effect**: Config changes trigger immediate metric re-publish with new interval
- **Config Publishing**: Device publishes its current configuration to MQTT on connection (retained)

### Compatibility
- **ESP32 Compatible**: Works with all ESP32 variants (ESP32-C3, ESP32-C6, ESP32-S3, etc.)

## Installation

### Option 1: Arduino IDE (Recommended)

1. Download or clone this repository
2. Open `ESPRazorBlade.ino` or an example sketch in Arduino IDE
3. Copy `Configuration.h.example` to `Configuration.h` in the same directory
4. Edit `Configuration.h` with your WiFi credentials
5. Install ESP32 board support (see [Arduino IDE Guide](ARDUINO_IDE_GUIDE.md))d
6. Select your board and upload!

**Note**: `Configuration.h` is gitignored - your credentials stay local.

### Option 2: As Arduino Library

1. Copy the `ESPRazorBlade` folder to your Arduino `libraries` directory
2. Restart Arduino IDE
3. File → Examples → ESPRazorBlade → [Choose an example]
4. Copy `Configuration.h.example` to `Configuration.h` in the example folder
5. Add your WiFi credentials

**Available examples:**
- `Basic_MQTT_NoAuth` - Simple MQTT without authentication
- `MQTT_With_Auth` - Secure MQTT with username/password

See [ARDUINO_IDE_GUIDE.md](ARDUINO_IDE_GUIDE.md) for detailed instructions.

## Quick Start

> **👉 See complete, ready-to-use examples in the [`examples/`](examples/) directory:**
> - **[Basic_MQTT_NoAuth](examples/Basic_MQTT_NoAuth/)** - Simple MQTT without authentication
> - **[MQTT_With_Auth](examples/MQTT_With_Auth/)** - Secure MQTT with username/password

### 1. Configure Settings

Copy `Configuration.h.example` to `Configuration.h` and fill in your credentials:

```cpp
// WiFi Configuration
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// MQTT Configuration
#define MQTT_BROKER "your_mqtt_broker_ip"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESPRazorBlade_Client"

// Optional: Uncomment if your MQTT broker requires authentication
// #define MQTT_USERNAME "your_username"
// #define MQTT_PASSWORD "your_password"

// Device Identity & Telemetry Configuration
#define DEVICE_ID "esp32-c3-frosty"              // Unique device identifier for MQTT topics
#define WIFI_SIGNAL_INTERVAL_MS 30000            // Publish WiFi RSSI every 30 seconds
#define TIME_ALIVE_INTERVAL_MS 60000             // Publish time alive every 60 seconds
#define FREE_HEAP_INTERVAL_MS 90000              // Publish free heap memory every 90 seconds
```

**Note**: Telemetry intervals can be changed at runtime via MQTT (see Runtime Configuration below).

### 2. Basic Usage Examples

#### Example 1: Minimal Setup (Built-in Telemetry Only)

The simplest way to use ESPRazorBlade - just call `begin()` and you get automatic system telemetry!

**Configuration.h:**
```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_BROKER "192.168.1.100"
#define DEVICE_ID "my-esp32"
// Leave MQTT_USERNAME and MQTT_PASSWORD commented out for no auth
```

**Sketch:**
```cpp
#include "ESPRazorBlade.h"
#include "Configuration.h"

ESPRazorBlade razorBlade;

void setup() {
    Serial.begin(115200);
    
    // That's it! Built-in telemetry starts automatically
    razorBlade.begin();
    
    Serial.println("Device publishing WiFi RSSI, uptime, and heap memory automatically!");
}

void loop() {
    delay(100);
}
```

**Published topics** (with DEVICE_ID="my-esp32"):
- `my-esp32/telemetry/wifi_rssi` - Signal strength
- `my-esp32/telemetry/time_alive` - Uptime
- `my-esp32/telemetry/free_heap` - Memory
- `my-esp32/telemetry/reset_reason` - Boot reason
- `my-esp32/status` - "online"

#### Example 2: Custom Telemetry + Built-in Metrics

Add your own sensor readings alongside the automatic system metrics.

**Configuration.h:**
```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_BROKER "192.168.1.100"
#define DEVICE_ID "my-esp32"
```

**Sketch:**
```cpp
#include "ESPRazorBlade.h"
#include "Configuration.h"

ESPRazorBlade razorBlade;

String readTemperature() {
    float temp = 22.5; // Replace with actual sensor
    return String(temp, 1);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize (built-in telemetry starts automatically)
    razorBlade.begin();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(100);
    }
    
    Serial.println("Connected!");
    
    // Add custom temperature telemetry (publishes every 30 seconds)
    razorBlade.registerTelemetry("my-esp32/telemetry/temperature", readTemperature, 30000);
    
    // Publish one-time startup message
    razorBlade.publish("my-esp32/info/firmware", "v1.0.0");
}

void loop() {
    // Your code here
    delay(100);
}
```

#### Example 2: MQTT With Authentication

**Configuration.h:**
```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_BROKER "mqtt.example.com"
#define MQTT_USERNAME "esp32_user"
#define MQTT_PASSWORD "secure_password"
#define DEVICE_ID "authenticated-esp32"
```

**Sketch:**
```cpp
#include "ESPRazorBlade.h"
#include "Configuration.h"

ESPRazorBlade razorBlade;

String readTemperature() {
    float temp = 22.5;
    return String(temp, 1);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize with authentication (credentials from Configuration.h)
    razorBlade.begin();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(100);
    }
    
    Serial.println("Authenticated and publishing!");
    
    // Register custom telemetry
    razorBlade.registerTelemetry("authenticated-esp32/telemetry/temperature", readTemperature, 30000);
    
    // Publish device info (retained)
    razorBlade.publish("authenticated-esp32/info/version", "1.0.0", true);
}

void loop() {
    // Manually publish additional metrics as needed
    static unsigned long lastCustomMetric = 0;
    if (millis() - lastCustomMetric > 60000) {
        int rssi = WiFi.RSSI();
        razorBlade.publish("authenticated-esp32/debug/manual_rssi", rssi);
        lastCustomMetric = millis();
    }
    delay(100);
}
```

#### Example 3: Runtime Configuration Changes

Monitor and update telemetry intervals via MQTT (no code changes needed).

**Subscribe to see config changes:**
```bash
# Terminal 1: Watch all config topics
mosquitto_sub -h mqtt.example.com -t "my-esp32/config/#" -v

# Terminal 2: Watch telemetry
mosquitto_sub -h mqtt.example.com -t "my-esp32/telemetry/#" -v
```

**Update intervals on the fly:**
```bash
# Speed up WiFi RSSI to every 5 seconds
mosquitto_pub -h mqtt.example.com -t "my-esp32/config/telemetry/timeouts/wifi_rssi" -m "5000"

# Slow down heap memory to every 5 minutes
mosquitto_pub -h mqtt.example.com -t "my-esp32/config/telemetry/timeouts/heap_memory" -m "300000"
```

Your device will:
1. Receive the config update immediately
2. Publish the metric right away
3. Use the new interval for all future publishes

No restart required!

## Built-in Telemetry

The library automatically publishes system metrics when you call `begin()`. No additional code required!

### Default Metrics

| Metric | Topic | Interval | Description |
|--------|-------|----------|-------------|
| WiFi RSSI | `<device-id>/telemetry/wifi_rssi` | Configurable (default: 30s) | WiFi signal strength in dBm |
| Time Alive | `<device-id>/telemetry/time_alive` | Configurable (default: 60s) | Uptime in HHHhMMmSSs format |
| Free Heap | `<device-id>/telemetry/free_heap` | Configurable (default: 90s) | Available heap memory in bytes |
| Reset Reason | `<device-id>/telemetry/reset_reason` | Once on boot | Why the device restarted (PowerOn, Reboot, Crash, etc.) |
| Status | `<device-id>/status` | Once on MQTT connect | "online" (retained message) |

### MQTT Topic Structure

```
<device-id>/
├── status                                    # Device online/offline status (retained)
├── telemetry/
│   ├── wifi_rssi                            # WiFi signal strength
│   ├── time_alive                           # Device uptime
│   ├── free_heap                            # Available memory
│   ├── reset_reason                         # Boot reason (retained)
│   └── heartbeat                            # Custom telemetry via registerTelemetry()
└── config/
    └── telemetry/
        └── timeouts/
            ├── wifi_rssi                    # WiFi RSSI interval in ms (retained)
            ├── time_alive                   # Time alive interval in ms (retained)
            └── heap_memory                  # Heap memory interval in ms (retained)
```

## Runtime Configuration

Change telemetry intervals on-the-fly without restarting your device!

### How It Works

1. **Device publishes current config** when it connects to MQTT
2. **Publish new interval** to a config topic to update it immediately
3. **Device applies change** and republishes the metric right away
4. **New interval takes effect** for subsequent publishes

### Example: Change WiFi RSSI Interval

```bash
# Change WiFi RSSI telemetry to every 15 seconds
mosquitto_pub -h mqtt.example.com -t "esp32-c3-frosty/config/telemetry/timeouts/wifi_rssi" -m "15000"

# Change time alive telemetry to every 2 minutes
mosquitto_pub -h mqtt.example.com -t "esp32-c3-frosty/config/telemetry/timeouts/time_alive" -m "120000"

# Change heap memory telemetry to every 5 minutes
mosquitto_pub -h mqtt.example.com -t "esp32-c3-frosty/config/telemetry/timeouts/heap_memory" -m "300000"
```

**Valid ranges**: 1000ms (1 second) to 86400000ms (24 hours)

### Monitoring Config Changes

Subscribe to see current configurations:

```bash
mosquitto_sub -h mqtt.example.com -t "esp32-c3-frosty/config/#" -v
```

## Development Script (`deploy.sh`)
￼
￼
￼￼
### Basic Usage (Compile + Upload)

```bash
./scripts/deploy.sh
```

This compiles the `examples/Basic_Usage` sketch, then uploads it to a connected board.
You can also run `./scripts/deploy.sh --dev` for the same compile+upload behavior.

### Compile Only (No Upload)

```bash
./scripts/deploy.sh --compile-only
```

### Select a Different Board Target

```bash
./scripts/deploy.sh --fqbn esp32:esp32:esp32s3
```

### Specify Upload Port

If multiple boards are connected, specify the target serial device:

```bash
./scripts/deploy.sh --port /dev/ttyACM0
```

### Show Script Help

```bash
./scripts/deploy.sh --help
```

### Troubleshooting

- If upload fails with serial port permission errors (for example, `/dev/ttyACM* is not readable`), add your user to the serial-access group (`uucp`/`dialout`, distro dependent), then reconnect the board.
- If build directory permissions are wrong, the script falls back to `/tmp/esprazorblade-basic-usage-build`.

## API Reference

### `begin()`
Initialize the library. Starts WiFi and MQTT connection tasks automatically and registers built-in telemetry metrics.

**Built-in metrics automatically registered:**
- WiFi RSSI signal strength
- Device uptime (time alive)
- Free heap memory
- Reset reason (published once on boot)
- Device online status

```cpp
bool begin();
```

**Returns**: `true` if initialization successful

**Example:**
```cpp
ESPRazorBlade razorBlade;

void setup() {
    Serial.begin(115200);
    razorBlade.begin();
    
    // Wait for connections
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(100);
    }
    
    Serial.println("Connected! Built-in telemetry is now active.");
}
```

### `publish()`
Publish a message to an MQTT topic.
```cpp
bool publish(const char* topic, const char* payload, bool retained = false);
bool publish(const char* topic, float value, bool retained = false);
bool publish(const char* topic, int value, bool retained = false);
bool publish(const char* topic, long value, bool retained = false);
```

### `registerTelemetry()`
Register a callback function to automatically publish custom telemetry data at intervals.

**Note**: This is for *additional* custom metrics. Built-in system metrics (WiFi RSSI, uptime, heap) are registered automatically.

```cpp
bool registerTelemetry(const char* topic, TelemetryCallback callback, unsigned long intervalMs);
```

**Parameters:**
- `topic`: MQTT topic to publish to (use your device-id prefix for consistency)
- `callback`: Function that returns the telemetry data as a String
- `intervalMs`: Publish interval in milliseconds

**Returns**: `true` if registration successful, `false` if max callbacks reached (limit: 10 total)

**Example:**
```cpp
String readTemperature() {
    float temp = 22.5; // Replace with actual sensor reading
    return String(temp, 1);
}

// Register custom temperature telemetry
razorBlade.registerTelemetry("esp32-c3-frosty/telemetry/temperature", readTemperature, 30000);
```

### Connection Status
```cpp
bool isWiFiConnected();
bool isMQTTConnected();
String getIPAddress();
```

## Architecture

The library uses FreeRTOS tasks for non-blocking operation:

- **WiFi Task**: Manages WiFi connection and automatic reconnection
- **MQTT Task**: Handles MQTT connection, keepalive, and telemetry publishing
- **Main Loop**: Your code runs independently without blocking

## Requirements

- ESP32 board (any variant)
- Arduino IDE 2.x or 1.8.x
- ESP32 Arduino Core (install via Board Manager)
- **ArduinoMqttClient library** (install via Library Manager) - Required for Phase 2+

## Board Setup

1. Install ESP32 board support:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "esp32" → Install

2. Select your board:
   - ESP32-C3: Tools → Board → esp32 → ESP32C3 Dev Module
   - ESP32-C6: Tools → Board → esp32 → ESP32C6 Dev Module
   - ESP32-S3: Tools → Board → esp32 → ESP32S3 Dev Module

## Serial Monitor

Use baud rate **115200** to see connection status and debug messages.

## License

[Add your license here]

## Contributing

[Add contribution guidelines here]
