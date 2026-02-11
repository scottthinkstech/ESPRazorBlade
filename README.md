# ESPRazorBlade

Lightweight Arduino library for ESP32 devices to broadcast device telemetry and sensor data via MQTT.

## Features

- **Easy WiFi Configuration**: Simple WiFi setup via `Configuration.h`
- **Resilient WiFi**: Automatic reconnect with retry and status checks
- **MQTT Connectivity**: ArduinoMqttClient-based connect/reconnect with keepalive polling
- **MQTT Auth Support**: Optional username/password configuration
- **Thread-Safe Publish**: Mutex-protected publish helpers for string/int/float/long
- **Telemetry Callbacks**: Register interval-based callbacks that publish automatically
- **Connection Status APIs**: `isWiFiConnected()`, `isMQTTConnected()`, `getIPAddress()`
- **RTOS-Based**: FreeRTOS tasks keep networking non-blocking
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
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

#define MQTT_BROKER "your_mqtt_broker_ip"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESPRazorBlade_Client"

// Optional: Uncomment if your MQTT broker requires authentication
// #define MQTT_USERNAME "your_username"
// #define MQTT_PASSWORD "your_password"
```

### 2. Basic Usage Examples

#### Example 1: MQTT Without Authentication

**Configuration.h:**
```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_BROKER "192.168.1.100"
// Leave MQTT_USERNAME and MQTT_PASSWORD commented out
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
    
    // Initialize and connect
    razorBlade.begin();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(100);
    }
    
    Serial.println("Connected!");
    
    // Register telemetry (publishes every 30 seconds)
    razorBlade.registerTelemetry("home/esp32/temperature", readTemperature, 30000);
    
    // Publish startup message
    razorBlade.publish("home/esp32/status", "online");
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
    
    // Initialize with authentication
    razorBlade.begin();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(100);
    }
    
    Serial.println("Authenticated!");
    
    // Register telemetry
    razorBlade.registerTelemetry("sensors/temperature", readTemperature, 30000);
    
    // Publish device info (retained)
    razorBlade.publish("device/status", "online", true);
}

void loop() {
    // Publish system metrics every minute
    static unsigned long lastMetrics = 0;
    if (millis() - lastMetrics > 60000) {
        razorBlade.publish("device/uptime", millis() / 1000);
        lastMetrics = millis();
    }
    delay(100);
}
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
Initialize the library. Starts WiFi and MQTT connection tasks automatically.
```cpp
bool begin();
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
Register a callback function to automatically publish telemetry data at intervals.
```cpp
bool registerTelemetry(const char* topic, TelemetryCallback callback, unsigned long intervalMs);
```

**Parameters:**
- `topic`: MQTT topic to publish to
- `callback`: Function that returns the telemetry data as a String
- `intervalMs`: Publish interval in milliseconds

**Example:**
```cpp
String readSensor() {
    return String(analogRead(A0));
}

razorBlade.registerTelemetry("sensor/value", readSensor, 5000); // Every 5 seconds
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
