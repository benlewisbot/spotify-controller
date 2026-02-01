/**
 * @file WiFiManager.cpp
 * @brief WiFi Manager Implementation
 */

#include "WiFiManager.hpp"

// Static member
WiFiManager* WiFiManager::instance = nullptr;

// Connection settings
#define WIFI_CONNECT_TIMEOUT_MS 30000
#define WIFI_RECONNECT_INTERVAL_MS 5000
#define WIFI_MAX_RECONNECT_ATTEMPTS 10

WiFiManager::WiFiManager()
    : connectTimeout(WIFI_CONNECT_TIMEOUT_MS)
    , autoReconnect(true)
    , lastDisconnectReason(WIFI_REASON_UNSPECIFIED)
    , reconnectAttempts(0) {

    instance = this;
    state = WiFiState::DISCONNECTED;

    // Register WiFi event handler
    WiFi.onEvent(onWiFiEvent);

    Serial.println("📶 WiFiManager initialized");
}

WiFiManager::~WiFiManager() {
    disconnect();
}

void WiFiManager::update() {
    switch (state) {
        case WiFiState::CONNECTING:
            // Check for timeout
            if (millis() - connectStartTime > connectTimeout) {
                Serial.println("⏰ WiFi connection timeout");
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

    Serial.printf("📶 Connecting to WiFi: %s\n", ssid.c_str());

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

void WiFiManager::disconnect() {
    if (state == WiFiState::AP_MODE) {
        stopAPMode();
    }

    WiFi.disconnect(true);
    state = WiFiState::DISCONNECTED;
    currentSSID = "";

    Serial.println("📶 WiFi disconnected");
}

void WiFiManager::reconnect() {
    if (ssid.isEmpty()) {
        return;
    }

    Serial.printf("📶 Reconnecting to WiFi (attempt %d)...\n", reconnectAttempts + 1);

    if (reconnectAttempts >= WIFI_MAX_RECONNECT_ATTEMPTS) {
        Serial.println("⚠️  Max reconnect attempts reached");
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
    Serial.printf("📶 Starting AP mode: %s\n", apName.c_str());

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

    Serial.printf("✅ AP mode started, IP: %s\n", WiFi.softAPIP().toString().c_str());
}

void WiFiManager::stopAPMode() {
    if (state == WiFiState::AP_MODE) {
        WiFi.softAPdisconnect(true);
        Serial.println("📶 AP mode stopped");
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

void WiFiManager::onWiFiEvent(WiFiEvent_t event) {
    if (!instance) {
        return;
    }

    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("📶 WiFi station started");
            break;

        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("📶 WiFi station stopped");
            break;

        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("✅ WiFi connected");
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.printf("📶 WiFi disconnected: %d\n",
                         ((WiFiEventInfo_t*)xTaskGetCurrentTaskHandle())->disconnected.reason);
            instance->lastDisconnectTime = millis();
            instance->lastDisconnectReason = (WiFiMode_t)((WiFiEventInfo_t*)xTaskGetCurrentTaskHandle())->disconnected.reason;
            instance->state = WiFiState::DISCONNECTED;
            break;

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.printf("📶 WiFi got IP: %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("   Signal strength: %d dBm\n", WiFi.RSSI());
            instance->currentSSID = WiFi.SSID();
            instance->state = WiFiState::CONNECTED;
            instance->reconnectAttempts = 0;
            break;

        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("📶 WiFi lost IP");
            break;

        default:
            break;
    }
}
