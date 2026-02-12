#include "ESPRazorBlade.h"
#include "esp_system.h"

#ifndef DEVICE_ID
#define DEVICE_ID "ESPRazorBlade"
#endif
#ifndef WIFI_SIGNAL_INTERVAL_MS
#define WIFI_SIGNAL_INTERVAL_MS 30000
#endif
#ifndef TIME_ALIVE_INTERVAL_MS
#define TIME_ALIVE_INTERVAL_MS 60000
#endif
#ifndef FREE_HEAP_INTERVAL_MS
#define FREE_HEAP_INTERVAL_MS 30000
#endif

// Built-in telemetry callback helpers (static, used by registerTelemetry)
static String readWiFiRSSI() {
    return String(WiFi.RSSI());
}

static String readFreeHeap() {
    return String(ESP.getFreeHeap());
}

static String readTimeAlive() {
    unsigned long totalSec = millis() / 1000UL;
    unsigned int hours = (unsigned int)(totalSec / 3600UL);
    unsigned int minutes = (unsigned int)((totalSec % 3600UL) / 60UL);
    unsigned int seconds = (unsigned int)(totalSec % 60UL);
    char buf[11];
    snprintf(buf, sizeof(buf), "%03uh%02um%02us", hours, minutes, seconds);
    return String(buf);
}

static const char* getResetReasonString() {
    switch (esp_reset_reason()) {
        case ESP_RST_UNKNOWN:   return "Unknown";
        case ESP_RST_POWERON:   return "PowerOn";
        case ESP_RST_EXT:       return "ExtPin";
        case ESP_RST_SW:        return "Reboot";
        case ESP_RST_PANIC:     return "Crash";
        case ESP_RST_INT_WDT:   return "WDT_Int";
        case ESP_RST_TASK_WDT:  return "WDT_Task";
        case ESP_RST_WDT:       return "WDT_Other";
        case ESP_RST_DEEPSLEEP: return "Sleep";
        case ESP_RST_BROWNOUT:  return "BrownOut";
        case ESP_RST_SDIO:      return "SDIO";
        default:                return "Unknown";
    }
}

// WiFi connection settings
const int WIFI_MAX_RETRIES = 20;
const int WIFI_RETRY_DELAY_MS = 500;
const int WIFI_CHECK_INTERVAL_MS = 5000;

// MQTT connection settings
const int MQTT_MAX_RETRIES = 10;
const int MQTT_RETRY_DELAY_MS = 2000;
const int MQTT_POLL_INTERVAL_MS = 100;
const int MQTT_INITIAL_DELAY_MS = 3000; // Wait 3 seconds after WiFi connects before first MQTT attempt

// Task stack sizes (in words, 4 bytes each on ESP32)
const int WIFI_TASK_STACK_SIZE = 4096;
const int MQTT_TASK_STACK_SIZE = 4096;

// Task priorities
const int WIFI_TASK_PRIORITY = 1;
const int MQTT_TASK_PRIORITY = 2;

ESPRazorBlade::ESPRazorBlade() 
    : mqttClient(&wifiClient),
      wifiTaskHandle(nullptr),
      mqttTaskHandle(nullptr),
      mqttMutex(nullptr),
      wifiConnected(false),
      mqttConnected(false),
      mqttConnecting(false),
      wifiConnectedTime(0),
      firstMQTTAttempt(true),
      telemetryCallbackCount(0),
      resetReasonPublished(false) {
    // Initialize telemetry callback array
    for (int i = 0; i < MAX_TELEMETRY_CALLBACKS; i++) {
        telemetryCallbacks[i].active = false;
        telemetryCallbacks[i].topic[0] = '\0';
        telemetryCallbacks[i].callback = nullptr;
        telemetryCallbacks[i].intervalMs = 0;
        telemetryCallbacks[i].lastExecution = 0;
    }
}

ESPRazorBlade::~ESPRazorBlade() {
    // Cleanup tasks if they exist
    if (wifiTaskHandle != nullptr) {
        vTaskDelete(wifiTaskHandle);
    }
    if (mqttTaskHandle != nullptr) {
        vTaskDelete(mqttTaskHandle);
    }
    if (mqttMutex != nullptr) {
        vSemaphoreDelete(mqttMutex);
    }
}

