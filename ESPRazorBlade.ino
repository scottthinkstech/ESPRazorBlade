/*
 * ESPRazorBlade Example Sketch - Phase 1: WiFi Only
 * 
 * Demonstrates how to use the ESPRazorBlade library Phase 1 features:
 * - Connect to WiFi automatically
 * - Monitor WiFi connection status
 * - Automatic reconnection
 * 
 * Make sure to configure your WiFi settings in Configuration.h
 * 
 * Note: MQTT features will be available in later phases
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

void setup() {
    // Initialize the library
    // This starts WiFi connection task automatically
    if (!razorBlade.begin()) {
        Serial.println("Failed to initialize ESPRazorBlade!");
        while (1) {
            delay(1000); // Halt if initialization fails
        }
    }
    
    Serial.println("\n=== ESPRazorBlade Phase 1: WiFi Only ===");
    Serial.println("Waiting for WiFi connection...");
    Serial.println("(This may take a few seconds)\n");
}

void loop() {
    // Your main application code goes here
    // The library handles WiFi connection in background RTOS task
    
    // Print connection status periodically
    static unsigned long lastStatusPrint = 0;
    if (millis() - lastStatusPrint > 5000) { // Every 5 seconds
        Serial.println("\n--- Status Check ---");
        Serial.print("WiFi: ");
        Serial.println(razorBlade.isWiFiConnected() ? "Connected" : "Disconnected");
        
        if (razorBlade.isWiFiConnected()) {
            Serial.print("IP Address: ");
            Serial.println(razorBlade.getIPAddress());
            Serial.print("RSSI: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
        } else {
            Serial.println("(Reconnection will be attempted automatically)");
        }
        
        Serial.println("-------------------\n");
        lastStatusPrint = millis();
    }
    
    // Small delay to prevent tight loop
    delay(100);
}
