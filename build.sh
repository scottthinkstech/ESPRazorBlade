#!/bin/bash

# ESPRazorBlade Build Script
# Compiles library and examples using Arduino CLI

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
# Board selection: XIAO_ESP32C3 (Xiao ESP32-C3), esp32c6, esp32s3, etc.
BOARD_VARIANT="${BOARD_VARIANT:-XIAO_ESP32C3}"
BOARD_FQBN="esp32:esp32:${BOARD_VARIANT}"
LIBRARY_NAME="ESPRazorBlade"
BUILD_DIR="build"
ARDUINO_CLI="arduino-cli"
SERIAL_PORT="${SERIAL_PORT:-}"  # Auto-detect if empty
SERIAL_BAUD="${SERIAL_BAUD:-115200}"

# Print colored messages
info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if Arduino CLI is installed
check_arduino_cli() {
    if ! command -v $ARDUINO_CLI &> /dev/null; then
        error "Arduino CLI not found. Please install it first."
        error "Visit: https://arduino.github.io/arduino-cli/"
        exit 1
    fi
    info "Arduino CLI found: $(which $ARDUINO_CLI)"
}

# Initialize Arduino CLI if needed
init_arduino_cli() {
    info "Initializing Arduino CLI..."
    $ARDUINO_CLI config init --overwrite 2>/dev/null || true
    
    # Add ESP32 board support URL
    info "Adding ESP32 board support..."
    $ARDUINO_CLI config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
    
    # Update board index
    info "Updating board index..."
    $ARDUINO_CLI core update-index
    
    # Install ESP32 core
    info "Installing ESP32 core..."
    $ARDUINO_CLI core install esp32:esp32 || warning "ESP32 core may already be installed"
    
    success "Arduino CLI initialized"
}

# Check if ESP32 board is installed
check_board() {
    local board_name=$(echo $BOARD_VARIANT | tr '[:lower:]' '[:upper:]')
    info "Checking for ESP32-${board_name} board support..."
    if $ARDUINO_CLI board listall | grep -qi "$BOARD_VARIANT"; then
        success "ESP32-${board_name} board support found"
    else
        warning "ESP32-${board_name} board support not found, installing..."
        init_arduino_cli
    fi
}

# Detect serial port
detect_serial_port() {
    if [ -n "$SERIAL_PORT" ]; then
        echo "$SERIAL_PORT"
        return 0
    fi
    
    # Try to detect ESP32 serial port
    local ports=$(ls /dev/ttyUSB* /dev/ttyACM* /dev/tty.usbserial* 2>/dev/null | head -1)
    if [ -n "$ports" ]; then
        echo "$ports"
        return 0
    fi
    
    return 1
}

# Clean build directory
clean() {
    info "Cleaning build directory..."
    rm -rf $BUILD_DIR
    success "Build directory cleaned"
}

# Compile library
compile_library() {
    info "Compiling library: $LIBRARY_NAME"
    
    # Create temporary sketch directory for library compilation
    TEMP_SKETCH=$(mktemp -d)
    TEMP_SKETCH_NAME=$(basename "$TEMP_SKETCH")
    TEMP_SKETCH_FILE="$TEMP_SKETCH/$TEMP_SKETCH_NAME.ino"
    
    # Create minimal sketch that includes the library
    cat > "$TEMP_SKETCH_FILE" << 'EOF'
#include "ESPRazorBlade.h"

void setup() {
    ESPRazorBlade razorBlade;
    razorBlade.begin();
}

void loop() {
}
EOF
    
    # Copy library files to temp sketch
    cp ESPRazorBlade.h "$TEMP_SKETCH/"
    cp ESPRazorBlade.cpp "$TEMP_SKETCH/"
    
    # Copy Configuration.h if it exists
    if [ -f "Configuration.h" ]; then
        cp Configuration.h "$TEMP_SKETCH/"
    else
        warning "Configuration.h not found, using example"
        cp Configuration.h.example "$TEMP_SKETCH/Configuration.h"
    fi
    
    # Compile
    info "Compiling with board: $BOARD_FQBN"
    if $ARDUINO_CLI compile --fqbn $BOARD_FQBN --library "." "$TEMP_SKETCH" --build-path "$BUILD_DIR/library"; then
        success "Library compiled successfully"
        rm -rf "$TEMP_SKETCH"
        return 0
    else
        error "Library compilation failed"
        rm -rf "$TEMP_SKETCH"
        return 1
    fi
}