bool ESPRazorBlade::begin() {
    Serial.begin(115200);
    delay(100); // Give Serial time to initialize
    
    Serial.println("\n=== ESPRazorBlade Library - Phase 5: WiFi + MQTT + Publish + Telemetry ===");
    
    // Create mutex for thread-safe MQTT operations
    mqttMutex = xSemaphoreCreateMutex();
    if (mqttMutex == nullptr) {
        Serial.println("ERROR: Failed to create MQTT mutex");
        return false;
    }
    
    // MQTT client is already initialized with wifiClient in constructor
    // No begin() method needed - we'll use connect() when WiFi is ready
    
    // Create WiFi management task
    // ESP32-C3 is single-core, so pin to core 0
    xTaskCreatePinnedToCore(
        wifiTask,
        "WiFiTask",
        WIFI_TASK_STACK_SIZE,
        this,
        WIFI_TASK_PRIORITY,
        &wifiTaskHandle,
        0  // Pin to core 0 (ESP32-C3 only has one core)
    );
    
    if (wifiTaskHandle == nullptr) {
        Serial.println("ERROR: Failed to create WiFi task");
        return false;
    }
    
    // Create MQTT management task
    xTaskCreatePinnedToCore(
        mqttTask,
        "MQTTTask",
        MQTT_TASK_STACK_SIZE,
        this,
        MQTT_TASK_PRIORITY,
        &mqttTaskHandle,
        0  // Pin to core 0 (ESP32-C3 only has one core)
    );
    
    if (mqttTaskHandle == nullptr) {
        Serial.println("ERROR: Failed to create MQTT task");
        return false;
    }
    
    // Register built-in telemetry (WiFi RSSI, time alive, free heap)
    registerTelemetry(DEVICE_ID "/telemetry/wifi_rssi", readWiFiRSSI, WIFI_SIGNAL_INTERVAL_MS);
    registerTelemetry(DEVICE_ID "/telemetry/time_alive", readTimeAlive, TIME_ALIVE_INTERVAL_MS);
    registerTelemetry(DEVICE_ID "/telemetry/free_heap", readFreeHeap, FREE_HEAP_INTERVAL_MS);

    Serial.println("ESPRazorBlade initialized successfully");
    Serial.println("WiFi and MQTT connection tasks started");
    return true;
}

void ESPRazorBlade::wifiTask(void* parameter) {
    ESPRazorBlade* instance = static_cast<ESPRazorBlade*>(parameter);
    
    Serial.println("WiFi task started");
    
    while (true) {
        // Check WiFi connection status
        if (WiFi.status() != WL_CONNECTED) {
            instance->wifiConnected = false;
            instance->connectWiFi();
        } else {
            instance->wifiConnected = true;
        }
        
        // Wait before next check
        vTaskDelay(pdMS_TO_TICKS(WIFI_CHECK_INTERVAL_MS));
    }
}

void ESPRazorBlade::connectWiFi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < WIFI_MAX_RETRIES) {
        vTaskDelay(pdMS_TO_TICKS(WIFI_RETRY_DELAY_MS));
        Serial.print(".");
        retryCount++;
    }
    
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        // Track when WiFi first connects (for MQTT initial delay)
        if (!wifiConnected) {
            wifiConnectedTime = millis();
        }
        wifiConnected = true;
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        wifiConnected = false;
        wifiConnectedTime = 0; // Reset on disconnect
        Serial.println("WiFi connection failed!");
        Serial.print("Status: ");
        Serial.println(WiFi.status());
    }
}

bool ESPRazorBlade::isWiFiConnected() {
    return wifiConnected && (WiFi.status() == WL_CONNECTED);
}

void ESPRazorBlade::mqttTask(void* parameter) {
    ESPRazorBlade* instance = static_cast<ESPRazorBlade*>(parameter);
    
    Serial.println("MQTT task started");
    
    while (true) {
        // Only process MQTT if WiFi is connected
        if (instance->wifiConnected) {
            // Check MQTT connection
            if (!instance->mqttClient.connected()) {
                instance->mqttConnected = false;
                // Only attempt connection if we're not already trying
                // Wait 2 seconds after WiFi connects before first MQTT attempt
                if (!instance->mqttConnecting) {
                    unsigned long now = millis();
                    if (instance->wifiConnectedTime == 0 || 
                        (now - instance->wifiConnectedTime) >= MQTT_INITIAL_DELAY_MS) {
                        instance->connectMQTT();
                    }
                }
            } else {
                instance->mqttConnected = true;
                instance->mqttConnecting = false; // Clear connecting flag when connected
                
                // Poll MQTT to maintain connection and process messages
                instance->mqttClient.poll();
                
                // Process telemetry callbacks
                instance->processTelemetry();
            }
        } else {
            instance->mqttConnected = false;
            instance->mqttConnecting = false; // Clear connecting flag when WiFi disconnects
            instance->wifiConnectedTime = 0; // Reset WiFi connection time
            instance->firstMQTTAttempt = true; // Reset for next WiFi connection
        }
        
        // Small delay to prevent tight loop
        vTaskDelay(pdMS_TO_TICKS(MQTT_POLL_INTERVAL_MS));
    }
}

