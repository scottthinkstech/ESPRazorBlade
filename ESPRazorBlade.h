#ifndef ESPRAZORBLADE_H
#define ESPRAZORBLADE_H

#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <Arduino.h>
// Use quotes so the compiler searches the sketch directory first.
// This ensures each sketch uses its own Configuration.h.
#include "Configuration.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Forward declarations
class ESPRazorBlade;

// Telemetry callback function type
// Returns a String that will be published to the topic
typedef String (*TelemetryCallback)();

/**
 * ESPRazorBlade Library - Phase 5: WiFi + MQTT + Publish + Telemetry
 * 
 * Lightweight library for ESP32 devices.
 * Phase 5 Features:
 * - Resilient WiFi connection with automatic reconnection
 * - MQTT broker connection with automatic reconnection
 * - MQTT publish functionality with thread-safe operations
 * - Telemetry callback system for automatic data publishing
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
    
    /**
     * Register a telemetry callback function
     * The callback will be executed at the specified interval and its return value
     * will be published to the given MQTT topic.
     * 
     * @param topic MQTT topic to publish to
     * @param callback Function that returns a String to publish
     * @param intervalMs Interval in milliseconds between executions
     * @return true if registration successful (false if max callbacks reached)
     */
    bool registerTelemetry(const char* topic, TelemetryCallback callback, unsigned long intervalMs);

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
    bool resetReasonPublished;  // Flag for one-time reset reason publish on boot
    bool configTimeoutsPublished;  // Flag for one-time config timeout publish on MQTT connect
    bool configTopicsSubscribed;  // Flag to track if config topics have been subscribed
    
    // Telemetry callback structure
    struct TelemetryEntry {
        char topic[64];              // MQTT topic (max 63 chars + null terminator)
        TelemetryCallback callback;   // Callback function
        unsigned long intervalMs;     // Interval between executions
        unsigned long lastExecution;   // Last execution time
        bool active;                  // Whether this entry is active
    };
    
    static const int MAX_TELEMETRY_CALLBACKS = 10;
    TelemetryEntry telemetryCallbacks[MAX_TELEMETRY_CALLBACKS];
    int telemetryCallbackCount;
    
    // Static instance pointer for callback access
    static ESPRazorBlade* instance;
    
    // Static task functions (RTOS entry points)
    static void wifiTask(void* parameter);
    static void mqttTask(void* parameter);
    
    // Static MQTT callback (MQTT message handler)
    static void onMQTTMessage(int messageSize);
    
    // Internal helper functions
    void connectWiFi();
    void connectMQTT();
    void processTelemetry();  // Process registered telemetry callbacks
    void publishBootTelemetry();  // One-time status and reset reason on MQTT connect
    void publishConfigurationTimeouts();  // One-time config timeout publish on MQTT connect
    void handleConfigUpdate(const char* topic, String payload);  // Handle config topic updates
    void subscribeToConfigTopics();  // Subscribe to configuration topics
};

#endif // ESPRAZORBLADE_H
