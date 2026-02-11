/*
 * Minimal Serial ping example for ESP32-C3.
 * Use to verify: compile, upload, and serial monitor work.
 * No WiFi, MQTT, or Configuration.h required.
 */

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== Ping Test ===");
}

void loop() {
    Serial.println("ping");
    delay(1000);
}
