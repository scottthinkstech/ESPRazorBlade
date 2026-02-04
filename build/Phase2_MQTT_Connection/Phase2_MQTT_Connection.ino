/*
 * ESPRazorBlade Phase 2 Test: MQTT Connection
 * 
 * This sketch tests the MQTT connection functionality of ESPRazorBlade.
 * 
 * Test Checklist:
 * - [ ] Code compiles without errors
 * - [ ] WiFi connects successfully
 * - [ ] MQTT connects successfully
 * - [ ] Connection status displayed in Serial Monitor
 * - [ ] Keepalive maintains connection for 2+ minutes
 * - [ ] Reconnection works after broker disconnect
 * 
 * Make sure to configure your WiFi and MQTT settings in Configuration.h
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

void setup() {
    // Initialize the library
    // This starts WiFi and MQTT connection tasks automatically
    if (!razorBlade.begin()) {
        Serial.println("Failed to initialize ESPRazorBlade!");
        while (1) {
            delay(1000); // Halt if initialization fails
        }
    }
    
    Serial.println("\n=== Phase 2 MQTT Connection Test ===");
    Serial.println("Waiting for WiFi and MQTT connections...");
    Serial.println("(This may take a few seconds)\n");
}

void loop() {
    // Check connection status periodically
    static unsigned long lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 5000) { // Every 5 seconds
        Serial.println("--- Status Check ---");
        
        // WiFi status
        if (razorBlade.isWiFiConnected()) {
            Serial.println("WiFi: Connected");
            Serial.print("IP Address: ");
            Serial.println(razorBlade.getIPAddress());
            Serial.print("RSSI: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
        } else {
            Serial.println("WiFi: Disconnected");
            Serial.println("(Reconnection will be attempted automatically)");
        }
        
        // MQTT status
        if (razorBlade.isMQTTConnected()) {
            Serial.println("MQTT: Connected");
        } else {
            Serial.println("MQTT: Disconnected");
            if (razorBlade.isWiFiConnected()) {
                Serial.println("(Reconnection will be attempted automatically)");
            } else {
                Serial.println("(Waiting for WiFi connection)");
            }
        }
        
        Serial.println("-------------------\n");
        lastStatusCheck = millis();
    }
    
    // Small delay to prevent tight loop
    delay(100);
}
