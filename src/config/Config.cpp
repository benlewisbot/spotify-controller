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
    // Create mutex for thread-safe config operations
    configMutex = xSemaphoreCreateMutex();
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

    // Check filesystem info
    Serial.printf("   Total: %d bytes, Used: %d bytes\n",
                   LittleFS.totalBytes(), LittleFS.usedBytes());

    // Generate device ID if not set
    if (config.device.deviceId.isEmpty()) {
        generateDeviceId();
    }

    // Try to load existing config
    if (!load()) {
        Serial.println("📝 No config file found, using defaults");
        Serial.println("💡 Configure via web interface or edit data/config.json");
    } else {
        Serial.println("📄 Configuration loaded");
        printConfig();
    }

    initialized = true;
    return true;
}

bool ConfigManager::load() {
    const char* BACKUP_FILE = "/config.bak";

    // Try to load main config file first
    if (LittleFS.exists(CONFIG_FILE)) {
        File file = LittleFS.open(CONFIG_FILE, "r");
        if (file) {
            StaticJsonDocument<2048> doc;
            DeserializationError error = deserializeJson(doc, file);
            file.close();

            if (!error) {
                Serial.println("📄 Configuration loaded from main file");
                JsonObject obj = doc.as<JsonObject>();
                return parseFromJson(obj);
            } else {
                Serial.printf("⚠️  Failed to parse main config: %s\n", error.c_str());
            }
        }
    }

    // Try backup file
    if (LittleFS.exists(BACKUP_FILE)) {
        Serial.println("⚠️  Trying backup config file...");
        File file = LittleFS.open(BACKUP_FILE, "r");
        if (file) {
            StaticJsonDocument<2048> doc;
            DeserializationError error = deserializeJson(doc, file);
            file.close();

            if (!error) {
                Serial.println("✅ Configuration loaded from backup");
                // Restore backup as main config
                LittleFS.rename(BACKUP_FILE, CONFIG_FILE);
                JsonObject obj = doc.as<JsonObject>();
                return parseFromJson(obj);
            } else {
                Serial.printf("⚠️  Failed to parse backup config: %s\n", error.c_str());
            }
        }
    }

    Serial.println("⚠️  No valid config file found");
    return false;
}

bool ConfigManager::save() {
    // Acquire mutex for thread-safe file operations
    if (configMutex == nullptr || xSemaphoreTake(configMutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        Serial.println("❌ Failed to acquire config mutex");
        return false;
    }

    const char* BACKUP_FILE = "/config.bak";
    const char* TEMP_FILE = "/config.tmp";

    // 1. Write to temp file first
    File file = LittleFS.open(TEMP_FILE, "w");
    if (!file) {
        Serial.println("❌ Failed to create temp config file");
        xSemaphoreGive(configMutex);
        return false;
    }

    StaticJsonDocument<2048> doc;
    JsonObject obj = doc.as<JsonObject>();
    if (!serializeToJson(obj)) {
        file.close();
        LittleFS.remove(TEMP_FILE);  // Clean up temp file
        xSemaphoreGive(configMutex);
        return false;
    }

    // Write to file
    if (serializeJson(doc, file) == 0) {
        Serial.println("❌ Failed to write config");
        file.close();
        LittleFS.remove(TEMP_FILE);  // Clean up temp file
        xSemaphoreGive(configMutex);
        return false;
    }
    file.close();

    // 2. Create backup of current config (atomic operation)
    if (LittleFS.exists(CONFIG_FILE)) {
        if (LittleFS.exists(BACKUP_FILE)) {
            LittleFS.remove(BACKUP_FILE);
        }
        LittleFS.rename(CONFIG_FILE, BACKUP_FILE);
    }

    // 3. Atomic rename: temp -> config
    if (!LittleFS.rename(TEMP_FILE, CONFIG_FILE)) {
        Serial.println("❌ Failed to rename temp to config");
        // Try to restore from backup
        if (LittleFS.exists(BACKUP_FILE)) {
            LittleFS.rename(BACKUP_FILE, CONFIG_FILE);
            Serial.println("✅ Restored config from backup");
        }
        LittleFS.remove(TEMP_FILE);  // Clean up temp file
        xSemaphoreGive(configMutex);
        return false;
    }

    // 4. Success - clean up backup (optional, keep for safety)
    // LittleFS.remove(BACKUP_FILE);  // Uncomment if you want to delete backup

    // Release mutex
    xSemaphoreGive(configMutex);

    Serial.println("💾 Configuration saved (atomic)");
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

bool ConfigManager::parseFromJson(const JsonObject& doc) {
    // WiFi - NULL SAFETY: Check for null values
    if (doc.containsKey("wifi")) {
        JsonObject wifi = doc["wifi"];
        if (!wifi.isNull()) {
            const char* ssidVal = wifi["ssid"];
            config.wifi.ssid = (ssidVal != nullptr) ? ssidVal : DEFAULT_WIFI_SSID;
            const char* passVal = wifi["password"];
            config.wifi.password = (passVal != nullptr) ? passVal : DEFAULT_WIFI_PASSWORD;
        }
    }

    // Spotify - NULL SAFETY: Check for null values
    if (doc.containsKey("spotify")) {
        JsonObject spotify = doc["spotify"];
        if (!spotify.isNull()) {
            const char* clientIdVal = spotify["client_id"];
            config.spotify.clientId = (clientIdVal != nullptr) ? clientIdVal : DEFAULT_SPOTIFY_CLIENT_ID;
            const char* secretVal = spotify["client_secret"];
            config.spotify.clientSecret = (secretVal != nullptr) ? secretVal : DEFAULT_SPOTIFY_CLIENT_SECRET;
            const char* accessTokenVal = spotify["access_token"];
            config.spotify.accessToken = (accessTokenVal != nullptr) ? accessTokenVal : "";
            const char* refreshTokenVal = spotify["refresh_token"];
            config.spotify.refreshToken = (refreshTokenVal != nullptr) ? refreshTokenVal : "";
        }
    }

    // Display - NULL SAFETY: Check for null values
    if (doc.containsKey("display")) {
        JsonObject display = doc["display"];
        if (!display.isNull()) {
            config.display.orientation = display["orientation"] | DEFAULT_DISPLAY_ORIENTATION;
            config.display.brightness = display["brightness"] | DEFAULT_BRIGHTNESS;

            if (display.containsKey("screensaver")) {
                JsonObject screensaver = display["screensaver"];
                if (!screensaver.isNull()) {
                    config.display.screensaver.enabled = screensaver["enabled"] | true;
                    config.display.screensaver.timeoutMinutes = screensaver["timeout_minutes"] | DEFAULT_SCREENSAVER_TIMEOUT;
                }
            }
        }
    }

    // Volume - NULL SAFETY: Check for null values
    if (doc.containsKey("volume")) {
        JsonObject volume = doc["volume"];
        if (!volume.isNull()) {
            config.volume.limit = volume["limit"] | DEFAULT_VOLUME_LIMIT;
        }
    }

    // Device - NULL SAFETY: Check for null values
    if (doc.containsKey("device")) {
        JsonObject device = doc["device"];
        if (!device.isNull()) {
            const char* deviceIdVal = device["device_id"];
            config.device.deviceId = (deviceIdVal != nullptr) ? deviceIdVal : "";
        }
    }

    return true;
}

bool ConfigManager::serializeToJson(JsonObject& doc) const {
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
