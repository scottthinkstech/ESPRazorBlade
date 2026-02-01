#line 1 "/home/thinks/src/ESPRazorBlade/CLAUDE.md"
# ESPRazorBlade

ESP32-C3 Arduino project with WiFi capability.

## Development Environment

- **IDE**: Arduino IDE 2.x
- **Board**: ESP32-C3
- **Framework**: Arduino

## Project Structure

- `ESPRazorBlade.ino` - Main sketch file

## Board Setup

Install ESP32 board support in Arduino IDE:
1. File → Preferences → Additional Board Manager URLs
2. Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Tools → Board → Boards Manager → Search "esp32" → Install

Select board: Tools → Board → esp32 → ESP32C3 Dev Module

## Serial Monitor

Baud rate: 9600 (current), typically 115200 for ESP32
