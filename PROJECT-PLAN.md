# Project Developement and Release Plan

## Overview

Launch the ESP32RazorBlade Arduino Library by following an incremental and phased development plan. 

## Feature Breakdown

+ Configurable
    - Configuration variables stored in `Configuration.h` 
        - WiFi info & credentials [completed]
        - MQTT info & credentials [completed]
        - Telemetry broadcast timeout intervals in ms [todo]
            + WiFi Signal
            + Free memory heap size
            + Time alive
        - Telemetry broadcast intervals (timeouts between metric publishes) [todo]
    - Hot configuration updates by publishing to MQTT configuration topics
        + on start, configuration values are loaded from `Configuration.h` [partial]
        + on MQTT connection 
            - device configuration topics are checked [todo]
                + check MQTT topics for configuration value [todo]
                + if a configuration has not been published to the MQTT topic
                    - use the value specified in `Configuration.h` [todo]
                    - publish the value to the appropriate device config topic [todo]
        + on configuration value published to MQTT topic by external publisher
            - device detects incoming configuration value via MQTT topic subscription [todo]
            - device performs a hot update of the configuration variable [todo]
                + without restarting, updates the configuration value to the new value
                (example, wifi signal timeout updated to every 30 seconds instead of 5 minutes because a user published to the device's MQTT topic for wifi signal telemetry timeout)
+ Durable Connectivity
    - Connects to WiFi on start [completed]
        - detects disconnection [completed]
        - reconnects gracefully [completed]
    - Connects to MQTT Server on start [completed]
        - detects disconnection [completed]
        - reconnects gracefully [completed]
+ Flexible MQTT Usage
    - Connect without auth [completed]
    - Connect with auth (username + password) [completed]
    - Publish to telemetry channels [completed]
    - Subscribe to configuration changes [todo]
+ Extensible
    - Users of the library can..
        + Add their own telemetry publishing callback with minimal coding
            - temperature, switch states, sensor readings, etc..
+ User Friendly
    - The library contains a README file that can be understood by the average Arduino user
        + README markdown looks good on Github (images, links, etc..)
    - The library includes example files
        + Basic usage (WiFi + MQTT without credentials)
        + Advanced usage 
            + MQTT with auth (username, password)
            + Sensor telemetry reporting example
                - Si7021 temperature sensor example
    - Examples are written with simple code
    - Using the library requires minimal code writing
+ Stable Development Tools (not shipped with library)
    - Easy to use developer scripts for..
        + deployment (compiling and uploading sketch)
        + testing (execute functionality tests to check for regressions)

## Status (update this plan as development progresses)

+ WiFi connects, keeps alive, detects disconnects, & attempts reconnection if needed
+ MQTT connects, keeps alive, detects disconnects, & attempts reconnection if needed
+ Publish callbacks established so users can register their own callback for publishing updates to MQTT host
+ Basic deployment scripts exist (needs refined, more definition when we come to this phase)
+ Test harness (exists, but is not working, requirements will be defined when we come to this task)
+ Default telemetry metrics working: WiFi RSSI, time alive, free heap, reset reason (broadcast to MQTT when connected)
+ Telemetry timeout configuration values movable to Configuration.h and published to MQTT on connect (Phase 9 complete)
+ Device subscribes to config topics and hot-reloads timeout values in real-time (Phase 11 complete)
+ Config changes trigger immediate metric publish with new interval applied

## Where We Left Off (2026-02-13)

Core library functionality complete (Phases 9-11). Device has WiFi/MQTT connectivity, built-in telemetry, custom telemetry callbacks, and hot configuration updates via MQTT. Now preparing for beta launch with focus on library metadata, documentation, examples, testing, and repository cleanup.

## Phased Development Plan

---

## COMPLETED PHASES

### Phase 9: Publish Default Telemetry Metric Timeouts on Start [COMPLETED]

When the device starts, publish timeout values assigned to default telemetry to the MQTT server.

DELIVERABLES
+ On MQTT connection, publish timeout values to config topics [completed]
  - `<device-id>/config/telemetry/timeouts/wifi_rssi` [completed]
  - `<device-id>/config/telemetry/timeouts/time_alive` [completed]
  - `<device-id>/config/telemetry/timeouts/heap_memory` [completed]

### Phase 11: Hot Load Default Telemetry Metric Timeouts [COMPLETED]

After device has started and connected to the MQTT server, subscribe to telemetry configuration topics. As the device is running, update the appropriate configuration value.

DELIVERABLES
+ On load, load default metric configuration values from Configuration.h [completed]
+ Subscribe to telemetry metric topics [completed]
+ When values are published to the topics, update the corresponding configuration value [completed]
+ Immediately send metric that had configuration value changed [completed]
+ Use the new configuration value for subsequent publishes [completed]

IMPLEMENTATION DETAILS
+ Added MQTT subscription to three config topics on connection
+ Static callback handler receives MQTT messages
+ Config parser validates timeout values (1000ms - 24 hours)
+ Telemetry intervals updated dynamically
+ Immediate metric publish triggered by resetting lastExecution to 0

---

## BETA LAUNCH PREPARATION PHASES

### Phase 12: Library Metadata & Essential Files [NOT STARTED]

Create required Arduino library files and update metadata for public beta release.

ATOMIC TASKS
+ [ ] Create `LICENSE` file
  - Choose appropriate license (MIT, Apache 2.0, GPL, etc.)
  - Add license text to file
+ [ ] Create `keywords.txt` for Arduino IDE syntax highlighting
  - Add class name: `ESPRazorBlade`
  - Add public methods: `begin`, `publish`, `registerTelemetry`, `isWiFiConnected`, `isMQTTConnected`, `getIPAddress`
  - Add callback type: `TelemetryCallback`
  - Mark keywords with appropriate type (KEYWORD1, KEYWORD2, LITERAL1)
+ [ ] Update `library.properties`
  - Change version from `1.0.0-phase5` to `0.1.0-beta` or `0.9.0-beta`
  - Replace placeholder `author` with actual name
  - Replace placeholder `maintainer` with actual name and email
  - Replace placeholder `url` with actual GitHub repository URL
  - Update `sentence` to remove "Phase 5" reference
  - Update `paragraph` to remove "Phase 5" reference
+ [ ] Create `CHANGELOG.md`
  - Add `## [0.1.0-beta] - 2026-02-XX` section
  - Document all features in this release
  - List known limitations

DELIVERABLES
+ All required Arduino library files present
+ Metadata reflects beta status
+ License clarifies usage terms

### Phase 13: Documentation Cleanup [NOT STARTED]

Fix inconsistencies, errors, and gaps in user-facing documentation.

ATOMIC TASKS
+ [ ] Fix `README.md` typo on line 43
  - Remove trailing 'd' from `ARDUINO_IDE_GUIDE.md))d`
+ [ ] Update "Available examples" section in README
  - Currently lists `Basic_MQTT_NoAuth` and `MQTT_With_Auth`
  - Verify actual example names and sync list
+ [ ] Add "Known Limitations" or "Beta Notes" section to README
  - Document any edge cases
  - List tested ESP32 variants
  - Note future features (TLS/SSL, etc.)
+ [ ] Add "Troubleshooting" section to README
  - WiFi connection issues
  - MQTT authentication failures
  - Serial port permissions
  - Memory constraints
+ [ ] Add "Hardware Requirements" section to README
  - Minimum RAM requirements
  - Flash memory usage
  - Tested ESP32 variants (ESP32-C3, ESP32-C6, ESP32-S3, etc.)
+ [ ] Review all Configuration.h interval defines
  - Ensure README examples match actual required defines
  - Verify `WIFI_SIGNAL_INTERVAL_MS`, `TIME_ALIVE_INTERVAL_MS`, `FREE_HEAP_INTERVAL_MS` are documented
+ [ ] Add security warning to README
  - Note about MQTT credentials security
  - Recommend using authentication in production
  - Mention TLS/SSL as future enhancement
+ [ ] Review inline code comments for Doxygen compatibility
  - Add @param, @return, @brief tags where missing
  - Ensure public API is well-documented

DELIVERABLES
+ README is accurate, complete, and beginner-friendly
+ No broken links or typos
+ Security best practices documented

### Phase 14: Example Consolidation & Polish [NOT STARTED]

Streamline examples to 2-3 polished, production-ready sketches.

ATOMIC TASKS
+ [ ] Audit existing examples
  - `Basic_Usage` - has `Configuration.h` (not `.example`)
  - `MQTT_With_Auth` - has `Configuration.h.example` ✓
  - `Basic_MQTT_NoAuth` - has `Configuration.h.example` ✓
  - `Ping_Test` - investigate purpose and quality
+ [ ] Standardize Configuration.h approach
  - All examples should use `Configuration.h.example`
  - Add to `.gitignore` to prevent credential commits
+ [ ] Create/refine `Basic_Usage` example
  - Simple WiFi + MQTT connection
  - Built-in telemetry only (no custom callbacks)
  - Clear comments explaining each step
  - Must compile without modification after config
+ [ ] Create/refine `Custom_Telemetry` example
  - Shows `registerTelemetry()` usage
  - Simulated sensor readings (temperature, humidity, pressure)
  - Demonstrates different publish intervals
  - Shows both retained and non-retained messages
+ [ ] Create/refine `Runtime_Config` example
  - Demonstrates hot config updates via MQTT
  - Includes mosquitto_pub commands in comments
  - Shows before/after telemetry interval changes
  - Documents valid timeout ranges
+ [ ] Remove or archive low-quality examples
  - Review `Ping_Test` - keep only if valuable
  - Remove `Basic_MQTT_NoAuth` if redundant with `Basic_Usage`
+ [ ] Create `examples/README.md`
  - Brief description of each example
  - When to use which example
  - Prerequisites for each
+ [ ] Verify all examples compile
  - Test on ESP32-C3 (primary target)
  - Verify `Configuration.h.example` has all required defines

DELIVERABLES
+ 2-3 polished, well-commented examples
+ All examples use `.example` pattern for config files
+ Examples demonstrate different use cases clearly

### Phase 15: Testing & Validation [NOT STARTED]

Comprehensive testing across hardware, edge cases, and stress scenarios.

ATOMIC TASKS
+ [ ] Hardware compatibility testing
  - Test on ESP32-C3 (primary target)
  - Test on ESP32-C6
  - Test on ESP32-S3
  - Test on classic ESP32 (if available)
  - Document which boards were tested
+ [ ] WiFi resilience testing
  - Test WiFi disconnect/reconnect (disable router WiFi)
  - Test weak signal conditions (RSSI < -80 dBm)
  - Test WiFi password change (force auth failure)
  - Verify reconnection logic works correctly
+ [ ] MQTT resilience testing
  - Test MQTT broker restart (docker stop/start)
  - Test MQTT broker unreachable (wrong IP)
  - Test authentication failure (wrong username/password)
  - Test keepalive timeout (network pause)
  - Verify reconnection and republish works
+ [ ] Hot config update edge cases
  - Test invalid timeout values (negative, zero, > 24 hours)
  - Test extremely short timeout (1 second)
  - Test extremely long timeout (24 hours)
  - Test rapid config changes (multiple updates in quick succession)
  - Verify validation prevents invalid values
+ [ ] Memory profiling
  - Measure flash usage after compilation
  - Measure RAM usage (free heap at startup and during operation)
  - Monitor for memory leaks (run for 24+ hours)
  - Document baseline memory requirements
+ [ ] Custom telemetry callback testing
  - Test maximum callbacks (10 total)
  - Test callback that returns empty string
  - Test callback that returns very long string
  - Test callback with different intervals
+ [ ] Configuration validation
  - Test missing defines in Configuration.h
  - Test malformed MQTT broker address
  - Test invalid port numbers
  - Ensure graceful failure messages
+ [ ] Long-duration stability test
  - Run for 24+ hours continuously
  - Monitor for crashes, memory leaks, or reconnection failures
  - Log any unexpected behavior

DELIVERABLES
+ Documented test results for each ESP32 variant tested
+ Known issues and limitations documented
+ Memory usage baseline documented
+ Confidence in WiFi/MQTT resilience

### Phase 16: Repository Cleanup [NOT STARTED]

Remove internal development files and organize repository for public release.

ATOMIC TASKS
+ [ ] Review and remove internal planning documents
  - Remove or archive `PROJECT-PLAN.md` (this file - save elsewhere if needed)
  - Remove `LEARNINGS.md` (development log)
  - Remove `RESUME_PLAN.md` (internal workflow)
  - Remove `MIGRATION_PLAN.md` (internal only)
  - Remove `PHASE1_COMPLETE.md` (milestone marker)
+ [ ] Review and remove AI-specific files
  - Remove `AGENTS.md` (AI agent instructions)
  - Remove `CLAUDE.md` (AI context)
  - Add `.cursor/` to `.gitignore` (IDE-specific)
+ [ ] Review and decide on development guides
  - Keep `BUILD.md` if helpful for contributors
  - Keep `ARDUINO_IDE_GUIDE.md` (helpful for users)
  - Keep `FLASH_GUIDE.md` (helpful for users)
  - Merge or consolidate if redundant
+ [ ] Update `.gitignore`
  - Ensure `Configuration.h` is ignored (already done)
  - Add `.cursor/` directory
  - Add common Arduino IDE temp files (.vscode, *.bak, etc.)
  - Add build artifacts (.arduino-build, /tmp/esprazorblade-*, etc.)
+ [ ] Review root directory files
  - Keep only: `README.md`, `LICENSE`, `CHANGELOG.md`, `keywords.txt`
  - Keep: `library.properties`, `.gitignore`
  - Keep: User-facing guides (`ARDUINO_IDE_GUIDE.md`, etc.)
  - Consider: Move `scripts/` to `.github/scripts/` or document as dev-only
+ [ ] Clean up example directories
  - Remove any test/debug sketches
  - Ensure only polished examples remain

DELIVERABLES
+ Repository contains only user-facing and contributor-facing files
+ No internal development artifacts exposed
+ Clean, professional appearance

### Phase 17: Final Polish & Release Prep [NOT STARTED]

Final review, version tagging, and release artifact preparation.

ATOMIC TASKS
+ [ ] Final README review
  - Read through as if you're a new user
  - Verify all links work
  - Check for consistency in terminology
  - Verify code examples match actual API
+ [ ] Verify all Configuration.h.example files are complete
  - All required defines present
  - Sensible default values
  - Clear comments explaining each setting
+ [ ] Create release notes for beta 1
  - Summarize features
  - List known limitations
  - Provide upgrade path (N/A for first release)
  - Include hardware compatibility matrix
+ [ ] Git tag preparation
  - Ensure all changes committed
  - Clean working directory
  - Create annotated tag `v0.1.0-beta` or `v0.9.0-beta`
+ [ ] GitHub repository preparation (if applicable)
  - Write good repository description
  - Add topics/tags (arduino, esp32, mqtt, iot, telemetry)
  - Configure GitHub Issues (if accepting bug reports)
  - Add CONTRIBUTING.md if accepting contributions
+ [ ] Create release ZIP
  - Verify ZIP contents match Arduino Library Specification
  - Test ZIP extraction and installation
  - Verify examples appear in Arduino IDE after installation
+ [ ] Final compilation check
  - Compile all examples one more time
  - Verify no warnings or errors
  - Check binary sizes are reasonable

DELIVERABLES
+ Beta release package ready for distribution
+ Git repository tagged and clean
+ Release notes published
+ Library installable via ZIP in Arduino IDE

### Phase 18: Post-Beta Tasks [NOT STARTED]

Future improvements after beta launch (not blocking release).

ATOMIC TASKS
+ [ ] Add TLS/SSL support for secure MQTT
+ [ ] Create GitHub Actions CI/CD pipeline
  - Automated compilation tests
  - Multi-board compatibility testing
  - Automated example validation
+ [ ] Add Arduino Library Manager submission
  - Meet all Arduino Library Manager requirements
  - Submit library for indexing
+ [ ] Create additional examples
  - Real sensor integration (DHT22, BMP280, etc.)
  - Deep sleep / power management example
  - Multiple device coordination example
+ [ ] Performance optimizations
  - Reduce memory footprint if needed
  - Optimize MQTT reconnection logic
  - Add compile-time feature flags to disable unused features
+ [ ] Enhanced configuration
  - Support for custom telemetry topic patterns
  - Configurable QoS levels
  - Last Will and Testament (LWT) configuration

DELIVERABLES
+ Roadmap for post-beta improvements
+ Prioritized based on user feedback 
