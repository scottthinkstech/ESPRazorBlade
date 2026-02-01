#line 1 "/home/thinks/src/ESPRazorBlade/ARDUINO_IDE_GUIDE.md"
# Arduino IDE Usage Guide

## Overview

ESPRazorBlade is designed to work seamlessly with the standard Arduino IDE. You don't need the build script - it's just a convenience tool.

## Setup for Arduino IDE

### 1. Install ESP32 Board Support

1. Open Arduino IDE
2. File → Preferences → Additional Board Manager URLs
3. Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
4. Tools → Board → Boards Manager
5. Search "esp32" → Install "esp32 by Espressif Systems"

### 2. Select Your Board

- **Xiao ESP32-C3**: Tools → Board → esp32 → XIAO_ESP32C3
- **ESP32-C6**: Tools → Board → esp32 → ESP32C6 Dev Module
- **ESP32-S3**: Tools → Board → esp32 → ESP32S3 Dev Module

### 3. Configure WiFi Credentials

#### For Root Sketch (ESPRazorBlade.ino)

1. Copy `Configuration.h.example` to `Configuration.h` in the project root
2. Edit `Configuration.h` and add your WiFi credentials:
   ```cpp
   #define WIFI_SSID "your_wifi_ssid"
   #define WIFI_PASSWORD "your_wifi_password"
   ```

#### For Examples

1. Open the example folder (e.g., `examples/Phase1_WiFi/`)
2. Copy `Configuration.h.example` to `Configuration.h` in that folder
3. Edit `Configuration.h` and add your WiFi credentials

**Important**: `Configuration.h` is gitignored, so your credentials won't be committed.

### 4. Install Library Dependencies

For Phase 1 (WiFi only): No additional libraries needed.

For Phase 2+ (MQTT): Install ArduinoMqttClient via Library Manager.

### 5. Open and Upload

1. File → Open → Select `ESPRazorBlade.ino` or an example sketch
2. Tools → Port → Select your ESP32 device
3. Click Upload (→ button)

### 6. Monitor Serial Output

1. Tools → Serial Monitor
2. Set baud rate to **115200**
3. You should see WiFi connection status

## File Structure for Arduino IDE

```
ESPRazorBlade/
├── ESPRazorBlade.ino          ← Main sketch (open this)
├── Configuration.h             ← Your credentials (create from .example)
├── Configuration.h.example     ← Template
├── ESPRazorBlade.h             ← Library header (auto-included)
├── ESPRazorBlade.cpp           ← Library implementation (auto-included)
└── examples/
    └── Phase1_WiFi/
        ├── Phase1_WiFi.ino     ← Example sketch
        ├── Configuration.h     ← Your credentials (create from .example)
        └── Configuration.h.example ← Template
```

## How It Works

Arduino IDE automatically:
- Includes `.h` and `.cpp` files in the sketch directory
- Compiles them together with your `.ino` file
- Links the library code automatically

The `#include "ESPRazorBlade.h"` in your sketch finds `ESPRazorBlade.h` in the same directory.

## Configuration.h Location

Arduino IDE looks for `Configuration.h` in the **same directory as your `.ino` file**:

- **Root sketch**: `Configuration.h` in project root (same folder as `ESPRazorBlade.ino`)
- **Examples**: `Configuration.h` in example folder (same folder as `Phase1_WiFi.ino`)

This is why each example has its own `Configuration.h.example` - users can copy it locally.

## Troubleshooting

### "Configuration.h: No such file or directory"

- Make sure `Configuration.h` exists in the same folder as your `.ino` file
- Copy `Configuration.h.example` to `Configuration.h` if needed

### "WiFi connection failed"

- Check your WiFi credentials in `Configuration.h`
- Ensure SSID and password are correct
- Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)

### "Board not found"

- Install ESP32 board support (see Setup step 1)
- Select correct board variant in Tools → Board

### Library Not Found

- Make sure `ESPRazorBlade.h` and `ESPRazorBlade.cpp` are in the same directory as your sketch
- Or install as a proper Arduino library (see Library Installation below)

## Library Installation (Optional)

To use ESPRazorBlade as a proper Arduino library:

1. Copy the entire `ESPRazorBlade` folder to your Arduino `libraries` directory:
   - **Linux**: `~/Arduino/libraries/`
   - **macOS**: `~/Documents/Arduino/libraries/`
   - **Windows**: `Documents\Arduino\libraries\`

2. Restart Arduino IDE

3. File → Examples → ESPRazorBlade → Phase1_WiFi

4. Each example will have its own `Configuration.h.example` to copy

## Build Script vs Arduino IDE

The `build.sh` script is **optional** - it's just a convenience tool for:
- Command-line compilation
- Automated testing
- CI/CD pipelines

**Arduino IDE users don't need it at all.** Just:
1. Copy `Configuration.h.example` to `Configuration.h`
2. Add your credentials
3. Open and upload in Arduino IDE

Both methods produce the same result!
