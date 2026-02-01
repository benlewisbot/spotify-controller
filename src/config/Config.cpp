/**
 * @file Config.cpp
 * @brief Configuration Manager Implementation
 */

#include "Config.hpp"

// For device ID generation
#include <esp_system.h>
#include <esp_mac.h>

ConfigManager::ConfigManager() : initialized(false) {
    createDefaults();
}

ConfigManager::~ConfigManager() {
}

void ConfigManager::createDefaults() {
    config.wifi.ssid = DEFAULT_WIFI_SSID;
    config.wifi.password = DEFAULT_WIFI_PASSWORD;
    config.spotify.clientId = DEFAULT_SPOTIFY_CLIENT_ID;
    config.spotify.clientSecret = DEFAULT_SPOTIFY_CLIENT_SECRET;
    config.spotify.accessToken = "";
    config.spotify.refreshToken = "";
    config.display.orientation = DEFAULT_DISPLAY_ORIENTATION;
    config.display.brightness = DEFAULT_BRIGHTNESS;
    config.display.screensaver.enabled = true;
    config.display.screensaver.timeoutMinutes = DEFAULT_SCREENSAVER_TIMEOUT;
    config.volume.limit = DEFAULT_VOLUME_LIMIT;
    config.device.deviceId = "";
}

bool ConfigManager::init() {
    if (initialized) {
        return true;
    }

    Serial.println("📁 Initializing ConfigManager...");

    // Initialize LittleFS
    if (!LittleFS.begin(true)) {  // true = format if fails
        Serial.println("❌ Failed to mount LittleFS");
        return false;
    }

    Serial.println("✅ LittleFS mounted");

    // Generate device ID if not set
    if (config.device.deviceId.isEmpty()) {
        generateDeviceId();
    }

    // Try to load existing config
    if (!load()) {
        Serial.println("📝 No config file found, using defaults");
    } else {
        Serial.println("📄 Configuration loaded");
        printConfig();
    }

    initialized = true;
    return true;
}

bool ConfigManager::load() {
    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        return false;
    }

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.printf("⚠️  Failed to parse config: %s\n", error.c_str());
        return false;
    }

    return parseFromJson(doc);
}

bool ConfigManager::save() {
    // Create a temporary file first
    String tempFile = CONFIG_FILE;
    tempFile += ".tmp";

    File file = LittleFS.open(tempFile, "w");
    if (!file) {
        Serial.println("❌ Failed to create config file");
        return false;
    }

    StaticJsonDocument<2048> doc;
    if (!serializeToJson(doc)) {
        file.close();
        return false;
    }

    // Write to file
    if (serializeJson(doc, file) == 0) {
        Serial.println("❌ Failed to write config");
        file.close();
        return false;
    }
    file.close();

    // Delete old config and rename temp
    LittleFS.remove(CONFIG_FILE);
    if (!LittleFS.rename(tempFile, CONFIG_FILE)) {
        Serial.println("❌ Failed to rename config file");
        return false;
    }

    Serial.println("💾 Configuration saved");
    return true;
}

void ConfigManager::reset() {
    createDefaults();
    save();
    Serial.println("🔄 Configuration reset to defaults");
}

void ConfigManager::saveTokens(const String& accessToken, const String& refreshToken) {
    config.spotify.accessToken = accessToken;
    config.spotify.refreshToken = refreshToken;
    save();
    Serial.println("💾 Spotify tokens saved");
}

