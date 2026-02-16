/**
 * wifi_manager.h - WiFi & Auth Manager
 * Behandelt WiFi Verbindung, Hotspot Mode und Authentifizierung
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "config.h"

// WiFi Status
enum WiFiStatus {
    WIFI_DISCONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_HOTSPOT_ACTIVE
};

// Credentials
struct Credentials {
    String ssid;
    String password;
    String spotifyClientId;
    String spotifyClientSecret;
};

class WiFiManager {
public:
    WiFiManager();
    
    void begin();
    void update();
    
    // WiFi Operationen
    bool connectToWiFi(const String& ssid, const String& password);
    void startHotspot();
    void stopHotspot();
    
    // Status
    WiFiStatus getStatus() const { return status; }
    String getSSID() const;
    IPAddress getIP() const;
    bool isConnected() const { return status == WIFI_CONNECTED; }
    
    // Credentials Management
    bool saveCredentials(const Credentials& creds);
    bool loadCredentials(Credentials& creds);
    bool hasCredentials() const;
    void clearCredentials();
    
    // Auth Flow
    bool startAuthServer();
    void stopAuthServer();
    bool authCompleted();
    String getAuthCode();
    
    // Web Server Callbacks
    void handleRoot();
    void handleSetup();
    void handleCallback();
    void handleSave();
    void handleNotFound();
    
    // Reset
    void factoryReset();
    
private:
    WiFiStatus status;
    WebServer* server;
    String authCode;
    bool authCodeReceived;
    unsigned long lastConnectAttempt;
    int connectAttempts;
    
    static constexpr int MAX_CONNECT_ATTEMPTS = 3;
    static constexpr unsigned long RECONNECT_INTERVAL = 30000; // 30s
    
    void initWebServer();
    void handleWiFiConnection();
};

#endif // WIFI_MANAGER_H
