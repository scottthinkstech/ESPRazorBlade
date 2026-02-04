/*
 * MQTT With Authentication Example
 * 
 * Demonstrates:
 * - WiFi connection
 * - MQTT connection with username/password
 * - Multiple telemetry streams
 * - System metrics publishing
 * - Retained messages
 * 
 * Setup:
 * 1. Copy Configuration.h.example to Configuration.h
 * 2. Edit Configuration.h with your WiFi and MQTT broker details
 * 3. Set MQTT_USERNAME and MQTT_PASSWORD in Configuration.h
 * 4. Upload and open Serial Monitor at 115200 baud
 */

#include "ESPRazorBlade.h"
#include "Configuration.h"

ESPRazorBlade razorBlade;

// Sensor reading functions - replace with your actual sensors
String readTemperature() {
    float temp = 22.0 + (random(0, 30) / 10.0);  // Simulated 22-25°C
    return String(temp, 1);
}

String readHumidity() {
    float humidity = 65.0 + (random(0, 50) / 10.0);  // Simulated 65-70%
    return String(humidity, 1);
}

String readPressure() {
    float pressure = 1013.0 + (random(-20, 20) / 10.0);  // Simulated ~1013 hPa
    return String(pressure, 1);
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== MQTT Example (With Auth) ===\n");
    
    // Initialize library with authentication
    if (!razorBlade.begin()) {
        Serial.println("ERROR: Failed to initialize! Check Configuration.h");
        while (1) delay(1000);
    }
    
    // Wait for connections
    Serial.println("Connecting with authentication...");
    unsigned long startTime = millis();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(500);
        if (millis() - startTime > 30000) {
            Serial.println("ERROR: Connection timeout!");
            Serial.println("Check WiFi and MQTT credentials");
            while (1) delay(1000);
        }
    }
    
    Serial.println("Connected!");
    Serial.print("IP: ");
    Serial.println(razorBlade.getIPAddress());
    
    // Register telemetry streams
    razorBlade.registerTelemetry("sensors/temperature", readTemperature, 30000);
    razorBlade.registerTelemetry("sensors/humidity", readHumidity, 30000);
    razorBlade.registerTelemetry("sensors/pressure", readPressure, 60000);
    
    // Publish device info (retained)
    razorBlade.publish("device/status", "online", true);
    razorBlade.publish("device/firmware", "1.0.0", true);
    razorBlade.publish("device/ip", razorBlade.getIPAddress().c_str());
    
    Serial.println("Setup complete!\n");
}

void loop() {
    // Your code here - library runs in background
    
    // Publish system metrics every minute
    static unsigned long lastMetrics = 0;
    if (millis() - lastMetrics > 60000) {
        razorBlade.publish("device/uptime", millis() / 1000);
        razorBlade.publish("device/free_heap", ESP.getFreeHeap());
        razorBlade.publish("device/rssi", WiFi.RSSI());
        lastMetrics = millis();
    }
    
    delay(100);
}
