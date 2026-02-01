/*
 * ESPRazorBlade MQTT With Authentication Example
 * 
 * This example demonstrates secure MQTT connection with username/password
 * authentication, multiple telemetry streams, and retained messages.
 *
 * Use this example when:
 * - Your MQTT broker requires authentication (production environments)
 * - You need to publish to custom topics (not device-id prefixed)
 * - You want both automatic telemetry and manual publishing
 * 
 * Demonstrates:
 * - WiFi connection with automatic reconnection
 * - Authenticated MQTT connection (username/password)
 * - Custom telemetry callbacks with different intervals
 * - Manual publishing in loop() for additional metrics
 * - Retained messages for device status
 * 
 * Setup Instructions:
 * 1. Copy Configuration.h.example to Configuration.h
 * 2. Edit Configuration.h with your WiFi and MQTT broker details
 * 3. Uncomment and set MQTT_USERNAME and MQTT_PASSWORD in Configuration.h
 * 4. Upload to your ESP32 board
 * 5. Open Serial Monitor at 115200 baud
 *
 * Testing with Mosquitto (with authentication):
 *   # Subscribe to all topics
 *   mosquitto_sub -h <broker> -u <username> -P <password> -t "#" -v
 *
 *   # Subscribe to specific namespace
 *   mosquitto_sub -h <broker> -u <username> -P <password> -t "sensors/#" -v
 *   mosquitto_sub -h <broker> -u <username> -P <password> -t "device/#" -v
 */

#include "Configuration.h"
#include "ESPRazorBlade.h"

ESPRazorBlade razorBlade;

// Custom sensor reading functions
// Replace these simulated readings with your actual sensor code
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
    delay(200);
    
    Serial.println();
    Serial.println("=== ESPRazorBlade MQTT With Authentication Example ===");
    Serial.println();
    
    // Initialize library - authentication credentials from Configuration.h
    // If MQTT_USERNAME and MQTT_PASSWORD are defined, they're used automatically
    if (!razorBlade.begin()) {
        Serial.println("ERROR: Failed to initialize ESPRazorBlade.");
        Serial.println("Check that Configuration.h exists and credentials are correct.");
        while (true) {
            delay(1000);
        }
    }
    
    // Wait for connections with authentication
    Serial.println("Connecting with MQTT authentication...");
    unsigned long startTime = millis();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(500);
        Serial.print(".");
        
        if (millis() - startTime > 30000) {
            Serial.println();
            Serial.println("ERROR: Connection timeout!");
            Serial.println("Check WiFi credentials and MQTT authentication (username/password).");
            while (true) {
                delay(1000);
            }
        }
    }
    
    Serial.println();
    Serial.println("Connected successfully with authentication!");
    Serial.print("IP Address: ");
    Serial.println(razorBlade.getIPAddress());
    Serial.println();
    
    // Register custom telemetry streams
    // Note: Built-in metrics already publishing to <DEVICE_ID>/telemetry/*
    // These custom topics use different namespaces for organization
    razorBlade.registerTelemetry("sensors/temperature", readTemperature, 30000);
    Serial.println("Registered: sensors/temperature (30s interval)");
    
    razorBlade.registerTelemetry("sensors/humidity", readHumidity, 30000);
    Serial.println("Registered: sensors/humidity (30s interval)");
    
    razorBlade.registerTelemetry("sensors/pressure", readPressure, 60000);
    Serial.println("Registered: sensors/pressure (60s interval)");
    
    Serial.println();
    
    // Publish device info as retained messages
    // Retained messages persist on broker for new subscribers
    razorBlade.publish("device/status", "online", true);
    razorBlade.publish("device/firmware", "mqtt_auth_v1.0", true);
    razorBlade.publish("device/ip", razorBlade.getIPAddress().c_str(), true);
    
    Serial.println("Device info published (retained).");
    Serial.println("Setup complete! Telemetry streams active.");
    Serial.println();
}

void loop() {
    // Your application code goes here
    
    // Example: Publish additional system metrics manually every minute
    // This demonstrates manual publishing in loop() alongside automatic telemetry
    static unsigned long lastManualMetrics = 0;
    if (razorBlade.isMQTTConnected() && (millis() - lastManualMetrics > 60000)) {
        // Publish system metrics to device namespace
        razorBlade.publish("device/uptime", (long)(millis() / 1000));
        razorBlade.publish("device/free_heap", (int)ESP.getFreeHeap());
        razorBlade.publish("device/rssi", (int)WiFi.RSSI());
        
        Serial.println("Published manual system metrics to device/* topics");
        
        lastManualMetrics = millis();
    }
    
    delay(100);
}