void ESPRazorBlade::connectMQTT() {
    // Set connecting flag to prevent overlapping attempts
    mqttConnecting = true;
    
    bool isFirstAttempt = firstMQTTAttempt;
    firstMQTTAttempt = false; // Mark that we've attempted connection
    
    Serial.print("Connecting to MQTT broker: ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
    
    // Set client ID
    mqttClient.setId(MQTT_CLIENT_ID);
    
    // Set username/password if provided
    #ifdef MQTT_USERNAME
        mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASSWORD);
    #endif
    
    int retryCount = 0;
    int result = 0;
    bool connected = false;
    
    while (retryCount < MQTT_MAX_RETRIES) {
        // Check if we're already connected (might have connected from another attempt)
        if (mqttClient.connected()) {
            mqttConnected = true;
            connected = true;
            Serial.println("MQTT connected!");
            mqttConnecting = false;
            return;
        }
        
        // Attempt connection (connect() takes host and port)
        result = mqttClient.connect(MQTT_BROKER, MQTT_PORT);
        
        if (result == 0) {
            // Verify connection is actually established
            if (mqttClient.connected()) {
                mqttConnected = true;
                connected = true;
                Serial.println("MQTT connected!");
                mqttConnecting = false;
                return;
            }
        }
        
        // Connection failed or not yet established
        // On first attempt, try once silently without error messages
        // Only show errors if first attempt fails
        if (!isFirstAttempt || retryCount > 0) {
            if (retryCount == (isFirstAttempt ? 1 : 0)) {
                // Print failure message only after first silent attempt fails
                Serial.print("MQTT connection failed (rc=");
                Serial.print(result);
                Serial.println("), retrying...");
            }
        }
        retryCount++;
        vTaskDelay(pdMS_TO_TICKS(MQTT_RETRY_DELAY_MS));
    }
    
    // After retry loop, check one final time if we're connected
    // (connection might have succeeded during the last delay)
    if (mqttClient.connected()) {
        mqttConnected = true;
        connected = true;
        Serial.println("MQTT connected!");
    } else if (!connected) {
        // Only print failure if we're definitely not connected
        // Suppress failure message on first attempt to avoid confusing novice users
        if (!isFirstAttempt) {
            Serial.print("MQTT connection failed after ");
            Serial.print(MQTT_MAX_RETRIES);
            Serial.println(" attempts");
        }
    }
    
    mqttConnecting = false;
}

bool ESPRazorBlade::isMQTTConnected() {
    return mqttConnected && mqttClient.connected();
}

bool ESPRazorBlade::publish(const char* topic, const char* payload, bool retained) {
    if (!mqttConnected || !mqttClient.connected()) {
        return false;
    }
    
    bool result = false;
    if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        mqttClient.beginMessage(topic, retained);
        mqttClient.print(payload);
        mqttClient.endMessage();
        result = true;
        xSemaphoreGive(mqttMutex);
    }
    
    return result;
}

bool ESPRazorBlade::publish(const char* topic, float value, bool retained) {
    if (!mqttConnected || !mqttClient.connected()) {
        return false;
    }
    
    bool result = false;
    if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        mqttClient.beginMessage(topic, retained);
        mqttClient.print(value);
        mqttClient.endMessage();
        result = true;
        xSemaphoreGive(mqttMutex);
    }
    
    return result;
}

bool ESPRazorBlade::publish(const char* topic, int value, bool retained) {
    if (!mqttConnected || !mqttClient.connected()) {
        return false;
    }
    
    bool result = false;
    if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        mqttClient.beginMessage(topic, retained);
        mqttClient.print(value);
        mqttClient.endMessage();
        result = true;
        xSemaphoreGive(mqttMutex);
    }
    
    return result;
}

