/*
 * ESPRazorBlade Basic Usage Example
 *
 * This minimal example demonstrates:
 * - Automatic WiFi connection and reconnection
 * - Automatic MQTT connection and reconnection
 * - Built-in telemetry metrics (WiFi RSSI, uptime, free heap, reset reason)
 * - No custom callbacks needed - everything runs automatically!
 *
 * Setup Instructions:
 * 1) Copy Configuration.h.example to Configuration.h
 * 2) Edit Configuration.h with your WiFi and MQTT broker details
 * 3) Upload to your ESP32 board
 * 4) Open Serial Monitor at 115200 baud
 *
 * Once connected, the device automatically publishes to these MQTT topics:
 * - <DEVICE_ID>/status                           ("online", retained)
 * - <DEVICE_ID>/telemetry/wifi_rssi              (signal strength)
 * - <DEVICE_ID>/telemetry/time_alive             (uptime)
 * - <DEVICE_ID>/telemetry/free_heap              (available memory)
 * - <DEVICE_ID>/telemetry/reset_reason           (why device restarted)
 * - <DEVICE_ID>/config/telemetry/timeouts/*      (current interval settings)
 *
 * Monitor with: mosquitto_sub -h <broker> -t "<DEVICE_ID>/#" -v
 */

#include "Configuration.h"
#include "ESPRazorBlade.h"

ESPRazorBlade razorBlade;

const unsigned long STATUS_PRINT_INTERVAL_MS = 5000;
unsigned long lastStatusPrintMs = 0;

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("=== ESPRazorBlade Basic Usage Example ===");
    Serial.println("Initializing ESPRazorBlade...");
    Serial.println();

    // Initialize the library - this starts WiFi and MQTT connection tasks
    // and registers all built-in telemetry metrics automatically
    if (!razorBlade.begin()) {
        Serial.println("ERROR: Failed to initialize ESPRazorBlade.");
        Serial.println("Check Configuration.h exists and is properly configured.");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("ESPRazorBlade initialized successfully!");
    Serial.println("Built-in telemetry is now active.");
    Serial.println("WiFi and MQTT tasks running in background.");
    Serial.println();
}

void loop() {
    unsigned long now = millis();

    // Print connection status periodically to Serial Monitor
    if (now - lastStatusPrintMs >= STATUS_PRINT_INTERVAL_MS) {
        Serial.println("--- Connection Status ---");
        Serial.print("WiFi: ");
        Serial.println(razorBlade.isWiFiConnected() ? "Connected" : "Disconnected");

        if (razorBlade.isWiFiConnected()) {
            Serial.print("IP Address: ");
            Serial.println(razorBlade.getIPAddress());
        }

        Serial.print("MQTT: ");
        Serial.println(razorBlade.isMQTTConnected() ? "Connected" : "Disconnected");
        
        if (razorBlade.isMQTTConnected()) {
            Serial.println("Built-in telemetry publishing automatically.");
        }
        
        Serial.println("-------------------------");
        Serial.println();

        lastStatusPrintMs = now;
    }

    // Your application code goes here!
    // The library handles all telemetry publishing in the background.
    
    delay(100);
}