# Compile root sketch (ESPRazorBlade.ino)
compile_sketch() {
    info "Compiling root sketch: ESPRazorBlade.ino"
    
    if [ ! -f "ESPRazorBlade.ino" ]; then
        error "Sketch file not found: ESPRazorBlade.ino"
        exit 1
    fi
    
    # Check for Configuration.h in sketch directory (Arduino IDE style)
    # This allows users to have Configuration.h in the sketch folder
    if [ -f "Configuration.h" ]; then
        info "Using Configuration.h from sketch directory"
    elif [ -f "Configuration.h.example" ]; then
        warning "Configuration.h not found. Using Configuration.h.example"
        warning "For Arduino IDE: Copy Configuration.h.example to Configuration.h and add your credentials"
        cp Configuration.h.example Configuration.h
    else
        error "No Configuration.h or Configuration.h.example found"
        error "Please create Configuration.h with your WiFi credentials"
        exit 1
    fi
    
    # Compile from current directory
    info "Compiling with board: $BOARD_FQBN"
    if $ARDUINO_CLI compile --fqbn $BOARD_FQBN --library "." . --build-path "$BUILD_DIR/sketch"; then
        success "Sketch compiled successfully"
        info "Binary location: $BUILD_DIR/sketch/ESPRazorBlade.ino.bin"
        return 0
    else
        error "Sketch compilation failed"
        return 1
    fi
}

# Compile example sketch
compile_example() {
    local example_name=$1
    
    if [ -z "$example_name" ]; then
        error "Example name required"
        echo "Usage: $0 single <example_name>"
        echo "Available examples:"
        list_examples
        exit 1
    fi
    
    local example_path="examples/$example_name"
    
    if [ ! -d "$example_path" ]; then
        error "Example not found: $example_path"
        list_examples
        exit 1
    fi
    
    info "Compiling example: $example_name"
    info "Path: $example_path"
    
    # Check for required files
    if [ ! -f "$example_path/$example_name.ino" ]; then
        error "Sketch file not found: $example_path/$example_name.ino"
        exit 1
    fi
    
    # Check for Configuration.h in example directory first (Arduino IDE style)
    # Then check project root, then use example file
    if [ -f "$example_path/Configuration.h" ]; then
        info "Using Configuration.h from example directory"
    elif [ -f "Configuration.h" ]; then
        cp Configuration.h "$example_path/"
        info "Using Configuration.h from project root"
    elif [ -f "$example_path/Configuration.h.example" ]; then
        warning "Configuration.h not found in example directory"
        warning "For Arduino IDE: Copy Configuration.h.example to Configuration.h and add your credentials"
        cp "$example_path/Configuration.h.example" "$example_path/Configuration.h"
    elif [ -f "Configuration.h.example" ]; then
        warning "Configuration.h not found. Using Configuration.h.example"
        cp Configuration.h.example "$example_path/Configuration.h"
    else
        error "No Configuration.h found. Please create one from Configuration.h.example"
        exit 1
    fi
    
    # Compile
    info "Compiling with board: $BOARD_FQBN"
    if $ARDUINO_CLI compile --fqbn $BOARD_FQBN --library "." "$example_path" --build-path "$BUILD_DIR/$example_name"; then
        success "Example '$example_name' compiled successfully"
        info "Binary location: $BUILD_DIR/$example_name/$example_name.ino.bin"
        return 0
    else
        error "Example compilation failed"
        return 1
    fi
}

# List available examples
list_examples() {
    info "Available examples:"
    if [ -d "examples" ]; then
        for example in examples/*/; do
            if [ -d "$example" ]; then
                echo "  - $(basename $example)"
            fi
        done
    else
        warning "No examples directory found"
    fi
}

# Compile all examples
compile_all() {
    info "Compiling all examples..."
    local failed=0
    
    if [ ! -d "examples" ]; then
        error "Examples directory not found"
        return 1
    fi
    
    for example_dir in examples/*/; do
        if [ -d "$example_dir" ]; then
            example_name=$(basename "$example_dir")
            echo ""
            info "=== Compiling $example_name ==="
            if ! compile_example "$example_name"; then
                failed=$((failed + 1))
            fi
        fi
    done
    
    echo ""
    if [ $failed -eq 0 ]; then
        success "All examples compiled successfully"
        return 0
    else
        error "$failed example(s) failed to compile"
        return 1
    fi
}

# Show usage
usage() {
    echo "ESPRazorBlade Build Script"
    echo ""
    echo "Usage: $0 <command> [options]"
    echo ""
    echo "Commands:"
    echo "  sketch               Compile root sketch (ESPRazorBlade.ino)"
    echo "  library              Compile the library only"
    echo "  single <name>        Compile a specific example"
    echo "  all                  Compile all examples"
    echo "  upload <target>      Upload compiled sketch (sketch|single:<name>)"
    echo "  monitor              Open serial monitor"
    echo "  flash <target>       Compile, upload, and monitor (sketch|single:<name>)"
    echo "  list                 List available examples"
    echo "  ports                List available serial ports"
    echo "  clean                Clean build directory"
    echo "  init                 Initialize Arduino CLI (install ESP32 core)"
    echo "  check                Check Arduino CLI and board support"
    echo ""
    echo "Environment Variables:"
    echo "  BOARD_VARIANT        Board variant (esp32c3, esp32c6, etc.) [default: esp32c3]"
    echo "  SERIAL_PORT          Serial port (auto-detected if not set)"
    echo "  SERIAL_BAUD          Serial baud rate [default: 115200]"
    echo ""
    echo "Examples:"
    echo "  $0 sketch"
    echo "  $0 single Phase1_WiFi"
    echo "  $0 upload sketch"
    echo "  $0 flash single:Phase1_WiFi"
    echo "  $0 monitor"
    echo "  BOARD_VARIANT=esp32c6 $0 sketch"
}

