#line 1 "/home/thinks/src/ESPRazorBlade/ESPRazorBlade.cpp"
#include "ESPRazorBlade.h"

// WiFi connection settings
const int WIFI_MAX_RETRIES = 20;
const int WIFI_RETRY_DELAY_MS = 500;
const int WIFI_CHECK_INTERVAL_MS = 5000;

// MQTT connection settings
const int MQTT_MAX_RETRIES = 10;
const int MQTT_RETRY_DELAY_MS = 2000;
const int MQTT_POLL_INTERVAL_MS = 100;

// Task stack sizes (in words, 4 bytes each on ESP32)
const int WIFI_TASK_STACK_SIZE = 4096;
const int MQTT_TASK_STACK_SIZE = 4096;

// Task priorities
const int WIFI_TASK_PRIORITY = 1;
const int MQTT_TASK_PRIORITY = 2;

ESPRazorBlade::ESPRazorBlade() 
    : mqttClient(wifiClient),
      wifiTaskHandle(nullptr),
      mqttTaskHandle(nullptr),
      wifiConnected(false),
      mqttConnected(false) {
}

ESPRazorBlade::~ESPRazorBlade() {
    // Cleanup tasks if they exist
    if (wifiTaskHandle != nullptr) {
        vTaskDelete(wifiTaskHandle);
    }
    if (mqttTaskHandle != nullptr) {
        vTaskDelete(mqttTaskHandle);
    }
}

bool ESPRazorBlade::begin() {
    Serial.begin(115200);
    delay(100); // Give Serial time to initialize
    
    Serial.println("\n=== ESPRazorBlade Library - Phase 2: WiFi + MQTT ===");
    
    // Configure MQTT client
    mqttClient.begin(MQTT_BROKER, MQTT_PORT, wifiClient);
    
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
        wifiConnected = true;
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        wifiConnected = false;
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
                instance->connectMQTT();
            } else {
                instance->mqttConnected = true;
                
                // Poll MQTT to maintain connection and process messages
                instance->mqttClient.poll();
            }
        } else {
            instance->mqttConnected = false;
        }
        
        // Small delay to prevent tight loop
        vTaskDelay(pdMS_TO_TICKS(MQTT_POLL_INTERVAL_MS));
    }
}

void ESPRazorBlade::connectMQTT() {
    Serial.print("Connecting to MQTT broker: ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
    
    int retryCount = 0;
    int result = 0;
    
    while (retryCount < MQTT_MAX_RETRIES) {
        // Attempt connection with optional credentials
        #ifdef MQTT_USERNAME
            result = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
        #else
            result = mqttClient.connect(MQTT_CLIENT_ID);
        #endif
        
        if (result == 0) {
            mqttConnected = true;
            Serial.println("MQTT connected!");
            break;
        } else {
            Serial.print("MQTT connection failed, rc=");
            Serial.print(result);
            Serial.print(" - retrying in ");
            Serial.print(MQTT_RETRY_DELAY_MS / 1000);
            Serial.println(" seconds...");
            retryCount++;
            vTaskDelay(pdMS_TO_TICKS(MQTT_RETRY_DELAY_MS));
        }
    }
    
    if (mqttConnected == false) {
        Serial.println("MQTT connection failed after retries");
    }
}

bool ESPRazorBlade::isMQTTConnected() {
    return mqttConnected && mqttClient.connected();
}

String ESPRazorBlade::getIPAddress() {
    if (wifiConnected) {
        return WiFi.localIP().toString();
    }
    return String("");
}
