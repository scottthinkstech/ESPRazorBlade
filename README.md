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
5. Install ESP32 board support (see [Arduino IDE Guide](ARDUINO_IDE_GUIDE.md))
6. Select your board and upload!

**Note**: `Configuration.h` is gitignored - your credentials stay local.

### Option 2: As Arduino Library

1. Copy the `ESPRazorBlade` folder to your Arduino `libraries` directory
2. Restart Arduino IDE
3. File → Examples → ESPRazorBlade → Phase1_WiFi
4. Copy `Configuration.h.example` to `Configuration.h` in the example folder
5. Add your WiFi credentials

See [ARDUINO_IDE_GUIDE.md](ARDUINO_IDE_GUIDE.md) for detailed instructions.

## Quick Start

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

### 2. Basic Usage

```cpp
#include "ESPRazorBlade.h"

ESPRazorBlade razorBlade;

// Telemetry callback function
String readTemperature() {
    float temp = 22.5; // Read from your sensor
    return String(temp, 1);
}

void setup() {
    razorBlade.begin(); // Initialize WiFi and MQTT
    
    // Register telemetry to publish every 30 seconds
    razorBlade.registerTelemetry("sensor/temperature", readTemperature, 30000);
}

void loop() {
    // Your code here - library runs in background!
    
    // Optional: Manual publish
    razorBlade.publish("device/status", "online");
}
```

## MQTT with Authentication

For MQTT brokers that require username and password authentication:

```cpp
#include "ESPRazorBlade.h"

ESPRazorBlade razorBlade;

void setup() {
    Serial.begin(115200);
    
    // Initialize with WiFi and MQTT auth from Configuration.h
    razorBlade.begin();
    
    // Wait for connections
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(100);
    }
    
    Serial.println("Connected with authentication!");
    Serial.print("IP Address: ");
    Serial.println(razorBlade.getIPAddress());
    
    // Publish authenticated message
    razorBlade.publish("device/authenticated", "true");
}

void loop() {
    // Your authenticated code here
}
```

**Configuration.h for authenticated MQTT:**

```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

#define MQTT_BROKER "broker.example.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESPRazorBlade_Client"

// Enable MQTT authentication
#define MQTT_USERNAME "your_mqtt_username"
#define MQTT_PASSWORD "your_mqtt_password"
```

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
