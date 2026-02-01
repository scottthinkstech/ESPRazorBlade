#line 1 "/home/thinks/src/ESPRazorBlade/BUILD.md"
# ESPRazorBlade Build Guide

**Note**: This build script is a development tool located in `scripts/` and is not part of the library distribution. Users should use Arduino IDE for normal development.

## Prerequisites

- Arduino CLI installed (check with `arduino-cli version`)
- ESP32 board support installed (script will install automatically)

## Quick Start

### 1. Restore Stashed Changes

```bash
git stash pop
```

### 2. Initialize Arduino CLI (First Time Only)

```bash
scripts/build.sh init
```

This will:
- Initialize Arduino CLI configuration
- Add ESP32 board support URL
- Install ESP32 core
- Set up ESP32-C6 board support

### 3. Check Setup

```bash
scripts/build.sh check
```

### 4. Compile Phase 1 Example

```bash
scripts/build.sh single Phase1_WiFi
```

## Build Commands

### Compile Library Only

```bash
scripts/build.sh library
```

Compiles the ESPRazorBlade library files to verify they're syntactically correct.

### Compile Specific Example

```bash
scripts/build.sh single Phase1_WiFi
```

Compiles a specific example sketch. The script will:
- Copy `Configuration.h` to the example directory (if it exists)
- Compile the sketch with ESP32-C6 board
- Output binary to `build/<example_name>/`

### Compile All Examples

```bash
scripts/build.sh all
```

Compiles all examples in the `examples/` directory.

### List Available Examples

```bash
scripts/build.sh list
```

### Clean Build Artifacts

```bash
scripts/build.sh clean
```

Removes the `build/` directory and all compiled binaries.

## Configuration

### Board Selection

The build script uses ESP32-C6 by default. To change, edit `build.sh`:

```bash
BOARD_FQBN="esp32:esp32:esp32c6"  # Change to your board
```

Common ESP32 variants:
- `esp32:esp32:esp32c6` - ESP32-C6 (default)
- `esp32:esp32:esp32c3` - ESP32-C3
- `esp32:esp32:esp32s3` - ESP32-S3
- `esp32:esp32:esp32` - Original ESP32

### Configuration File

Before compiling examples, ensure `Configuration.h` exists with your WiFi credentials:

```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
```

If `Configuration.h` doesn't exist, the script will use `Configuration.h.example` (which will fail at runtime - you need real credentials).

## Build Output

Compiled binaries are placed in:
```
build/
├── library/          # Library compilation artifacts
└── Phase1_WiFi/     # Example compilation artifacts
    └── Phase1_WiFi.ino.bin
```

## Troubleshooting

### Arduino CLI Not Found

```bash
# Install Arduino CLI
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Or use package manager
# Arch Linux:
sudo pacman -S arduino-cli
```

### Board Support Not Found

```bash
scripts/build.sh init
```

This will install ESP32 board support.

### Compilation Errors

1. Check that `Configuration.h` exists with valid credentials
2. Verify ESP32 core is installed: `arduino-cli core list`
3. Check board FQBN: `arduino-cli board listall | grep esp32`

### Library Not Found

The build script uses `--library "."` to include the current directory. Make sure you're running the script from the project root.

## Uploading to Board

After compilation, upload using Arduino CLI:

```bash
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32c6 --input-dir build/Phase1_WiFi
```

Or use Arduino IDE:
1. Open the example sketch
2. Select Tools → Board → ESP32C6 Dev Module
3. Select Tools → Port → Your device
4. Click Upload

## IDE Integration

### Arduino IDE

1. Open Arduino IDE
2. File → Preferences → Additional Board Manager URLs
3. Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
4. Tools → Board → Boards Manager → Install "esp32"
5. Open example sketch from `examples/Phase1_WiFi/Phase1_WiFi.ino`

### VS Code with PlatformIO

Create `platformio.ini`:

```ini
[env:esp32-c6-devkitc-1]
platform = espressif32
board = esp32-c6-devkitc-1
framework = arduino
lib_deps = 
    ArduinoMqttClient  ; For Phase 2+
```

## Continuous Integration

Example GitHub Actions workflow:

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Arduino CLI
        run: |
          curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
      - name: Build examples
        run: |
          scripts/build.sh init
          scripts/build.sh all
```
