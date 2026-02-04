/*
 * MQTT Connection With Authentication
 * 
 * This example demonstrates how to:
 * - Connect to WiFi
 * - Connect to an MQTT broker WITH username/password authentication
 * - Publish multiple telemetry streams
 * - Use retained messages
 * - Publish device metrics
 * 
 * Perfect for: Production deployments, secure brokers, cloud MQTT services
 * 
 * Hardware: ESP32 (any variant: ESP32-C3, C6, S3, etc.)
 * 
 * Setup:
 * 1. Copy Configuration.h.example to Configuration.h
 * 2. Edit Configuration.h with your WiFi and MQTT broker details
 * 3. UNCOMMENT and fill in MQTT_USERNAME and MQTT_PASSWORD
 * 4. Upload and open Serial Monitor at 115200 baud
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

// Simulated sensor readings - replace with your actual sensors
String readTemperature() {
    // Example: Read from DHT22, DS18B20, BME280, etc.
    float temp = 22.0 + (random(0, 30) / 10.0);  // Simulated: 22-25°C
    return String(temp, 1);
}

String readHumidity() {
    // Example: Read from DHT22, BME280, etc.
    float humidity = 65.0 + (random(0, 50) / 10.0);  // Simulated: 65-70%
    return String(humidity, 1);
}

String readPressure() {
    // Example: Read from BME280, BMP180, etc.
    float pressure = 1013.0 + (random(-20, 20) / 10.0);  // Simulated: ~1013 hPa
    return String(pressure, 1);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n====================================");
    Serial.println("MQTT With Authentication");
    Serial.println("====================================\n");
    
    // Initialize library (starts WiFi and MQTT tasks with auth)
    Serial.println("Initializing ESPRazorBlade with authentication...");
    if (!razorBlade.begin()) {
        Serial.println("ERROR: Failed to initialize!");
        Serial.println("Check your Configuration.h file");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("Connecting to WiFi and MQTT broker...");
    Serial.println("(Authenticating with username/password)");
    
    // Wait for both connections
    unsigned long startTime = millis();
    int dotCount = 0;
    while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        Serial.print(".");
        dotCount++;
        if (dotCount % 40 == 0) Serial.println();
        delay(500);
        
        // Timeout after 30 seconds
        if (millis() - startTime > 30000) {
            Serial.println("\n\nERROR: Connection timeout!");
            Serial.println("Possible causes:");
            Serial.println("  - WiFi credentials incorrect");
            Serial.println("  - MQTT broker unreachable");
            Serial.println("  - MQTT username/password incorrect");
            Serial.println("  - MQTT broker not configured for authentication");
            while (1) delay(1000);
        }
    }
    
    Serial.println("\n\n✓ Authenticated and connected!");
    Serial.println("---------------------------------");
    Serial.print("IP Address: ");
    Serial.println(razorBlade.getIPAddress());
    Serial.print("WiFi RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.println("---------------------------------\n");
    
    // Register multiple telemetry streams
    // These will publish automatically at the specified intervals
    Serial.println("Registering telemetry callbacks...");
    razorBlade.registerTelemetry("sensors/temperature", readTemperature, 30000);  // Every 30 sec
    razorBlade.registerTelemetry("sensors/humidity", readHumidity, 30000);        // Every 30 sec
    razorBlade.registerTelemetry("sensors/pressure", readPressure, 60000);        // Every 60 sec
    Serial.println("✓ Telemetry registered (3 streams)\n");
    
    // Publish device information with retained messages
    Serial.println("Publishing device information...");
    razorBlade.publish("device/status", "online", true);           // Retained
    razorBlade.publish("device/firmware", "1.0.0", true);          // Retained
    razorBlade.publish("device/hardware", "ESP32-C3", true);       // Retained
    razorBlade.publish("device/ip", razorBlade.getIPAddress().c_str());
    razorBlade.publish("device/mac", WiFi.macAddress().c_str());
    Serial.println("✓ Device info published\n");
    
    Serial.println("Setup complete! Authenticated device is running.");
    Serial.println("Telemetry streams active:");
    Serial.println("  - sensors/temperature (every 30s)");
    Serial.println("  - sensors/humidity (every 30s)");
    Serial.println("  - sensors/pressure (every 60s)");
    Serial.println("  - device/metrics (every 60s)");
    Serial.println("  - device/heartbeat (every 60s)\n");
}

void loop() {
    // Your main application code goes here
    // The library handles all WiFi and MQTT operations in background tasks
    // Authentication is handled automatically on reconnection
    
    // Publish system metrics every minute
    static unsigned long lastMetrics = 0;
    if (millis() - lastMetrics > 60000) {
        if (razorBlade.isMQTTConnected()) {
            // System uptime
            unsigned long uptimeSeconds = millis() / 1000;
            razorBlade.publish("device/uptime", uptimeSeconds);
            
            // Memory stats
            razorBlade.publish("device/free_heap", ESP.getFreeHeap());
            razorBlade.publish("device/heap_size", ESP.getHeapSize());
            
            // WiFi signal strength
            razorBlade.publish("device/rssi", WiFi.RSSI());
            
            Serial.println("System metrics published:");
            Serial.print("  Uptime: ");
            Serial.print(uptimeSeconds);
            Serial.println(" seconds");
            Serial.print("  Free Heap: ");
            Serial.print(ESP.getFreeHeap());
            Serial.println(" bytes");
            Serial.print("  WiFi RSSI: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm\n");
        }
        lastMetrics = millis();
    }
    
    // Send heartbeat every minute
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 60000) {
        if (razorBlade.isMQTTConnected()) {
            razorBlade.publish("device/heartbeat", "alive");
            Serial.println("Heartbeat sent");
        }
        lastHeartbeat = millis();
    }
    
    // Print connection status every 30 seconds
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 30000) {
        Serial.print("Connection Status: WiFi=");
        Serial.print(razorBlade.isWiFiConnected() ? "Connected" : "DISCONNECTED");
        Serial.print(" | MQTT=");
        Serial.print(razorBlade.isMQTTConnected() ? "Connected" : "DISCONNECTED");
        
        if (razorBlade.isWiFiConnected()) {
            Serial.print(" | IP=");
            Serial.print(razorBlade.getIPAddress());
        }
        Serial.println();
        
        lastStatus = millis();
    }
    
    // Add your authenticated application logic here
    // Examples:
    // - Read real sensors (DHT22, BME280, DS18B20, etc.)
    // - Control actuators based on MQTT commands
    // - Log data to cloud services
    // - Implement security features
    // - etc.
    
    delay(100);
}
