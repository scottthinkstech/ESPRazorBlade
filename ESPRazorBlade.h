#ifndef ESPRAZORBLADE_H
#define ESPRAZORBLADE_H

#include <WiFi.h>
#include <Arduino.h>
#include "Configuration.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Forward declarations
class ESPRazorBlade;

/**
 * ESPRazorBlade Library - Phase 1: WiFi Only
 * 
 * Lightweight library for ESP32 devices.
 * Phase 1 Features:
 * - Resilient WiFi connection with automatic reconnection
 * - RTOS-based non-blocking operation
 * 
 * Future phases will add MQTT telemetry functionality.
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
     * Starts WiFi connection task
     * @return true if initialization successful
     */
    bool begin();
    
    /**
     * Check if WiFi is connected
     * @return true if connected
     */
    bool isWiFiConnected();
    
    /**
     * Get current WiFi IP address
     * @return IP address as String
     */
    String getIPAddress();

private:
    // RTOS task handles
    TaskHandle_t wifiTaskHandle;
    
    // Connection state
    bool wifiConnected;
    
    // Static task functions (RTOS entry points)
    static void wifiTask(void* parameter);
    
    // Internal helper functions
    void connectWiFi();
};

#endif // ESPRAZORBLADE_H
