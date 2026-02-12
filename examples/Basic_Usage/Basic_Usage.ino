/*
 * ESPRazorBlade Basic Usage Example
 *
 * 1) Copy Configuration.h.example to Configuration.h
 * 2) Fill in WiFi/MQTT settings and DEVICE_ID
 * 3) Upload to your board and open Serial Monitor at 115200 baud
 */

#include "ESPRazorBlade.h"
#include "Configuration.h"

ESPRazorBlade razorBlade;

// DEVICE_ID comes from Configuration.h and is used as the MQTT topic prefix.
const char* TOPIC_STATUS = DEVICE_ID "/status";
const char* TOPIC_HEARTBEAT = DEVICE_ID "/telemetry/heartbeat";
const char* TOPIC_RSSI = DEVICE_ID "/telemetry/wifi_rssi";

const unsigned long STATUS_PRINT_INTERVAL_MS = 5000;
const unsigned long HEARTBEAT_INTERVAL_MS = 15000;

unsigned long lastStatusPrintMs = 0;
unsigned long lastHeartbeatMs = 0;

String readWiFiRSSI() {
    return String(WiFi.RSSI());
}

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

    // Register WiFi signal strength telemetry (interval from Configuration.h).
    razorBlade.registerTelemetry(TOPIC_RSSI, readWiFiRSSI, WIFI_SIGNAL_INTERVAL_MS);

    // Retained status helps dashboards know the latest device state.
    razorBlade.publish(TOPIC_STATUS, "online", true);

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
