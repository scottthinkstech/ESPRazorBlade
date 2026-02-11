#include <Arduino.h>
#line 1 "/home/thinks/src/ESPRazorBlade/examples/Ping_Test/Ping_Test.ino"
/*
 * Minimal Serial ping example for ESP32-C3.
 * Use to verify: compile, upload, and serial monitor work.
 * No WiFi, MQTT, or Configuration.h required.
 */

#line 7 "/home/thinks/src/ESPRazorBlade/examples/Ping_Test/Ping_Test.ino"
void setup();
#line 13 "/home/thinks/src/ESPRazorBlade/examples/Ping_Test/Ping_Test.ino"
void loop();
#line 7 "/home/thinks/src/ESPRazorBlade/examples/Ping_Test/Ping_Test.ino"
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== Ping Test ===");
}

void loop() {
    Serial.println("ping");
    delay(1000);
}

