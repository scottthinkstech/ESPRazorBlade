#ifndef ESPRAZORBLADE_H
#define ESPRAZORBLADE_H

#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <Arduino.h>
#include "Configuration.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Forward declarations
class ESPRazorBlade;

/**
 * ESPRazorBlade Library - Phase 2: WiFi + MQTT Connection
 * 
 * Lightweight library for ESP32 devices.
 * Phase 2 Features:
 * - Resilient WiFi connection with automatic reconnection
 * - MQTT broker connection with automatic reconnection
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

private:
    // WiFi client
    WiFiClient wifiClient;
    
    // MQTT client
    MqttClient mqttClient;
    
    // RTOS task handles
    TaskHandle_t wifiTaskHandle;
    TaskHandle_t mqttTaskHandle;
    
    // Connection state
    bool wifiConnected;
    bool mqttConnected;
    
    // Static task functions (RTOS entry points)
    static void wifiTask(void* parameter);
    static void mqttTask(void* parameter);
    
    // Internal helper functions
    void connectWiFi();
    void connectMQTT();
};

#endif // ESPRAZORBLADE_H
