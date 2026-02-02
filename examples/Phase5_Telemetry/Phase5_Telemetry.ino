/*
 * ESPRazorBlade Phase 5 Test: Telemetry Callback System
 * 
 * This sketch demonstrates the telemetry callback system of ESPRazorBlade.
 * Telemetry callbacks allow you to automatically publish sensor data or
 * other values at regular intervals without manually calling publish().
 * 
 * Test Checklist:
 * - [ ] Code compiles without errors
 * - [ ] WiFi connects successfully
 * - [ ] MQTT connects successfully
 * - [ ] Telemetry callbacks execute at correct intervals
 * - [ ] Multiple callbacks work simultaneously
 * - [ ] Data appears on MQTT broker with correct topics
 * 
 * Make sure to configure your WiFi and MQTT settings in Configuration.h
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

// Example sensor simulation variables
int sensorCounter = 0;
float simulatedTemperature = 22.5;

// Telemetry callback functions
// These functions return a String that will be published to the MQTT topic

String getCounterTelemetry() {
    sensorCounter++;
    return String(sensorCounter);
}

String getTemperatureTelemetry() {
    // Simulate temperature reading (22.0 to 25.0 degrees)
    simulatedTemperature = 22.0 + (random(0, 30) / 10.0);
    return String(simulatedTemperature, 1); // 1 decimal place
}

String getUptimeTelemetry() {
    // Return uptime in seconds
    unsigned long uptimeSeconds = millis() / 1000;
    return String(uptimeSeconds);
}

String getStatusTelemetry() {
    // Return connection status
    String status = "WiFi: ";
    status += razorBlade.isWiFiConnected() ? "Connected" : "Disconnected";
    status += ", MQTT: ";
    status += razorBlade.isMQTTConnected() ? "Connected" : "Disconnected";
    return status;
}

void setup() {
    // Initialize the library
    if (!razorBlade.begin()) {
        Serial.println("Failed to initialize ESPRazorBlade!");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("\n=== Phase 5 Telemetry Callback Test ===");
    Serial.println("Waiting for WiFi and MQTT connections...");
    Serial.println("(This may take a few seconds)\n");
    
    // Wait for connections before registering telemetry
    // (Telemetry will only publish when MQTT is connected)
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(500);
    }
    
    Serial.println("\n--- Registering Telemetry Callbacks ---");
    
    // Register telemetry callbacks
    // Each callback will execute at its specified interval and publish to the topic
    
    // Counter that updates every 5 seconds
    if (razorBlade.registerTelemetry("telemetry/counter", getCounterTelemetry, 5000)) {
        Serial.println("✓ Registered: telemetry/counter (5s interval)");
    } else {
        Serial.println("✗ Failed to register: telemetry/counter");
    }
    
    // Temperature sensor that updates every 10 seconds
    if (razorBlade.registerTelemetry("telemetry/temperature", getTemperatureTelemetry, 10000)) {
        Serial.println("✓ Registered: telemetry/temperature (10s interval)");
    } else {
        Serial.println("✗ Failed to register: telemetry/temperature");
    }
    
    // Uptime that updates every 15 seconds
    if (razorBlade.registerTelemetry("telemetry/uptime", getUptimeTelemetry, 15000)) {
        Serial.println("✓ Registered: telemetry/uptime (15s interval)");
    } else {
        Serial.println("✗ Failed to register: telemetry/uptime");
    }
    
    // Status that updates every 30 seconds
    if (razorBlade.registerTelemetry("telemetry/status", getStatusTelemetry, 30000)) {
        Serial.println("✓ Registered: telemetry/status (30s interval)");
    } else {
        Serial.println("✗ Failed to register: telemetry/status");
    }
    
    Serial.println("\n--- Telemetry System Active ---");
    Serial.println("Callbacks will execute automatically in the background.");
    Serial.println("Check your MQTT broker to see the published data.\n");
}

void loop() {
    // Main loop is free to do other work
    // Telemetry callbacks execute automatically in the background RTOS task
    
    // Print status every 60 seconds
    static unsigned long lastStatusPrint = 0;
    if (millis() - lastStatusPrint > 60000) {
        Serial.println("\n--- Status Update ---");
        Serial.print("WiFi: ");
        Serial.println(razorBlade.isWiFiConnected() ? "Connected" : "Disconnected");
        Serial.print("MQTT: ");
        Serial.println(razorBlade.isMQTTConnected() ? "Connected" : "Disconnected");
        Serial.print("Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
        Serial.println("-------------------\n");
        lastStatusPrint = millis();
    }
    
    // Small delay to prevent tight loop
    delay(100);
}
