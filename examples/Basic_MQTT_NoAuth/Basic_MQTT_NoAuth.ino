/*
 * Basic MQTT Connection Without Authentication
 * 
 * This example demonstrates how to:
 * - Connect to WiFi
 * - Connect to an MQTT broker without authentication
 * - Publish telemetry data automatically
 * - Manually publish messages
 * 
 * Perfect for: Local development, testing, public test brokers
 * 
 * Hardware: ESP32 (any variant: ESP32-C3, C6, S3, etc.)
 * 
 * Setup:
 * 1. Copy Configuration.h.example to Configuration.h
 * 2. Edit Configuration.h with your WiFi and MQTT broker details
 * 3. Leave MQTT_USERNAME and MQTT_PASSWORD commented out
 * 4. Upload and open Serial Monitor at 115200 baud
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

// Simulated sensor reading - replace with your actual sensor
String readTemperature() {
    // Example: Read from DHT22, DS18B20, or other sensor
    float temp = 20.0 + (random(0, 50) / 10.0);  // Simulated: 20-25°C
    return String(temp, 1);
}

String readHumidity() {
    // Example: Read from DHT22 or other humidity sensor
    float humidity = 60.0 + (random(0, 100) / 10.0);  // Simulated: 60-70%
    return String(humidity, 1);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n=================================");
    Serial.println("Basic MQTT - No Authentication");
    Serial.println("=================================\n");
    
    // Initialize library (starts WiFi and MQTT tasks)
    Serial.println("Initializing ESPRazorBlade...");
    if (!razorBlade.begin()) {
        Serial.println("ERROR: Failed to initialize!");
        Serial.println("Check your Configuration.h file");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("Connecting to WiFi and MQTT broker...");
    Serial.println("(This may take a few seconds)");
    
    // Wait for both connections
    unsigned long startTime = millis();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        Serial.print(".");
        delay(500);
        
        // Timeout after 30 seconds
        if (millis() - startTime > 30000) {
            Serial.println("\n\nERROR: Connection timeout!");
            Serial.println("Check your WiFi and MQTT broker settings");
            while (1) delay(1000);
        }
    }
    
    Serial.println("\n\n✓ Connected successfully!");
    Serial.println("---------------------------------");
    Serial.print("IP Address: ");
    Serial.println(razorBlade.getIPAddress());
    Serial.print("WiFi RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.println("---------------------------------\n");
    
    // Register automatic telemetry callbacks
    // These will publish automatically at the specified intervals
    Serial.println("Registering telemetry callbacks...");
    razorBlade.registerTelemetry("home/esp32/temperature", readTemperature, 30000);  // Every 30 sec
    razorBlade.registerTelemetry("home/esp32/humidity", readHumidity, 30000);        // Every 30 sec
    Serial.println("✓ Telemetry registered\n");
    
    // Publish startup messages
    Serial.println("Publishing startup messages...");
    razorBlade.publish("home/esp32/status", "online", true);  // Retained message
    razorBlade.publish("home/esp32/firmware", "1.0.0");
    razorBlade.publish("home/esp32/ip", razorBlade.getIPAddress().c_str());
    Serial.println("✓ Startup messages sent\n");
    
    Serial.println("Setup complete! Device is running.");
    Serial.println("Telemetry will be published every 30 seconds.");
    Serial.println("Watch your MQTT broker for messages on topics:");
    Serial.println("  - home/esp32/status");
    Serial.println("  - home/esp32/temperature");
    Serial.println("  - home/esp32/humidity");
    Serial.println("  - home/esp32/heartbeat\n");
}

void loop() {
    // Your main application code goes here
    // The library handles all WiFi and MQTT operations in background tasks
    
    // Example: Send heartbeat every minute
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 60000) {
        if (razorBlade.isMQTTConnected()) {
            unsigned long uptimeSeconds = millis() / 1000;
            razorBlade.publish("home/esp32/heartbeat", uptimeSeconds);
            
            Serial.print("Heartbeat sent - Uptime: ");
            Serial.print(uptimeSeconds);
            Serial.println(" seconds");
        }
        lastHeartbeat = millis();
    }
    
    // Example: Print connection status every 10 seconds
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000) {
        Serial.print("Status: WiFi=");
        Serial.print(razorBlade.isWiFiConnected() ? "OK" : "DISCONNECTED");
        Serial.print(" | MQTT=");
        Serial.println(razorBlade.isMQTTConnected() ? "OK" : "DISCONNECTED");
        lastStatus = millis();
    }
    
    // Add your application logic here
    // - Read sensors
    // - Control actuators
    // - Respond to events
    // - etc.
    
    delay(100);
}
