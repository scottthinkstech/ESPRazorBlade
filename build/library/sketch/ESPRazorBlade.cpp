#line 1 "/tmp/tmp.1GfJhZNuoc/ESPRazorBlade.cpp"
#include "ESPRazorBlade.h"

// WiFi connection settings
const int WIFI_MAX_RETRIES = 20;
const int WIFI_RETRY_DELAY_MS = 500;
const int WIFI_CHECK_INTERVAL_MS = 5000;

// Task stack sizes (in words, 4 bytes each on ESP32)
const int WIFI_TASK_STACK_SIZE = 4096;

// Task priorities
const int WIFI_TASK_PRIORITY = 1;

ESPRazorBlade::ESPRazorBlade() 
    : wifiTaskHandle(nullptr),
      wifiConnected(false) {
}

ESPRazorBlade::~ESPRazorBlade() {
    // Cleanup task if it exists
    if (wifiTaskHandle != nullptr) {
        vTaskDelete(wifiTaskHandle);
    }
}

bool ESPRazorBlade::begin() {
    Serial.begin(115200);
    delay(100); // Give Serial time to initialize
    
    Serial.println("\n=== ESPRazorBlade Library - Phase 1: WiFi Only ===");
    
    // Create WiFi management task
    xTaskCreatePinnedToCore(
        wifiTask,
        "WiFiTask",
        WIFI_TASK_STACK_SIZE,
        this,
        WIFI_TASK_PRIORITY,
        &wifiTaskHandle,
        1  // Pin to core 1
    );
    
    if (wifiTaskHandle == nullptr) {
        Serial.println("ERROR: Failed to create WiFi task");
        return false;
    }
    
    Serial.println("ESPRazorBlade initialized successfully");
    Serial.println("WiFi connection task started");
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

String ESPRazorBlade::getIPAddress() {
    if (wifiConnected) {
        return WiFi.localIP().toString();
    }
    return String("");
}
