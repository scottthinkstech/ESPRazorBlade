#ifndef ESPRAZORBLADE_H
#define ESPRAZORBLADE_H

#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <Arduino.h>
#include "Configuration.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Forward declarations
class ESPRazorBlade;

/**
 * ESPRazorBlade Library - Phase 3: WiFi + MQTT + Publish
 * 
 * Lightweight library for ESP32 devices.
 * Phase 3 Features:
 * - Resilient WiFi connection with automatic reconnection
 * - MQTT broker connection with automatic reconnection
 * - MQTT publish functionality with thread-safe operations
 * - RTOS-based non-blocking operation
 */
class ESPRazorBlade {
public:
    /**
     * Constructor
     */
    ESPRazorBlade();
    
    /**
     * Destructor
     */
    ~ESPRazorBlade();
    
    /**
     * Initialize the library
     * Starts WiFi and MQTT connection tasks
     * @return true if initialization successful
     */
    bool begin();
    
    /**
     * Check if WiFi is connected
     * @return true if connected
     */
    bool isWiFiConnected();
    
    /**
     * Check if MQTT is connected
     * @return true if connected
     */
    bool isMQTTConnected();
    
    /**
     * Get current WiFi IP address
     * @return IP address as String
     */
    String getIPAddress();
    
    /**
     * Publish a message to an MQTT topic
     * @param topic MQTT topic
     * @param payload Message payload (string)
     * @param retained Whether to retain the message (default: false)
     * @return true if publish successful
     */
    bool publish(const char* topic, const char* payload, bool retained = false);
    
    /**
     * Publish a numeric value to an MQTT topic
     * @param topic MQTT topic
     * @param value Numeric value
     * @param retained Whether to retain the message (default: false)
     * @return true if publish successful
     */
    bool publish(const char* topic, float value, bool retained = false);
    bool publish(const char* topic, int value, bool retained = false);
    bool publish(const char* topic, long value, bool retained = false);

private:
    // WiFi client
    WiFiClient wifiClient;
    
    // MQTT client
    MqttClient mqttClient;
    
    // RTOS task handles
    TaskHandle_t wifiTaskHandle;
    TaskHandle_t mqttTaskHandle;
    
    // Synchronization primitives
    SemaphoreHandle_t mqttMutex;
    
    // Connection state
    bool wifiConnected;
    bool mqttConnected;
    bool mqttConnecting;  // Flag to prevent overlapping connection attempts
    unsigned long wifiConnectedTime;  // Timestamp when WiFi first connected
    bool firstMQTTAttempt;  // Flag to track first MQTT connection attempt (for silent retry)
    
    // Static task functions (RTOS entry points)
    static void wifiTask(void* parameter);
    static void mqttTask(void* parameter);
    
    // Internal helper functions
    void connectWiFi();
    void connectMQTT();
};

#endif // ESPRAZORBLADE_H
