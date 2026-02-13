# ESPRazorBlade Examples

Ready-to-use example sketches demonstrating different features of the ESPRazorBlade library.

## Quick Start

1. **Copy the configuration template:**
   ```bash
   cd examples/[example_name]
   cp Configuration.h.example Configuration.h
   ```

2. **Edit Configuration.h** with your credentials and settings

3. **Open in Arduino IDE** and upload to your ESP32

4. **Open Serial Monitor** at **115200 baud**

## Available Examples

### 1. Ping_Test

**Hardware verification sketch - start here if you're having issues!**

A minimal test that prints "ping" to Serial Monitor every second. No WiFi, no MQTT, no Configuration.h needed.

**Use this to verify:**
- ESP32 board is functional
- Arduino IDE can compile and upload
- Serial Monitor communication works
- USB cable and drivers are working

**Best for:**
- First-time setup verification
- Troubleshooting hardware or IDE problems
- Testing a new ESP32 board

**Configuration:**
- None required!

**Expected output:**
```
===========================================
   ESPRazorBlade - Hardware Ping Test
===========================================

If you see this message, your ESP32 board
and Serial Monitor are working correctly!

You should see 'ping' messages below:
-------------------------------------------
ping
ping
ping
...
```

---

### 2. Basic_Usage

**The simplest way to use ESPRazorBlade - just WiFi, MQTT, and automatic telemetry!**

This minimal example demonstrates automatic built-in telemetry with zero custom code. Perfect for getting started or testing connectivity.

**Shows:**
- WiFi connection and automatic reconnection
- MQTT connection and automatic reconnection
- Built-in telemetry (WiFi RSSI, uptime, free heap, reset reason)
- Connection status monitoring
- No custom callbacks needed!

**Best for:**
- First-time users learning the library
- Testing WiFi and MQTT connectivity
- Simple device monitoring without custom sensors
- Understanding what the library does automatically

**Configuration:**
```cpp
// Required in Configuration.h
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_password"
#define MQTT_BROKER "192.168.1.100"
#define DEVICE_ID "my-esp32"
// Leave MQTT_USERNAME and MQTT_PASSWORD commented out
```

**Published MQTT topics:**
- `<DEVICE_ID>/status` - "online" (retained)
- `<DEVICE_ID>/telemetry/wifi_rssi` - Signal strength
- `<DEVICE_ID>/telemetry/time_alive` - Uptime
- `<DEVICE_ID>/telemetry/free_heap` - Available memory
- `<DEVICE_ID>/telemetry/reset_reason` - Why device restarted
- `<DEVICE_ID>/config/telemetry/timeouts/*` - Current intervals

**Monitor with mosquitto:**
```bash
# Subscribe to all device topics
mosquitto_sub -h 192.168.1.100 -t "my-esp32/#" -v

# Subscribe to just telemetry
mosquitto_sub -h 192.168.1.100 -t "my-esp32/telemetry/#" -v
```

---

### 3. Basic_MQTT_NoAuth (Custom Telemetry)

**Learn how to add your own sensor callbacks alongside built-in telemetry.**

This example shows how to use `registerTelemetry()` to add custom sensor data (temperature, humidity, pressure) with different publish intervals.

**Shows:**
- Everything from Basic_Usage, PLUS:
- Custom telemetry callbacks (`registerTelemetry()`)
- Multiple sensors with different publish intervals
- Simulated sensor readings (replace with real sensors!)
- One-time publishing with `publish()`
- Retained messages for device info

**Best for:**
- Adding sensor data to your IoT device
- Learning the callback pattern
- Understanding how to extend the library
- Projects with temperature, humidity, or other sensor readings

**Configuration:**
```cpp
// Same as Basic_Usage - no authentication
#define MQTT_BROKER "192.168.1.100"
#define DEVICE_ID "my-esp32"
// Leave MQTT_USERNAME and MQTT_PASSWORD commented out
```

**Published MQTT topics:**
- All built-in topics from Basic_Usage, PLUS:
- `<DEVICE_ID>/telemetry/temperature` - Every 30 seconds
- `<DEVICE_ID>/telemetry/humidity` - Every 45 seconds
- `<DEVICE_ID>/telemetry/pressure` - Every 60 seconds
- `<DEVICE_ID>/info/status` - "online" (retained)
- `<DEVICE_ID>/info/firmware` - Version (retained)

