/**
 * @file WiFiManager.cpp
 * @brief WiFi Manager Implementation
 */

#include "WiFiManager.hpp"
#include "../app/App.hpp"
#include <esp_log.h>

static const char* TAG = "WiFi";

// Static member
WiFiManager* WiFiManager::instance = nullptr;

// Connection settings
#define WIFI_CONNECT_TIMEOUT_MS 30000
#define WIFI_RECONNECT_INTERVAL_MS 5000
#define WIFI_MAX_RECONNECT_ATTEMPTS 10

WiFiManager::WiFiManager()
    : state(WiFiState::DISCONNECTED)
    , connectStartTime(0)
    , connectTimeout(WIFI_CONNECT_TIMEOUT_MS)
    , autoReconnect(true)
    , lastDisconnectTime(0)
    , lastDisconnectReason(0)
    , reconnectAttempts(0) {

    instance = this;

    // Register WiFi event handler
    WiFi.onEvent(onWiFiEvent);

    ESP_LOGI(TAG, "Initialized");
}

WiFiManager::~WiFiManager() {
    disconnect();
}

void WiFiManager::update() {
    switch (state) {
        case WiFiState::CONNECTING:
            // Check for timeout
            if (millis() - connectStartTime > connectTimeout) {
                ESP_LOGW(TAG, "Connection timeout");
                WiFi.disconnect();
                state = WiFiState::ERROR;
            }
            break;

        case WiFiState::DISCONNECTED:
        case WiFiState::ERROR:
            // Try to reconnect if auto-reconnect is enabled
            if (autoReconnect && shouldReconnect()) {
                reconnect();
            }
            break;

        case WiFiState::AP_MODE:
        case WiFiState::CONNECTED:
            // Nothing to do
            break;
    }
}

bool WiFiManager::connect(const String& ssidName, const String& pass) {
    ssid = ssidName;
    password = pass;

    ESP_LOGI(TAG, "Connecting to: %s", ssid.c_str());

    // Disconnect if connected
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
        delay(100);
    }

    // Set WiFi mode
    WiFi.mode(WIFI_STA);

    // Start connection
    state = WiFiState::CONNECTING;
    connectStartTime = millis();
    reconnectAttempts = 0;

    WiFi.begin(ssid.c_str(), password.c_str());

    return true;
}

bool WiFiManager::connectKeepAP(const String& ssidName, const String& pass) {
    ssid = ssidName;
    password = pass;

    ESP_LOGI(TAG, "Connecting (AP+STA) to: %s", ssid.c_str());

    // Switch to AP+STA — keeps AP alive so phone stays connected to captive portal
    WiFi.mode(WIFI_AP_STA);

    state = WiFiState::CONNECTING;
    connectStartTime = millis();
    reconnectAttempts = 0;

    WiFi.begin(ssid.c_str(), password.c_str());

    return true;
}

void WiFiManager::disconnect() {
    if (state == WiFiState::AP_MODE) {
        stopAPMode();
    }

    WiFi.disconnect(true);
    state = WiFiState::DISCONNECTED;
    currentSSID = "";

    ESP_LOGI(TAG, "Disconnected");
}

void WiFiManager::reconnect() {
    if (ssid.isEmpty()) {
        return;
    }

    ESP_LOGI(TAG, "Reconnecting (attempt %d)...", reconnectAttempts + 1);

    if (reconnectAttempts >= WIFI_MAX_RECONNECT_ATTEMPTS) {
        ESP_LOGW(TAG, "Max reconnect attempts reached");
        return;
    }

    reconnectAttempts++;

    // Disconnect first
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
        delay(100);
    }

    // Reconnect
    WiFi.reconnect();
    state = WiFiState::CONNECTING;
    connectStartTime = millis();
}

void WiFiManager::startAPMode(const String& apName) {
    ESP_LOGI(TAG, "Starting AP mode: %s", apName.c_str());

    // Disconnect from any network
    WiFi.disconnect();
    delay(100);

    // Set AP mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apName.c_str());
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1),
                      IPAddress(192, 168, 4, 1),
                      IPAddress(255, 255, 255, 0));

    state = WiFiState::AP_MODE;
    currentSSID = apName;

    ESP_LOGI(TAG, "AP mode started, IP: %s", WiFi.softAPIP().toString().c_str());
}

void WiFiManager::stopAPMode() {
    if (state == WiFiState::AP_MODE) {
        WiFi.softAPdisconnect(true);
        ESP_LOGI(TAG, "AP mode stopped");
    }
}

bool WiFiManager::shouldReconnect() {
    // Only reconnect if enough time has passed since last disconnect
    if (millis() - lastDisconnectTime < WIFI_RECONNECT_INTERVAL_MS) {
        return false;
    }

    return true;
}

void WiFiManager::attemptConnect() {
    if (ssid.isEmpty()) {
        return;
    }

    WiFi.begin(ssid.c_str(), password.c_str());
    state = WiFiState::CONNECTING;
    connectStartTime = millis();
}

void WiFiManager::onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    if (!instance) {
        return;
    }

    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            ESP_LOGI("WiFi", "Station started");
            break;

        case ARDUINO_EVENT_WIFI_STA_STOP:
            ESP_LOGI("WiFi", "Station stopped");
            break;

        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            ESP_LOGI("WiFi", "Connected to AP");
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            ESP_LOGW("WiFi", "Disconnected, reason: %d",
                     info.wifi_sta_disconnected.reason);
            instance->lastDisconnectTime = millis();
            instance->lastDisconnectReason = info.wifi_sta_disconnected.reason;
            instance->state = WiFiState::DISCONNECTED;

            // Publish WIFI_DISCONNECTED event to EventBus
            App::getInstance().getEventBus().publish(
                Event(EventType::WIFI_DISCONNECTED));
            break;

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            ESP_LOGI("WiFi", "Got IP: %s (RSSI: %d dBm)",
                     WiFi.localIP().toString().c_str(), WiFi.RSSI());
            instance->currentSSID = WiFi.SSID();
            instance->state = WiFiState::CONNECTED;
            instance->reconnectAttempts = 0;

            // Publish WIFI_CONNECTED event to EventBus
            App::getInstance().getEventBus().publish(
                Event(EventType::WIFI_CONNECTED));
            break;

        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            ESP_LOGW("WiFi", "Lost IP address");
            break;

        default:
            break;
    }
}
