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
+ Default telemetry metrics working: WiFi RSSI, time alive, reset reason (broadcast to MQTT when connected)

## Where We Left Off (2025-02-11)

Default telemetry metrics (WiFi RSSI, time alive, reset reason) are working. Next development task: add heap memory telemetry metric.

## Phased Development Plan

### Phase X: Hot Configurable Telemetry Metrics

Make the timeouts for publishing telemetry info configurable by putting their values in the Configuration.h file. On load, the timeout values will be pulled from Configuration. Once connected to the MQTT server, the server will check configured MQTT topics to see if a different value has been published and retained. If a value is located for the configuration item at the MQTT topic, update running logic to utlize the new value, else, use the value defined in `Configuration.h`.

METRIC CONFIGURATION
+ WiFi Signal Telemetry 
    - Publish Timeout in milliseconds
    - MQTT topic to subscribe to containing the mutable timeout in ms
+ Time Alive Telemetry
    - Publish Timeout in milliseconds
    - MQTT topic to subscribe to containing the mutable timeout in ms
+ Free Heap Memrory
    - Publish Timeout in milliseconds
    - MQTT topic to subscribe to containing the mutable timeout in ms

DELIVERABLES
+ all specified telemetry configuration items can be set in Configuration.h
+ all specified telemetry configuration items can be changed by publishing to the correct MQTT topic