# Upload compiled sketch
upload_sketch() {
    local target=$1
    
    if [ -z "$target" ]; then
        error "Upload target required"
        echo "Usage: $0 upload <sketch|single:<name>>"
        exit 1
    fi
    
    local port=$(detect_serial_port)
    if [ -z "$port" ]; then
        error "No serial port found. Please set SERIAL_PORT environment variable"
        echo "Available ports:"
        list_ports
        exit 1
    fi
    
    info "Using serial port: $port"
    
    local build_path=""
    local sketch_name=""
    
    case "$target" in
        sketch)
            if [ ! -d "$BUILD_DIR/sketch" ]; then
                error "Sketch not compiled. Run './build.sh sketch' first"
                exit 1
            fi
            build_path="$BUILD_DIR/sketch"
            sketch_name="ESPRazorBlade.ino"
            ;;
        single:*)
            local example_name=$(echo "$target" | cut -d: -f2)
            if [ -z "$example_name" ]; then
                error "Example name required for single:<name>"
                exit 1
            fi
            build_path="$BUILD_DIR/$example_name"
            sketch_name="$example_name.ino"
            if [ ! -d "$build_path" ]; then
                error "Example not compiled. Run './build.sh single $example_name' first"
                exit 1
            fi
            ;;
        *)
            error "Invalid upload target: $target"
            echo "Use 'sketch' or 'single:<name>'"
            exit 1
            ;;
    esac
    
    info "Uploading $sketch_name to $BOARD_FQBN..."
    if $ARDUINO_CLI upload -p "$port" --fqbn $BOARD_FQBN --input-dir "$build_path"; then
        success "Upload successful!"
        return 0
    else
        error "Upload failed"
        return 1
    fi
}

# Open serial monitor
open_monitor() {
    local port=$(detect_serial_port)
    if [ -z "$port" ]; then
        error "No serial port found. Please set SERIAL_PORT environment variable"
        echo "Available ports:"
        list_ports
        exit 1
    fi
    
    info "Opening serial monitor on $port at ${SERIAL_BAUD} baud"
    info "Press Ctrl+C to exit"
    echo ""
    $ARDUINO_CLI monitor -p "$port" -c baudrate=$SERIAL_BAUD
}

# List available serial ports
list_ports() {
    info "Available serial ports:"
    echo ""
    echo "USB/Serial devices:"
    ls -1 /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | while read port; do
        echo "  $port"
    done || echo "  (none found)"
    echo ""
    echo "macOS devices:"
    ls -1 /dev/tty.usbserial* /dev/tty.usbmodem* 2>/dev/null | while read port; do
        echo "  $port"
    done || echo "  (none found)"
    echo ""
    echo "To use a specific port, set SERIAL_PORT environment variable:"
    echo "  SERIAL_PORT=/dev/ttyUSB0 $0 upload sketch"
}

# Flash: compile, upload, and monitor
flash_sketch() {
    local target=$1
    
    if [ -z "$target" ]; then
        error "Flash target required"
        echo "Usage: $0 flash <sketch|single:<name>>"
        exit 1
    fi
    
    check_board
    
    # Compile first
    case "$target" in
        sketch)
            if ! compile_sketch; then
                error "Compilation failed, aborting flash"
                exit 1
            fi
            ;;
        single:*)
            local example_name=$(echo "$target" | cut -d: -f2)
            if [ -z "$example_name" ]; then
                error "Example name required for single:<name>"
                exit 1
            fi
            if ! compile_example "$example_name"; then
                error "Compilation failed, aborting flash"
                exit 1
            fi
            ;;
        *)
            error "Invalid flash target: $target"
            echo "Use 'sketch' or 'single:<name>'"
            exit 1
            ;;
    esac
    
    echo ""
    info "=== Uploading ==="
    if ! upload_sketch "$target"; then
        exit 1
    fi
    
    echo ""
    info "=== Opening Serial Monitor ==="
    sleep 2  # Give device time to reset
    open_monitor
}

# Main script
main() {
    check_arduino_cli
    
    case "${1:-}" in
        sketch)
            check_board
            compile_sketch
            ;;
        library)
            check_board
            compile_library
            ;;
        single)
            check_board
            compile_example "$2"
            ;;
        all)
            check_board
            compile_all
            ;;
        upload)
            check_board
            upload_sketch "$2"
            ;;
        monitor)
            open_monitor
            ;;
        flash)
            flash_sketch "$2"
            ;;
        ports)
            list_ports
            ;;
        list)
            list_examples
            ;;
        clean)
            clean
            ;;
        init)
            init_arduino_cli
            ;;
        check)
            check_board
            ;;
        *)
            usage
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
