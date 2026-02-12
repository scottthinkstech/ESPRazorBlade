/*
 * Basic MQTT Without Authentication Example
 * 
 * Demonstrates:
 * - WiFi connection
 * - MQTT connection (no authentication)
 * - Automatic telemetry publishing
 * - Manual message publishing
 * 
 * Setup:
 * 1. Copy Configuration.h.example to Configuration.h
 * 2. Edit Configuration.h with your WiFi and MQTT broker details
 * 3. Upload and open Serial Monitor at 115200 baud
 */

#include "Configuration.h"
#include "ESPRazorBlade.h"

ESPRazorBlade razorBlade;

// Sensor reading functions - replace with your actual sensors
String readTemperature() {
    float temp = 20.0 + (random(0, 50) / 10.0);  // Simulated 20-25°C
    return String(temp, 1);
}

String readHumidity() {
    float humidity = 60.0 + (random(0, 100) / 10.0);  // Simulated 60-70%
    return String(humidity, 1);
}

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("\n=== Basic MQTT Example (No Auth) ===\n");
    
    // // Initialize library
    // if (!razorBlade.begin()) {
    //     Serial.println("ERROR: Failed to initialize! Check Configuration.h");
    //     while (1) delay(1000);
    // }
    
    // // Wait for connections
    // Serial.println("Connecting...");
    // unsigned long startTime = millis();
    // while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
    //     delay(500);
    //     if (millis() - startTime > 30000) {
    //         Serial.println("ERROR: Connection timeout!");
    //         while (1) delay(1000);
    //     }
    // }
    
    // Serial.println("Connected!");
    // Serial.print("IP: ");
    // Serial.println(razorBlade.getIPAddress());
    
    // // Register telemetry (publishes every 30 seconds)
    // razorBlade.registerTelemetry("home/esp32/temperature", readTemperature, 30000);
    // razorBlade.registerTelemetry("home/esp32/humidity", readHumidity, 30000);
    
    // // Publish startup message
    // razorBlade.publish("home/esp32/status", "online", true);
    
    Serial.println("Setup complete!\n");
}

void loop() {
    // Your code here - library runs in background
    
    // Optional: Send heartbeat every minute
    // static unsigned long lastHeartbeat = 0;
    // if (millis() - lastHeartbeat > 60000) {
    //     razorBlade.publish("home/esp32/heartbeat", millis() / 1000);
    //     lastHeartbeat = millis();
    // }
    
    delay(2000);
    Serial.println("Loop complete!");
}
