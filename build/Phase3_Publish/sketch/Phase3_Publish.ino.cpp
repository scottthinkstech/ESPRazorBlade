#include <Arduino.h>
#line 1 "/home/thinks/src/ESPRazorBlade/examples/Phase3_Publish/Phase3_Publish.ino"
/*
 * ESPRazorBlade Phase 3 Test: MQTT Publish
 * 
 * This sketch tests the MQTT publish functionality of ESPRazorBlade.
 * 
 * Test Checklist:
 * - [ ] Code compiles without errors
 * - [ ] WiFi connects successfully
 * - [ ] MQTT connects successfully
 * - [ ] String publish works
 * - [ ] Integer publish works
 * - [ ] Float publish works
 * - [ ] Messages appear on MQTT broker
 * 
 * Make sure to configure your WiFi and MQTT settings in Configuration.h
 */

#include "ESPRazorBlade.h"

// Create library instance
ESPRazorBlade razorBlade;

#line 23 "/home/thinks/src/ESPRazorBlade/examples/Phase3_Publish/Phase3_Publish.ino"
void setup();
#line 37 "/home/thinks/src/ESPRazorBlade/examples/Phase3_Publish/Phase3_Publish.ino"
void loop();
#line 23 "/home/thinks/src/ESPRazorBlade/examples/Phase3_Publish/Phase3_Publish.ino"
void setup() {
    // Initialize the library
    if (!razorBlade.begin()) {
        Serial.println("Failed to initialize ESPRazorBlade!");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("\n=== Phase 3 MQTT Publish Test ===");
    Serial.println("Waiting for WiFi and MQTT connections...");
    Serial.println("(This may take a few seconds)\n");
}

void loop() {
    // Wait for connections
    if (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
        delay(1000);
        return;
    }
    
    static unsigned long lastPublish = 0;
    if (millis() - lastPublish > 5000) { // Every 5 seconds
        Serial.println("--- Publishing Test Messages ---");
        
        // Test string publish
        if (razorBlade.publish("test/string", "Hello from ESPRazorBlade")) {
            Serial.println("✓ Published string message");
        } else {
            Serial.println("✗ Failed to publish string");
        }
        
        // Test integer publish
        int testInt = millis() / 1000;
        if (razorBlade.publish("test/integer", testInt)) {
            Serial.print("✓ Published integer: ");
            Serial.println(testInt);
        } else {
            Serial.println("✗ Failed to publish integer");
        }
        
        // Test float publish
        float testFloat = 22.5 + (random(0, 100) / 10.0);
        if (razorBlade.publish("test/float", testFloat)) {
            Serial.print("✓ Published float: ");
            Serial.println(testFloat, 1);
        } else {
            Serial.println("✗ Failed to publish float");
        }
        
        Serial.println("-------------------------------\n");
        lastPublish = millis();
    }
    
    delay(100);
}

