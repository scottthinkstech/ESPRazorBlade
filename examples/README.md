# ESPRazorBlade Examples

This directory contains complete, ready-to-use example sketches demonstrating different features and use cases of the ESPRazorBlade library.

## Available Examples

### 1. Basic_MQTT_NoAuth
**Perfect for:** Local development, testing, public test brokers

Simple MQTT connection without authentication. Demonstrates:
- WiFi connection
- MQTT connection without credentials
- Automatic telemetry publishing
- Manual message publishing
- Connection status monitoring
- Heartbeat messages

**Use when:**
- Testing with a local Mosquitto broker (default config)
- Using public test brokers (test.mosquitto.org, broker.hivemq.com)
- Rapid prototyping in a trusted network

**Configuration needed:**
```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_BROKER "192.168.1.100"  // Your broker IP
// Leave MQTT_USERNAME and MQTT_PASSWORD commented out
```

---

### 2. MQTT_With_Auth
**Perfect for:** Production deployments, secure brokers, cloud services

MQTT connection with username/password authentication. Demonstrates:
- WiFi connection
- Authenticated MQTT connection
- Multiple telemetry streams
- Retained messages
- System metrics publishing
- Device information broadcasting

**Use when:**
- Deploying to production
- Using cloud MQTT services (AWS IoT, Azure IoT Hub, etc.)
- Connecting to secured brokers
- Need for user-level access control

**Configuration needed:**
```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_BROKER "mqtt.example.com"
#define MQTT_USERNAME "esp32_user"      // REQUIRED
#define MQTT_PASSWORD "secure_password"  // REQUIRED
```

---

## How to Use Examples

### Method 1: Directly in Arduino IDE

1. Open the example `.ino` file in Arduino IDE
2. Copy `Configuration.h.example` to `Configuration.h` in the same directory as the example
3. Edit `Configuration.h` with your credentials
4. Select your ESP32 board from Tools → Board
5. Upload to your device

### Method 2: From Library Examples Menu

If ESPRazorBlade is installed as a library:

1. File → Examples → ESPRazorBlade → [Example Name]
2. Follow setup instructions in the sketch comments

---

## Testing Examples

### Test MQTT Without Auth
Using Mosquitto on Linux/Mac:
```bash
# Install mosquitto
sudo apt install mosquitto mosquitto-clients  # Ubuntu/Debian
brew install mosquitto                         # macOS

# Start broker (no auth)
mosquitto -v

# Subscribe to test messages
mosquitto_sub -h localhost -t "home/esp32/#" -v
```

### Test MQTT With Auth
Setup authenticated Mosquitto broker:
```bash
# Create password file
sudo mosquitto_passwd -c /etc/mosquitto/passwd esp32_user

# Edit /etc/mosquitto/mosquitto.conf
# Add:
#   allow_anonymous false
#   password_file /etc/mosquitto/passwd

# Restart mosquitto
sudo systemctl restart mosquitto

# Subscribe with auth
mosquitto_sub -h localhost -t "sensors/#" -u esp32_user -P your_password -v
```

---

## Common Configuration Patterns

### Local Development (No Auth)
```cpp
#define MQTT_BROKER "localhost"  // or "192.168.1.100"
#define MQTT_PORT 1883
// No auth needed
```

### Mosquitto with Auth
```cpp
#define MQTT_BROKER "192.168.1.100"
#define MQTT_USERNAME "esp32_device"
#define MQTT_PASSWORD "device_password"
```

### HiveMQ Cloud
```cpp
#define MQTT_BROKER "your-cluster.hivemq.cloud"
#define MQTT_PORT 8883  // TLS (note: TLS not yet supported in this library)
#define MQTT_USERNAME "your_username"
#define MQTT_PASSWORD "your_password"
```

### AWS IoT Core
*Note: Requires X.509 certificates (not yet supported in this library)*

---

## Serial Monitor Output

Set baud rate to **115200** to see:
- Connection status
- IP address
- MQTT messages being published
- Telemetry callbacks executing
- Heartbeat messages
- Error messages

---

## Troubleshooting

### "Connection timeout" error
- Verify WiFi credentials in `Configuration.h`
- Check that MQTT broker is reachable
- For authenticated broker, verify username/password are correct

### "Authentication failed" error  
- Broker requires auth but credentials not provided
- Username or password incorrect
- Broker not configured to accept the user

### "Failed to initialize" error
- `Configuration.h` file missing
- Syntax error in `Configuration.h`
- Library dependencies not installed (ArduinoMqttClient)

### Messages not appearing in MQTT broker
- Check topic names match what you're subscribing to
- Verify MQTT connection is established (check Serial Monitor)
- Use wildcard subscriptions for testing: `home/esp32/#` or `sensors/#`

---

## Next Steps

After trying these examples:
1. Modify topic names to match your MQTT topic structure
2. Replace simulated sensor readings with real sensor code
3. Add your own telemetry callbacks
4. Customize publish intervals
5. Add actuator control based on MQTT subscriptions (future feature)

---

## Getting Help

- Read the main [README.md](../README.md) for API reference
- Check [ARDUINO_IDE_GUIDE.md](../ARDUINO_IDE_GUIDE.md) for setup help
- Review source code in the `ESPRazorBlade` directory
- Open an issue on GitHub (if applicable)
