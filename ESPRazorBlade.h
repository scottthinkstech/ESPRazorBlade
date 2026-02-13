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
 * @brief ESPRazorBlade Library - Lightweight MQTT telemetry for ESP32 devices
 * 
 * Provides resilient WiFi and MQTT connectivity with automatic telemetry publishing.
 * 
 * Features:
 * - Resilient WiFi connection with automatic reconnection
 * - MQTT broker connection with automatic reconnection
 * - MQTT publish functionality with thread-safe operations
 * - Built-in system telemetry (WiFi RSSI, uptime, free heap)
 * - Custom telemetry callback system for sensor data
 * - Runtime configuration updates via MQTT
 * - RTOS-based non-blocking operation
 */
class ESPRazorBlade {
public:
    /**
     * @brief Constructor
     */
    ESPRazorBlade();
    
    /**
     * @brief Destructor
     */
    ~ESPRazorBlade();
    
    /**
     * @brief Initialize the library
     * 
     * Starts WiFi and MQTT connection tasks and registers built-in telemetry metrics.
     * Built-in metrics include WiFi RSSI, device uptime, free heap memory, reset reason,
     * and device online status.
     * 
     * @return true if initialization successful
     */
    bool begin();
    
    /**
     * @brief Check if WiFi is connected
     * @return true if connected, false otherwise
     */
    bool isWiFiConnected();
    
    /**
     * @brief Check if MQTT is connected
     * @return true if connected, false otherwise
     */
    bool isMQTTConnected();
    
    /**
     * @brief Get current WiFi IP address
     * @return IP address as String, or empty string if not connected
     */
    String getIPAddress();
    
    /**
     * @brief Publish a message to an MQTT topic
     * @param topic MQTT topic path
     * @param payload Message payload (string)
     * @param retained Whether to retain the message on the broker (default: false)
     * @return true if publish successful, false otherwise
     */
    bool publish(const char* topic, const char* payload, bool retained = false);
    
    /**
     * @brief Publish a float value to an MQTT topic
     * @param topic MQTT topic path
     * @param value Float value to publish
     * @param retained Whether to retain the message on the broker (default: false)
     * @return true if publish successful, false otherwise
     */
    bool publish(const char* topic, float value, bool retained = false);
    
    /**
     * @brief Publish an integer value to an MQTT topic
     * @param topic MQTT topic path
     * @param value Integer value to publish
     * @param retained Whether to retain the message on the broker (default: false)
     * @return true if publish successful, false otherwise
     */
    bool publish(const char* topic, int value, bool retained = false);
    
    /**
     * @brief Publish a long integer value to an MQTT topic
     * @param topic MQTT topic path
     * @param value Long integer value to publish
     * @param retained Whether to retain the message on the broker (default: false)
     * @return true if publish successful, false otherwise
     */
    bool publish(const char* topic, long value, bool retained = false);
    
    /**
     * @brief Register a custom telemetry callback function
     * 
     * The callback will be executed at the specified interval and its return value
     * will be published to the given MQTT topic. This is for custom metrics only;
     * built-in system metrics (WiFi RSSI, uptime, heap) are registered automatically.
     * 
     * @param topic MQTT topic to publish to
     * @param callback Function that returns a String to publish
     * @param intervalMs Interval in milliseconds between executions
     * @return true if registration successful, false if max callbacks reached (limit: 10 total)
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
