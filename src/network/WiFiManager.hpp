/**
 * @file WiFiManager.hpp
 * @brief WiFi Connection Manager
 *
 * Manages WiFi connection with auto-reconnect.
 * Handles connection state and provides AP mode for initial setup.
 */

#ifndef WIFI_MANAGER_HPP
#define WIFI_MANAGER_HPP

#include <Arduino.h>
#include <WiFi.h>

/**
 * @brief WiFi State Enum
 */
enum class WiFiState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    AP_MODE,
    ERROR
};

/**
 * @brief WiFi Manager Class
 */
class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();

    /**
     * @brief Update WiFi state (call periodically)
     */
    void update();

    /**
     * @brief Connect to WiFi network
     */
    bool connect(const String& ssid, const String& password);

    /**
     * @brief Disconnect from WiFi
     */
    void disconnect();

    /**
     * @brief Reconnect to WiFi
     */
    void reconnect();

    /**
     * @brief Start AP mode for setup
     */
    void startAPMode(const String& apName = "Spotify-Controller");

    /**
     * @brief Stop AP mode
     */
    void stopAPMode();

    /**
     * @brief Get WiFi state
     */
    WiFiState getState() const { return state; }

    /**
     * @brief Check if connected
     */
    bool isConnected() const { return state == WiFiState::CONNECTED; }

    /**
     * @brief Get SSID
     */
    String getSSID() const { return currentSSID; }

    /**
     * @brief Get IP address
     */
    String getIP() const {
        if (state == WiFiState::CONNECTED) {
            return WiFi.localIP().toString();
        } else if (state == WiFiState::AP_MODE) {
            return WiFi.softAPIP().toString();
        }
        return "0.0.0.0";
    }

    /**
     * @brief Get signal strength (RSSI)
     */
    int getRSSI() const {
        if (state == WiFiState::CONNECTED) {
            return WiFi.RSSI();
        }
        return -100;
    }

    /**
     * @brief Set connection timeout
     */
    void setTimeout(unsigned long timeoutMs) { connectTimeout = timeoutMs; }

    /**
     * @brief Enable/disable auto-reconnect
     */
    void setAutoReconnect(bool enable) { autoReconnect = enable; }

    /**
     * @brief Get last disconnect reason
     */
    WiFiMode_t getLastDisconnectReason() const { return lastDisconnectReason; }

private:
    /**
     * @brief Handle WiFi events
     */
    static void onWiFiEvent(WiFiEvent_t event);

    /**
     * @brief Check if we should reconnect
     */
    bool shouldReconnect();

    /**
     * @brief Attempt to connect
     */
    void attemptConnect();

    // Connection info
    String ssid;
    String password;
    String currentSSID;

    // State
    WiFiState state;
    unsigned long connectStartTime;
    unsigned long connectTimeout;
    bool autoReconnect;

    // Disconnect tracking
    unsigned long lastDisconnectTime;
    WiFiMode_t lastDisconnectReason;
    int reconnectAttempts;

    // Singleton reference for event handler
    static WiFiManager* instance;
};

#endif // WIFI_MANAGER_HPP