bool ESPRazorBlade::publish(const char* topic, long value, bool retained) {
    if (!mqttConnected || !mqttClient.connected()) {
        return false;
    }
    
    bool result = false;
    if (xSemaphoreTake(mqttMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        mqttClient.beginMessage(topic, retained);
        mqttClient.print(value);
        mqttClient.endMessage();
        result = true;
        xSemaphoreGive(mqttMutex);
    }
    
    return result;
}

bool ESPRazorBlade::registerTelemetry(const char* topic, TelemetryCallback callback, unsigned long intervalMs) {
    // Check if we've reached the maximum number of callbacks
    if (telemetryCallbackCount >= MAX_TELEMETRY_CALLBACKS) {
        Serial.print("ERROR: Maximum number of telemetry callbacks (");
        Serial.print(MAX_TELEMETRY_CALLBACKS);
        Serial.println(") reached");
        return false;
    }
    
    // Validate inputs
    if (topic == nullptr || callback == nullptr || intervalMs == 0) {
        Serial.println("ERROR: Invalid telemetry registration parameters");
        return false;
    }
    
    // Find an available slot
    int slot = -1;
    for (int i = 0; i < MAX_TELEMETRY_CALLBACKS; i++) {
        if (!telemetryCallbacks[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        Serial.println("ERROR: No available slot for telemetry callback");
        return false;
    }
    
    // Copy topic (ensure it fits)
    int topicLen = strlen(topic);
    if (topicLen >= 64) {
        Serial.println("ERROR: Topic name too long (max 63 characters)");
        return false;
    }
    
    strncpy(telemetryCallbacks[slot].topic, topic, 63);
    telemetryCallbacks[slot].topic[63] = '\0';
    telemetryCallbacks[slot].callback = callback;
    telemetryCallbacks[slot].intervalMs = intervalMs;
    telemetryCallbacks[slot].lastExecution = 0; // Will execute on next check
    telemetryCallbacks[slot].active = true;
    
    telemetryCallbackCount++;
    
    Serial.print("Registered telemetry: ");
    Serial.print(topic);
    Serial.print(" (interval: ");
    Serial.print(intervalMs);
    Serial.println("ms)");
    
    return true;
}

void ESPRazorBlade::processTelemetry() {
    if (!mqttConnected || !mqttClient.connected()) {
        return; // Don't process telemetry if MQTT is not connected
    }

    // One-time publish of status and reset reason when MQTT first connects
    publishBootTelemetry();
    
    unsigned long now = millis();
    
    // Process each active telemetry callback
    for (int i = 0; i < MAX_TELEMETRY_CALLBACKS; i++) {
        if (!telemetryCallbacks[i].active || telemetryCallbacks[i].callback == nullptr) {
            continue;
        }
        
        // Check if it's time to execute this callback
        unsigned long timeSinceLastExecution = now - telemetryCallbacks[i].lastExecution;
        
        // Handle millis() overflow (wraps around after ~49 days)
        if (telemetryCallbacks[i].lastExecution == 0 || timeSinceLastExecution >= telemetryCallbacks[i].intervalMs) {
            // Execute callback and publish result
            String value = telemetryCallbacks[i].callback();
            
            bool ok = publish(telemetryCallbacks[i].topic, value.c_str());
            if (ok) {
                telemetryCallbacks[i].lastExecution = now;
            }
            Serial.print("Telemetry published: ");
            Serial.print(telemetryCallbacks[i].topic);
            Serial.print(" = ");
            Serial.print(value);
            Serial.println(ok ? "" : " [FAILED]");
        }
    }
}

void ESPRazorBlade::publishBootTelemetry() {
    if (resetReasonPublished) {
        return;
    }
    bool okStatus = publish(DEVICE_ID "/status", "online", true);
    bool okReset = publish(DEVICE_ID "/telemetry/reset_reason", getResetReasonString(), true);
    if (okStatus) {
        resetReasonPublished = true;
    }
    Serial.print("Boot telemetry published: status=");
    Serial.print(okStatus ? "OK" : "FAILED");
    Serial.print(", reset_reason=");
    Serial.println(okReset ? "OK" : "FAILED");
}

String ESPRazorBlade::getIPAddress() {
    if (wifiConnected) {
        return WiFi.localIP().toString();
    }
    return String("");
}
