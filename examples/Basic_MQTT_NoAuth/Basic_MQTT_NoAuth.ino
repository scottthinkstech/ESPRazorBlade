/*
 * ESPRazorBlade Custom Telemetry Example
 * 
 * This example demonstrates how to add your own custom telemetry callbacks
 * in addition to the built-in system metrics. Perfect for publishing sensor
 * data like temperature, humidity, pressure, etc.
 *
 * Demonstrates:
 * - Using registerTelemetry() to add custom sensor callbacks
 * - Different publish intervals for different sensors
 * - Simulated sensor readings (replace with real sensors)
 * - Manual one-time publishing with publish()
 * - Both retained and non-retained messages
 * 
 * Setup Instructions:
 * 1. Copy Configuration.h.example to Configuration.h
 * 2. Edit Configuration.h with your WiFi and MQTT broker details
 * 3. Leave MQTT_USERNAME and MQTT_PASSWORD commented out (no auth)
 * 4. Upload to your ESP32 board
 * 5. Open Serial Monitor at 115200 baud
 *
 * Monitor published data with:
 *   mosquitto_sub -h <broker> -t "<DEVICE_ID>/#" -v
 *
 * You'll see:
 * - Built-in metrics (WiFi RSSI, uptime, heap)
 * - Custom sensor data (temperature, humidity, pressure)
 */

#include "Configuration.h"
#include "ESPRazorBlade.h"

ESPRazorBlade razorBlade;

// Custom telemetry callback functions
// These return String values that get published to MQTT at specified intervals
// Replace these simulated readings with your actual sensor code!

String readTemperature() {
    // Simulated temperature reading between 20-25°C
    // Replace with: DHT22, BMP280, DS18B20, etc.
    float temp = 20.0 + (random(0, 50) / 10.0);
    return String(temp, 1);
}

String readHumidity() {
    // Simulated humidity reading between 60-70%
    // Replace with: DHT22, BME280, etc.
    float humidity = 60.0 + (random(0, 100) / 10.0);
    return String(humidity, 1);
}

String readPressure() {
    // Simulated pressure reading around 1013 hPa
    // Replace with: BMP280, BME280, etc.
    float pressure = 1013.0 + (random(-50, 50) / 10.0);
    return String(pressure, 1);
}

void setup() {
    Serial.begin(115200);
    delay(200);
    
    Serial.println();
    Serial.println("=== ESPRazorBlade Custom Telemetry Example ===");
    Serial.println();
    
    // Initialize library - starts WiFi, MQTT, and built-in telemetry
    if (!razorBlade.begin()) {
        Serial.println("ERROR: Failed to initialize ESPRazorBlade.");
        Serial.println("Check that Configuration.h exists and is properly configured.");
        while (true) {
            delay(1000);
        }
    }
    
    Serial.println("ESPRazorBlade initialized successfully!");
    Serial.println("Waiting for WiFi and MQTT connection...");
    
    // Wait for connections with timeout
    unsigned long startTime = millis();
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(500);
        Serial.print(".");
        
        if (millis() - startTime > 30000) {
            Serial.println();
            Serial.println("ERROR: Connection timeout!");
            Serial.println("Check WiFi credentials and MQTT broker availability.");
            while (true) {
                delay(1000);
            }
        }
    }
    
    Serial.println();
    Serial.println("Connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(razorBlade.getIPAddress());
    Serial.println();
    
    // Register custom telemetry callbacks
    // Note: Built-in metrics (WiFi RSSI, uptime, heap) are already registered
    // The library supports up to 10 total callbacks (3 built-in + 7 custom max)
    
    // Temperature: publish every 30 seconds (30000 ms)
    String tempTopic = String(DEVICE_ID) + "/telemetry/temperature";
    razorBlade.registerTelemetry(tempTopic.c_str(), readTemperature, 30000);
    Serial.println("Registered: Temperature telemetry (30s interval)");
    
    // Humidity: publish every 45 seconds (different interval!)
    String humidityTopic = String(DEVICE_ID) + "/telemetry/humidity";
    razorBlade.registerTelemetry(humidityTopic.c_str(), readHumidity, 45000);
    Serial.println("Registered: Humidity telemetry (45s interval)");
    
    // Pressure: publish every 60 seconds
    String pressureTopic = String(DEVICE_ID) + "/telemetry/pressure";
    razorBlade.registerTelemetry(pressureTopic.c_str(), readPressure, 60000);
    Serial.println("Registered: Pressure telemetry (60s interval)");
    
    Serial.println();
    
    // Publish one-time startup messages (retained)
    // Retained messages persist on the broker for new subscribers
    String statusTopic = String(DEVICE_ID) + "/info/status";
    razorBlade.publish(statusTopic.c_str(), "online", true);
    
    String firmwareTopic = String(DEVICE_ID) + "/info/firmware";
    razorBlade.publish(firmwareTopic.c_str(), "custom_telemetry_v1.0", true);
    
    Serial.println("Setup complete! Custom telemetry is now active.");
    Serial.println("All callbacks will publish automatically at their intervals.");
    Serial.println();
}

void loop() {
    // Your application code goes here!
    // The library handles all telemetry publishing automatically in background tasks.
    // No need to call publish() in loop() for registered telemetry.
    
    // Optional: Print status periodically to show it's working
    static unsigned long lastStatusPrint = 0;
    if (millis() - lastStatusPrint > 10000) {
        Serial.print("Status check - WiFi: ");
        Serial.print(razorBlade.isWiFiConnected() ? "OK" : "DISCONNECTED");
        Serial.print(", MQTT: ");
        Serial.println(razorBlade.isMQTTConnected() ? "OK" : "DISCONNECTED");
        
        lastStatusPrint = millis();
    }
    
    delay(100);
}
