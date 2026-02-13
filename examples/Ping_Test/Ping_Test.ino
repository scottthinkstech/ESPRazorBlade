/*
 * ESPRazorBlade Ping Test
 *
 * This is a minimal hardware verification sketch for ESP32 boards.
 * Use this to verify that your hardware, IDE setup, and serial communication
 * are working correctly before trying more complex examples.
 *
 * Purpose:
 * - Test ESP32 board is functional
 * - Verify Arduino IDE can compile and upload
 * - Confirm Serial Monitor communication works
 * - Quick troubleshooting tool for hardware issues
 *
 * This sketch does NOT:
 * - Connect to WiFi
 * - Connect to MQTT
 * - Use the ESPRazorBlade library
 * - Require Configuration.h
 *
 * Expected behavior:
 * - Serial Monitor (115200 baud) shows "ping" every second
 * - If you see "ping" messages, your hardware and IDE setup are working!
 *
 * Troubleshooting:
 * - No output: Check Serial Monitor baud rate is 115200
 * - Garbled text: Wrong baud rate selected
 * - Upload fails: Check USB cable, board selection, and port selection
 * - Board not detected: Driver issue or incompatible USB cable
 */

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println();
    Serial.println("===========================================");
    Serial.println("   ESPRazorBlade - Hardware Ping Test");
    Serial.println("===========================================");
    Serial.println();
    Serial.println("If you see this message, your ESP32 board");
    Serial.println("and Serial Monitor are working correctly!");
    Serial.println();
    Serial.println("You should see 'ping' messages below:");
    Serial.println("-------------------------------------------");
}

void loop() {
    Serial.println("ping");
    delay(1000);
}
