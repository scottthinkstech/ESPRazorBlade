/*
 * ESPRazorBlade Phase 4 Test: RTOS Task Integration
 *
 * This sketch verifies that the WiFi/MQTT management runs in RTOS tasks
 * while the main loop remains free for application work.
 *
 * Test Checklist:
 * - [ ] Code compiles without errors
 * - [ ] WiFi connects successfully
 * - [ ] MQTT connects successfully
 * - [ ] Main loop stays responsive (no blocking)
 * - [ ] Publish works while RTOS task runs
 * - [ ] Reconnection works after disconnect
 *
 * Make sure to configure your WiFi and MQTT settings in Configuration.h
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

// Simple counter to publish periodically
unsigned long publishCounter = 0;

void setup() {
    // Initialize the library (starts WiFi + MQTT RTOS tasks)
    if (!razorBlade.begin()) {
        Serial.println("Failed to initialize ESPRazorBlade!");
        while (1) {
            delay(1000);
        }
    }

    Serial.println("\n=== Phase 4 RTOS Integration Test ===");
    Serial.println("WiFi/MQTT run in background tasks.");
    Serial.println("Main loop should remain responsive.\n");
}

void loop() {
    // If connections are not ready, keep main loop alive and responsive
    if (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        static unsigned long lastWaitPrint = 0;
        if (millis() - lastWaitPrint > 2000) {
            Serial.println("Waiting for WiFi/MQTT (background tasks running)...");
            lastWaitPrint = millis();
        }
        delay(100);
        return;
    }

    // Publish a heartbeat message every 5 seconds
    static unsigned long lastPublish = 0;
    if (millis() - lastPublish > 5000) {
        publishCounter++;
        bool ok = razorBlade.publish("phase4/heartbeat", (long)publishCounter);
        if (ok) {
            Serial.print("✓ Published heartbeat #");
            Serial.println(publishCounter);
        } else {
            Serial.println("✗ Failed to publish heartbeat");
        }
        lastPublish = millis();
    }

    // Simulated main loop work (non-blocking)
    // This should continue even while MQTT is polled in RTOS task.
    delay(50);
}