**Replace simulated sensors with real ones:**
```cpp
// Example: DHT22 temperature sensor
String readTemperature() {
    float temp = dht.readTemperature();
    return String(temp, 1);
}
```

**Monitor with mosquitto:**
```bash
mosquitto_sub -h 192.168.1.100 -t "my-esp32/#" -v
```

---

### 4. MQTT_With_Auth

**Production-ready example with MQTT authentication and custom topic organization.**

This example demonstrates secure MQTT connections with username/password authentication, multiple telemetry streams, and both automatic and manual publishing patterns.

**Shows:**
- Everything from Custom Telemetry, PLUS:
- MQTT username/password authentication
- Custom topic namespaces (sensors/*, device/*)
- Manual publishing in loop() alongside automatic telemetry
- Retained messages for device state
- Production-ready authentication pattern

**Best for:**
- Production deployments with secure brokers
- Projects requiring authentication
- Custom topic organization strategies
- Combining automatic telemetry with manual publishing

**Configuration:**
```cpp
// Required: MQTT authentication
#define MQTT_BROKER "mqtt.example.com"
#define MQTT_USERNAME "esp32_user"        // Required!
#define MQTT_PASSWORD "secure_password"   // Required!
#define DEVICE_ID "my-esp32"
```

**Published MQTT topics:**
- Built-in: `<DEVICE_ID>/telemetry/*` and `<DEVICE_ID>/config/*`
- Custom telemetry: `sensors/temperature`, `sensors/humidity`, `sensors/pressure`
- Device info: `device/status`, `device/firmware`, `device/ip` (retained)
- Manual metrics: `device/uptime`, `device/free_heap`, `device/rssi`

**Monitor with mosquitto (authenticated):**
```bash
# Subscribe to all topics
mosquitto_sub -h mqtt.example.com -u esp32_user -P secure_password -t "#" -v

# Subscribe to specific namespaces
mosquitto_sub -h mqtt.example.com -u esp32_user -P secure_password -t "sensors/#" -v
mosquitto_sub -h mqtt.example.com -u esp32_user -P secure_password -t "device/#" -v
```

---

## Prerequisites

### Hardware
- ESP32 board (ESP32-C3, ESP32-C6, ESP32-S3, or classic ESP32)
- USB cable with data support (not power-only)
- Stable 3.3V power supply

### Software
- Arduino IDE 2.x or 1.8.x
- ESP32 board support (via Board Manager)
- ArduinoMqttClient library (via Library Manager)

### Network
- 2.4GHz WiFi network (ESP32 does not support 5GHz)
- MQTT broker (Mosquitto, HiveMQ, etc.)
- Network connectivity between ESP32 and MQTT broker

### MQTT Broker Setup

**Option 1: Local Mosquitto (no auth)**
```bash
# Install mosquitto
sudo apt install mosquitto mosquitto-clients

# Start broker
mosquitto -v
```

**Option 2: Mosquitto with authentication**
```bash
# Create password file and user
sudo mosquitto_passwd -c /etc/mosquitto/passwd esp32_user

# Edit /etc/mosquitto/mosquitto.conf
allow_anonymous false
password_file /etc/mosquitto/passwd

# Restart
sudo systemctl restart mosquitto
```

---

## Choosing the Right Example

```
Start Here
    ↓
[Ping_Test] ────→ Hardware/IDE issues? ───→ Fix, then retry
    ↓ OK
[Basic_Usage] ───→ Need sensors? ──→ Yes ──→ [Custom_Telemetry]
    ↓ No                                            ↓
    Done!                              Need auth? ──→ [MQTT_With_Auth]
                                            ↓ No
                                           Done!
```

**Decision guide:**
- **Just starting?** → Ping_Test, then Basic_Usage
- **Adding sensors?** → Basic_MQTT_NoAuth (Custom Telemetry)
- **Need security?** → MQTT_With_Auth
- **Having problems?** → Ping_Test for hardware verification

---

## Common Troubleshooting

### Configuration Issues

**ERROR: Failed to initialize ESPRazorBlade**
- ✅ Verify `Configuration.h` exists (copy from `.example`)
- ✅ Check all required defines are present
- ✅ Verify syntax (no typos in define names)

**Missing Configuration.h**
```bash
# You need to copy the template first
cp Configuration.h.example Configuration.h
```

### WiFi Connection Issues

**WiFi not connecting**
- ✅ Verify SSID and password are correct in Configuration.h
- ✅ Ensure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- ✅ Check WiFi signal strength (move closer to router)
- ✅ Verify WiFi network is visible and broadcasting

**Frequent WiFi disconnects**
- ✅ Check power supply (insufficient power causes instability)
- ✅ Monitor WiFi RSSI via telemetry (should be > -80 dBm)
- ✅ Move device closer to WiFi access point
- ✅ Try a different power supply or USB cable

### MQTT Connection Issues

**MQTT broker unreachable**
- ✅ Verify broker IP address is correct
- ✅ Ping broker from another device to test connectivity
- ✅ Check firewall allows port 1883
- ✅ Verify broker is running (`mosquitto -v` or check service status)

**MQTT authentication failures**
- ✅ Verify username and password are correct
- ✅ Ensure authentication is enabled on broker (if using MQTT_USERNAME)
- ✅ Check broker logs for authentication errors
- ✅ Test credentials with `mosquitto_sub -u user -P pass`

**MQTT connects then immediately disconnects**
- ✅ Check for duplicate MQTT_CLIENT_ID (each device needs unique ID)
- ✅ Verify broker max connections limit not reached
- ✅ Check broker logs for disconnect reason

### Serial Monitor Issues

**No output or garbled text**
- ✅ Ensure baud rate is **115200** in Serial Monitor
- ✅ Try pressing reset button after opening Serial Monitor
- ✅ Check USB cable supports data (not power-only)
- ✅ Verify correct serial port is selected

### Upload Issues

**Upload fails or times out**
- ✅ Hold BOOT button while clicking upload (some boards)
- ✅ Check USB cable and port
- ✅ Verify correct board selected in Tools → Board
- ✅ Try different USB port or cable

**Permission denied on Linux**
```bash
# Add user to serial group (uucp or dialout depending on distro)
sudo usermod -a -G uucp $USER
# Log out and back in, then reconnect board
```

### Memory Issues

**Device crashes or reboots randomly**
- ✅ Monitor free heap via telemetry (should stay > 30KB)
- ✅ Reduce number of custom telemetry callbacks if heap is low
- ✅ Check for memory leaks in custom callback functions

**Stack overflow errors**
- ✅ Avoid large local arrays in callbacks
- ✅ Use heap allocation sparingly and always free memory
- ✅ Reduce string operations in tight loops

---

## Testing MQTT Topics

### Subscribe to Everything
```bash
# No auth
mosquitto_sub -h 192.168.1.100 -t "#" -v

# With auth
mosquitto_sub -h mqtt.example.com -u user -P pass -t "#" -v
```

### Subscribe to Specific Device
```bash
mosquitto_sub -h 192.168.1.100 -t "my-esp32/#" -v
```

### Publish Test Messages
```bash
# Test manual publish
mosquitto_pub -h 192.168.1.100 -t "test/topic" -m "hello"

# Update config (hot reload)
mosquitto_pub -h 192.168.1.100 -t "my-esp32/config/telemetry/timeouts/wifi_rssi" -m "15000"
```

---

## Next Steps

- Replace simulated sensors with real sensor libraries
- Customize MQTT topics for your project
- Add your application logic in `loop()`
- Explore runtime configuration via MQTT config topics
- Review the main [README](../README.md) for full API reference

---

## Getting Help

- Check the main [README](../README.md) for API documentation
- Review [troubleshooting section](#common-troubleshooting) above
- Start with Ping_Test if you're having hardware issues
- Verify Configuration.h is properly set up for your network

## Example Comparison

| Feature | Ping_Test | Basic_Usage | Custom_Telemetry | MQTT_With_Auth |
|---------|-----------|-------------|------------------|----------------|
| Hardware test | ✅ | ❌ | ❌ | ❌ |
| WiFi connection | ❌ | ✅ | ✅ | ✅ |
| MQTT connection | ❌ | ✅ | ✅ | ✅ |
| Built-in telemetry | ❌ | ✅ | ✅ | ✅ |
| Custom callbacks | ❌ | ❌ | ✅ | ✅ |
| MQTT authentication | ❌ | ❌ | ❌ | ✅ |
| Manual publishing | ❌ | ❌ | ✅ | ✅ |
| Retained messages | ❌ | ❌ | ✅ | ✅ |
| Complexity | 🟢 Minimal | 🟢 Easy | 🟡 Moderate | 🟠 Advanced |
