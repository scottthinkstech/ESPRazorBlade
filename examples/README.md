# ESPRazorBlade Examples

Ready-to-use example sketches for the ESPRazorBlade library.

## Available Examples

### Basic_MQTT_NoAuth
Simple MQTT connection without authentication.

**Shows:**
- WiFi and MQTT connection
- Automatic telemetry publishing
- Manual publishing

**Best for:** Local testing, development

**Configuration:**
```cpp
#define MQTT_BROKER "192.168.1.100"
// Leave MQTT_USERNAME and MQTT_PASSWORD commented
```

---

### MQTT_With_Auth
MQTT connection with username/password authentication.

**Shows:**
- Authenticated MQTT connection
- Multiple telemetry streams
- System metrics publishing
- Retained messages

**Best for:** Production, secure brokers

**Configuration:**
```cpp
#define MQTT_BROKER "mqtt.example.com"
#define MQTT_USERNAME "your_username"
#define MQTT_PASSWORD "your_password"
```

---

## Quick Start

1. **Copy configuration file:**
   ```bash
   cp Configuration.h.example Configuration.h
   ```

2. **Edit Configuration.h** with your WiFi and MQTT settings

3. **Open in Arduino IDE** and upload to your ESP32

4. **Open Serial Monitor** at 115200 baud

---

## Testing with Mosquitto

### Without Authentication
```bash
# Start broker
mosquitto -v

# Subscribe to messages
mosquitto_sub -h localhost -t "home/esp32/#" -v
```

### With Authentication
```bash
# Create user
sudo mosquitto_passwd -c /etc/mosquitto/passwd esp32_user

# Edit /etc/mosquitto/mosquitto.conf:
#   allow_anonymous false
#   password_file /etc/mosquitto/passwd

# Restart and subscribe
sudo systemctl restart mosquitto
mosquitto_sub -h localhost -t "sensors/#" -u esp32_user -P password -v
```

---

## Troubleshooting

**Connection timeout:**
- Check WiFi credentials
- Verify broker is reachable
- Confirm username/password for authenticated broker

**Failed to initialize:**
- Ensure `Configuration.h` exists
- Check for syntax errors
- Install ArduinoMqttClient library

**No messages in broker:**
- Check topic names
- Use wildcard subscriptions: `#`
- Verify connection in Serial Monitor

---

## Next Steps

- Replace simulated sensors with real sensor code
- Customize topics and intervals
- Add your application logic

See [main README](../README.md) for full API reference.
