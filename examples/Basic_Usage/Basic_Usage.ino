/*
 * ESPRazorBlade Basic Usage Example
 *
 * 1) Copy Configuration.h.example to Configuration.h
 * 2) Fill in WiFi/MQTT settings and DEVICE_ID
 * 3) Upload to your board and open Serial Monitor at 115200 baud
 *
 * The library provides built-in telemetry: WiFi RSSI, time alive, free heap,
 * reset reason, and status. Use registerTelemetry() to add your own metrics.
 */

// Include Configuration.h first so the library uses this directory's config.
#include "Configuration.h"
#include "ESPRazorBlade.h"

ESPRazorBlade razorBlade;

const char* TOPIC_HEARTBEAT = DEVICE_ID "/telemetry/heartbeat";

const unsigned long STATUS_PRINT_INTERVAL_MS = 5000;
const unsigned long HEARTBEAT_INTERVAL_MS = 15000;

unsigned long lastStatusPrintMs = 0;
unsigned long lastHeartbeatMs = 0;

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("=== ESPRazorBlade Basic Usage ===");
    Serial.println("Starting ESPRazorBlade...");

    if (!razorBlade.begin()) {
        Serial.println("ERROR: Failed to initialize ESPRazorBlade.");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("ESPRazorBlade initialized.");
}

void loop() {
    unsigned long now = millis();

    if (now - lastStatusPrintMs >= STATUS_PRINT_INTERVAL_MS) {
        Serial.println();
        Serial.println("--- Connection Status ---");
        Serial.print("WiFi: ");
        Serial.println(razorBlade.isWiFiConnected() ? "Connected" : "Disconnected");

        if (razorBlade.isWiFiConnected()) {
            Serial.print("IP: ");
            Serial.println(razorBlade.getIPAddress());
        }

        Serial.print("MQTT: ");
        Serial.println(razorBlade.isMQTTConnected() ? "Connected" : "Disconnected");
        Serial.println("-------------------------");

        lastStatusPrintMs = now;
    }

    // Publish a simple heartbeat when MQTT is connected.
    if (razorBlade.isMQTTConnected() && (now - lastHeartbeatMs >= HEARTBEAT_INTERVAL_MS)) {
        if (razorBlade.publish(TOPIC_HEARTBEAT, "alive")) {
            Serial.print("Published heartbeat to: ");
            Serial.println(TOPIC_HEARTBEAT);
        } else {
            Serial.println("Heartbeat publish failed.");
        }
        lastHeartbeatMs = now;
    }

    delay(50);
}
