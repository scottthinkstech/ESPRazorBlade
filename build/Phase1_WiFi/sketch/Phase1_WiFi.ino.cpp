#include <Arduino.h>
#line 1 "/home/thinks/src/ESPRazorBlade/examples/Phase1_WiFi/Phase1_WiFi.ino"
/*
 * ESPRazorBlade Phase 1 Test: WiFi Connection
 * 
 * This sketch tests the WiFi-only functionality of ESPRazorBlade.
 * 
 * Test Checklist:
 * - [ ] Code compiles without errors
 * - [ ] Uploads to ESP32-C6 successfully
 * - [ ] WiFi connects successfully
 * - [ ] IP address displayed in Serial Monitor
 * - [ ] WiFi reconnection works after disconnect
 * 
 * Make sure to configure your WiFi settings in Configuration.h
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

#line 21 "/home/thinks/src/ESPRazorBlade/examples/Phase1_WiFi/Phase1_WiFi.ino"
void setup();
#line 36 "/home/thinks/src/ESPRazorBlade/examples/Phase1_WiFi/Phase1_WiFi.ino"
void loop();
#line 21 "/home/thinks/src/ESPRazorBlade/examples/Phase1_WiFi/Phase1_WiFi.ino"
void setup() {
    // Initialize the library
    // This starts WiFi connection task automatically
    if (!razorBlade.begin()) {
        Serial.println("Failed to initialize ESPRazorBlade!");
        while (1) {
            delay(1000); // Halt if initialization fails
        }
    }
    
    Serial.println("\n=== Phase 1 WiFi Test ===");
    Serial.println("Waiting for WiFi connection...");
    Serial.println("(This may take a few seconds)\n");
}

void loop() {
    // Check WiFi connection status periodically
    static unsigned long lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 5000) { // Every 5 seconds
        Serial.println("--- Status Check ---");
        
        if (razorBlade.isWiFiConnected()) {
            Serial.println("WiFi: Connected");
            Serial.print("IP Address: ");
            Serial.println(razorBlade.getIPAddress());
            Serial.println("RSSI: " + String(WiFi.RSSI()) + " dBm");
        } else {
            Serial.println("WiFi: Disconnected");
            Serial.println("(Reconnection will be attempted automatically)");
        }
        
        Serial.println("-------------------\n");
        lastStatusCheck = millis();
    }
    
    // Small delay to prevent tight loop
    delay(100);
}