void ConfigManager::generateDeviceId() {
    // Generate a unique device ID from MAC address
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    char deviceId[13];
    sprintf(deviceId, "ESP%02X%02X%02X%02X%02X%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    config.device.deviceId = String(deviceId);

    Serial.printf("📱 Device ID: %s\n", config.device.deviceId.c_str());
}

bool ConfigManager::parseFromJson(const JsonDocument& doc) {
    // WiFi
    if (doc.containsKey("wifi")) {
        JsonObject wifi = doc["wifi"];
        config.wifi.ssid = wifi["ssid"] | DEFAULT_WIFI_SSID;
        config.wifi.password = wifi["password"] | DEFAULT_WIFI_PASSWORD;
    }

    // Spotify
    if (doc.containsKey("spotify")) {
        JsonObject spotify = doc["spotify"];
        config.spotify.clientId = spotify["client_id"] | DEFAULT_SPOTIFY_CLIENT_ID;
        config.spotify.clientSecret = spotify["client_secret"] | DEFAULT_SPOTIFY_CLIENT_SECRET;
        config.spotify.accessToken = spotify["access_token"] | "";
        config.spotify.refreshToken = spotify["refresh_token"] | "";
    }

    // Display
    if (doc.containsKey("display")) {
        JsonObject display = doc["display"];
        config.display.orientation = display["orientation"] | DEFAULT_DISPLAY_ORIENTATION;
        config.display.brightness = display["brightness"] | DEFAULT_BRIGHTNESS;

        if (display.containsKey("screensaver")) {
            JsonObject screensaver = display["screensaver"];
            config.display.screensaver.enabled = screensaver["enabled"] | true;
            config.display.screensaver.timeoutMinutes = screensaver["timeout_minutes"] | DEFAULT_SCREENSAVER_TIMEOUT;
        }
    }

    // Volume
    if (doc.containsKey("volume")) {
        JsonObject volume = doc["volume"];
        config.volume.limit = volume["limit"] | DEFAULT_VOLUME_LIMIT;
    }

    // Device
    if (doc.containsKey("device")) {
        JsonObject device = doc["device"];
        config.device.deviceId = device["device_id"] | "";
    }

    return true;
}

bool ConfigManager::serializeToJson(JsonDocument& doc) const {
    // WiFi
    JsonObject wifi = doc.createNestedObject("wifi");
    wifi["ssid"] = config.wifi.ssid;
    wifi["password"] = config.wifi.password;

    // Spotify
    JsonObject spotify = doc.createNestedObject("spotify");
    spotify["client_id"] = config.spotify.clientId;
    spotify["client_secret"] = config.spotify.clientSecret;
    spotify["access_token"] = config.spotify.accessToken;
    spotify["refresh_token"] = config.spotify.refreshToken;

    // Display
    JsonObject display = doc.createNestedObject("display");
    display["orientation"] = config.display.orientation;
    display["brightness"] = config.display.brightness;

    JsonObject screensaver = display.createNestedObject("screensaver");
    screensaver["enabled"] = config.display.screensaver.enabled;
    screensaver["timeout_minutes"] = config.display.screensaver.timeoutMinutes;

    // Volume
    JsonObject volume = doc.createNestedObject("volume");
    volume["limit"] = config.volume.limit;

    // Device
    JsonObject device = doc.createNestedObject("device");
    device["device_id"] = config.device.deviceId;

    return true;
}

void ConfigManager::printConfig() const {
    Serial.println("\n📋 Current Configuration:");
    Serial.println("─────────────────────────────────");

    Serial.println("WiFi:");
    Serial.printf("  SSID: %s\n", config.wifi.ssid.c_str());
    Serial.printf("  Password: %s\n", config.wifi.password.isEmpty() ? "(not set)" : "***");

    Serial.println("\nSpotify:");
    Serial.printf("  Client ID: %s\n",
                   config.spotify.clientId.isEmpty() ? "(not set)" : "***");
    Serial.printf("  Access Token: %s\n",
                   config.spotify.accessToken.isEmpty() ? "(not set)" : "***");
    Serial.printf("  Refresh Token: %s\n",
                   config.spotify.refreshToken.isEmpty() ? "(not set)" : "***");

    Serial.println("\nDisplay:");
    Serial.printf("  Orientation: %s\n",
                   config.display.orientation == 0 ? "Landscape" : "Portrait");
    Serial.printf("  Brightness: %d%%\n", config.display.brightness);
    Serial.printf("  Screensaver: %s (%d min)\n",
                   config.display.screensaver.enabled ? "ON" : "OFF",
                   config.display.screensaver.timeoutMinutes);

    Serial.println("\nVolume:");
    Serial.printf("  Limit: %d%%\n", config.volume.limit);

    Serial.println("\nDevice:");
    Serial.printf("  ID: %s\n", config.device.deviceId.c_str());

    Serial.println("─────────────────────────────────\n");
}
