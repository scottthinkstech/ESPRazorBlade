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

### Phase 9: Publish Default Telemetry Metric Timeouts on Start

When the device starts, publish timeout values assigned to default telemetry to the MQTT server.

METRIC TIMEOUT PUBLISH
+ WiFi Signal Telemetry 
    - On MQTT connection, publish timeout in millisecond to topic "<device-id>/config/timeout/wifi_rssi"
+ Time Alive Telemetry
    - On MQTT connection, publish timeout in millisecond to topic "<device-id>/config/timeout/time_alive"
+ Free Heap Memrory
    - On MQTT connection, publish timeout in millisecond to topic "<device-id>/config/heap_memory"

DELIVERABLES
+ on load, metric timeouts are published to the correct topics


### Phase 11: Hot Load Default Telemetry Metric Timeouts

After device has started and connected to the MQTT server, subscribe to telemetry configuration topics. As the device is running, update the appropriate configuration value. Example, if "<device-id>/config/timeout/wifi_rssi" gets the value 90000 published, the Wifi RSSI telmetry metric timeout will change from whatever it is currently to 90000. T

METRIC CONFIGURATION
+ On load, load default metric configuration values from configuration.h
+ Subscribe to telemetry metric topics
+ When values are published to the topics, update the corresponding configuration value
+ Immediately send metric that had configuration value changed (e.g. if wifi rssi timeout was changed, send wifi rssi telemetry metric now)
+ Use the new configuration value (eg. wifi rssi would publish next metric in 90000 ms)

DELIVERABLES
+ when publishing to a telemetry metric topic, the device updates the metric config and begins using it in real time. 
