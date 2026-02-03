#include <Arduino.h>
#line 1 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
 /*
  * ESPRazorBlade Phase 6 Test: Keepalive and Reconnection Stress Test
  *
  * This sketch performs long-running MQTT tests with rapid publish bursts
  * and telemetry callbacks to validate connection stability.
  *
  * Test Checklist:
  * - [ ] Code compiles without errors
  * - [ ] WiFi connects successfully
  * - [ ] MQTT connects successfully
  * - [ ] Keepalive maintains connection for 30+ minutes
  * - [ ] Rapid publish bursts do not crash or block
  * - [ ] Telemetry callbacks continue after reconnect
  * - [ ] WiFi and broker reconnect works after disruptions
  *
  * Manual stress ideas:
  * - Power cycle your WiFi router while running
  * - Stop and restart your MQTT broker
  * - Let the device run for 30+ minutes
  *
  * Make sure to configure your WiFi and MQTT settings in Configuration.h
  */
 
 #include "ESPRazorBlade.h"
 
 // Create library instance
 ESPRazorBlade razorBlade;
 
 // Telemetry callback functions
#line 30 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
String getUptimeTelemetry();
#line 34 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
String getHeapTelemetry();
#line 38 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
String getRssiTelemetry();
#line 45 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
void waitForConnections();
#line 56 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
void registerStressTelemetry();
#line 77 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
void publishBurst(unsigned long burstId);
#line 96 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
void setup();
#line 113 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
void loop();
#line 30 "/home/thinks/src/ESPRazorBlade/examples/Phase6_StressTest/Phase6_StressTest.ino"
 String getUptimeTelemetry() {
     return String(millis() / 1000);
 }
 
 String getHeapTelemetry() {
     return String(ESP.getFreeHeap());
 }
 
 String getRssiTelemetry() {
     if (razorBlade.isWiFiConnected()) {
         return String(WiFi.RSSI());
     }
     return String("disconnected");
 }
 
 void waitForConnections() {
     unsigned long start = millis();
     while (!razorBlade.isWiFiConnected() || !razorBlade.isMQTTConnected()) {
         if (millis() - start > 60000) {
             Serial.println("Timeout waiting for WiFi/MQTT. Continuing anyway.");
             break;
         }
         delay(500);
     }
 }
 
 void registerStressTelemetry() {
     Serial.println("\n--- Registering Stress Telemetry ---");
     if (razorBlade.registerTelemetry("stress/uptime", getUptimeTelemetry, 10000)) {
         Serial.println("Registered: stress/uptime (10s)");
     } else {
         Serial.println("Failed to register: stress/uptime");
     }
 
     if (razorBlade.registerTelemetry("stress/heap", getHeapTelemetry, 15000)) {
         Serial.println("Registered: stress/heap (15s)");
     } else {
         Serial.println("Failed to register: stress/heap");
     }
 
     if (razorBlade.registerTelemetry("stress/rssi", getRssiTelemetry, 20000)) {
         Serial.println("Registered: stress/rssi (20s)");
     } else {
         Serial.println("Failed to register: stress/rssi");
     }
 }
 
 void publishBurst(unsigned long burstId) {
     Serial.print("Publishing burst ");
     Serial.println(burstId);
 
     for (int i = 0; i < 10; i++) {
         char topic[64];
         snprintf(topic, sizeof(topic), "stress/burst/%lu", burstId);
 
         String payload = String("msg=") + i + ", ms=" + String(millis());
         bool ok = razorBlade.publish(topic, payload.c_str());
 
         if (!ok) {
             Serial.println("Publish failed (MQTT disconnected?)");
             break;
         }
         delay(10);
     }
 }
 
 void setup() {
     if (!razorBlade.begin()) {
         Serial.println("Failed to initialize ESPRazorBlade!");
         while (1) {
             delay(1000);
         }
     }
 
     Serial.println("\n=== Phase 6 Stress Test ===");
     Serial.println("Waiting for WiFi and MQTT connections...");
 
     waitForConnections();
     registerStressTelemetry();
 
     Serial.println("\nStress test running. Monitor broker for stress/* topics.");
 }
 
 void loop() {
     static unsigned long lastStatus = 0;
     static unsigned long lastBurst = 0;
     static unsigned long burstId = 0;
 
     if (millis() - lastStatus > 10000) {
         Serial.println("\n--- Status ---");
         Serial.print("WiFi: ");
         Serial.println(razorBlade.isWiFiConnected() ? "Connected" : "Disconnected");
         Serial.print("MQTT: ");
         Serial.println(razorBlade.isMQTTConnected() ? "Connected" : "Disconnected");
         Serial.print("Uptime (s): ");
         Serial.println(millis() / 1000);
         lastStatus = millis();
     }
 
     if (razorBlade.isWiFiConnected() && razorBlade.isMQTTConnected()) {
         if (millis() - lastBurst > 5000) {
             publishBurst(++burstId);
             lastBurst = millis();
         }
     } else {
         delay(500);
     }
 
     delay(50);
 }

