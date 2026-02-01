#line 1 "/home/thinks/src/ESPRazorBlade/FLASH_GUIDE.md"
# Flashing and Testing Guide

**Note**: The build script (`scripts/build.sh`) is a development tool and optional. Users can use Arduino IDE directly.

## Quick Start

### 1. Flash and Monitor (One Command)

```bash
# Flash root sketch
scripts/build.sh flash sketch

# Flash Phase1_WiFi example
scripts/build.sh flash single:Phase1_WiFi
```

This will:
1. Compile the code
2. Upload to your ESP32-C3
3. Open serial monitor automatically

### 2. Step-by-Step

```bash
# Compile
scripts/build.sh sketch

# Upload
scripts/build.sh upload sketch

# Monitor (in separate terminal)
scripts/build.sh monitor
```

## Board Selection

The build script defaults to **ESP32-C3** (Xiao). To use a different board:

```bash
# ESP32-C3 (default - Xiao)
BOARD_VARIANT=esp32c3 scripts/build.sh sketch

# ESP32-C6
BOARD_VARIANT=esp32c6 scripts/build.sh sketch

# ESP32-S3
BOARD_VARIANT=esp32s3 scripts/build.sh sketch
```

## Serial Port Configuration

### Auto-Detection

The script will auto-detect serial ports. If multiple devices are connected, specify the port:

```bash
# List available ports
scripts/build.sh ports

# Use specific port
SERIAL_PORT=/dev/ttyUSB0 scripts/build.sh upload sketch
SERIAL_PORT=/dev/ttyUSB0 scripts/build.sh monitor
```

### Serial Baud Rate

Default is 115200. To change:

```bash
SERIAL_BAUD=9600 scripts/build.sh monitor
```

## Commands Reference

### Compilation
- `scripts/build.sh sketch` - Compile root sketch
- `scripts/build.sh single Phase1_WiFi` - Compile example
- `scripts/build.sh library` - Compile library only

### Uploading
- `scripts/build.sh upload sketch` - Upload root sketch
- `scripts/build.sh upload single:Phase1_WiFi` - Upload example

### Monitoring
- `scripts/build.sh monitor` - Open serial monitor
- `scripts/build.sh ports` - List available serial ports

### All-in-One
- `scripts/build.sh flash sketch` - Compile + Upload + Monitor
- `scripts/build.sh flash single:Phase1_WiFi` - Compile + Upload + Monitor

## Testing Workflow

### Phase 1 Testing (WiFi Only)

1. **Ensure Configuration.h exists** with your WiFi credentials:
   ```cpp
   #define WIFI_SSID "your_ssid"
   #define WIFI_PASSWORD "your_password"
   ```

2. **Flash the firmware**:
   ```bash
   scripts/build.sh flash sketch
   ```

3. **Observe serial output**:
   - Should see WiFi connection attempts
   - IP address when connected
   - Status updates every 5 seconds

4. **Test reconnection**:
   - Disconnect WiFi router
   - Watch for reconnection attempts
   - Reconnect router
   - Verify automatic reconnection

## Troubleshooting

### Port Not Found

```bash
# Check available ports
scripts/build.sh ports

# Or manually check
ls -la /dev/ttyUSB* /dev/ttyACM*

# Set port explicitly
SERIAL_PORT=/dev/ttyUSB0 scripts/build.sh upload sketch
```

### Upload Permission Denied

```bash
# Add user to dialout group (Linux)
sudo usermod -a -G dialout $USER
# Log out and back in, or:
newgrp dialout

# Or use sudo (not recommended)
sudo scripts/build.sh upload sketch
```

### Board Not Found

```bash
# Check board support
scripts/build.sh check

# Reinitialize Arduino CLI
scripts/build.sh init
```

### Compilation Errors

- Ensure `Configuration.h` exists (not just `.example`)
- Check WiFi credentials are correct
- Verify board variant matches your hardware

## Branch Strategy

We're on `phase1-test` branch for testing. This keeps:
- WiFi credentials out of main branches
- Test code separate from production
- Easy rollback if needed

### After Testing

```bash
# Commit test results
git add .
git commit -m "Phase 1: WiFi testing complete"

# Return to develop
git checkout develop

# Merge when ready
git merge phase1-test
```

## Serial Monitor Tips

- Press `Ctrl+C` to exit monitor
- Monitor opens at 115200 baud by default
- Device resets automatically after upload
- Wait 2-3 seconds after upload before monitoring

## Expected Phase 1 Output

```
=== ESPRazorBlade Library - Phase 1: WiFi Only ===
WiFi task started
Connecting to WiFi: your_ssid
........
WiFi connected!
IP address: 192.168.1.xxx

--- Status Check ---
WiFi: Connected
IP Address: 192.168.1.xxx
RSSI: -45 dBm
-------------------
```
