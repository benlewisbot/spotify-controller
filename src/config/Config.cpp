/**
 * @file Config.cpp
 * @brief Configuration Manager Implementation
 */

#include "Config.hpp"
#include <esp_log.h>

// For device ID generation
#include <esp_system.h>
#include <esp_mac.h>

static const char* TAG = "Config";

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

    // Initialize LittleFS
    if (!LittleFS.begin(true)) {  // true = format if fails
        ESP_LOGE(TAG, "Failed to mount LittleFS");
        return false;
    }

    ESP_LOGI(TAG, "LittleFS: %d/%d bytes used",
             LittleFS.usedBytes(), LittleFS.totalBytes());

    // Generate device ID if not set
    if (config.device.deviceId.isEmpty()) {
        generateDeviceId();
    }

    // Try to load existing config
    if (!load()) {
        ESP_LOGI(TAG, "No config file, saving defaults");
        save();  // Create default config file to avoid errors on next boot
    }

    printConfig();  // Log what we loaded

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
                JsonObject obj = doc.as<JsonObject>();
                return parseFromJson(obj);
            } else {
                ESP_LOGW(TAG, "Config parse error: %s", error.c_str());
            }
        }
    }

    // Try backup file
    if (LittleFS.exists(BACKUP_FILE)) {
        ESP_LOGI(TAG, "Trying backup config...");
        File file = LittleFS.open(BACKUP_FILE, "r");
        if (file) {
            StaticJsonDocument<2048> doc;
            DeserializationError error = deserializeJson(doc, file);
            file.close();

            if (!error) {
                LittleFS.rename(BACKUP_FILE, CONFIG_FILE);
                JsonObject obj = doc.as<JsonObject>();
                return parseFromJson(obj);
            } else {
                ESP_LOGW(TAG, "Backup parse error: %s", error.c_str());
            }
        }
    }

    return false;
}

bool ConfigManager::save() {
    if (configMutex == nullptr || xSemaphoreTake(configMutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to acquire config mutex");
        return false;
    }

    const char* BACKUP_FILE = "/config.bak";
    const char* TEMP_FILE = "/config.tmp";

    // Write to temp file first (atomic save pattern)
    File file = LittleFS.open(TEMP_FILE, "w");
    if (!file) {
        ESP_LOGE(TAG, "Failed to create temp config");
        xSemaphoreGive(configMutex);
        return false;
    }

    StaticJsonDocument<2048> doc;
    JsonObject obj = doc.to<JsonObject>();  // to<> creates the object; as<> would return null
    if (!serializeToJson(obj)) {
        file.close();
        LittleFS.remove(TEMP_FILE);
        xSemaphoreGive(configMutex);
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        ESP_LOGE(TAG, "Failed to write config");
        file.close();
        LittleFS.remove(TEMP_FILE);
        xSemaphoreGive(configMutex);
        return false;
    }
    file.close();

    // Backup current, then atomic rename
    if (LittleFS.exists(CONFIG_FILE)) {
        if (LittleFS.exists(BACKUP_FILE)) {
            LittleFS.remove(BACKUP_FILE);
        }
        LittleFS.rename(CONFIG_FILE, BACKUP_FILE);
    }

    if (!LittleFS.rename(TEMP_FILE, CONFIG_FILE)) {
        ESP_LOGE(TAG, "Atomic rename failed");
        if (LittleFS.exists(BACKUP_FILE)) {
            LittleFS.rename(BACKUP_FILE, CONFIG_FILE);
        }
        LittleFS.remove(TEMP_FILE);
        xSemaphoreGive(configMutex);
        return false;
    }

    xSemaphoreGive(configMutex);
    return true;
}

void ConfigManager::reset() {
    createDefaults();
    save();
    ESP_LOGI(TAG, "Config reset to defaults");
}

void ConfigManager::saveTokens(const String& accessToken, const String& refreshToken) {
    config.spotify.accessToken = accessToken;
    config.spotify.refreshToken = refreshToken;
    save();
    ESP_LOGI(TAG, "Spotify tokens saved");
}

void ConfigManager::generateDeviceId() {
    // Generate a unique device ID from MAC address
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    char deviceId[13];
    sprintf(deviceId, "ESP%02X%02X%02X%02X%02X%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    config.device.deviceId = String(deviceId);

    ESP_LOGI(TAG, "Device ID: %s", config.device.deviceId.c_str());
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
    ESP_LOGI(TAG, "WiFi SSID: %s", config.wifi.ssid.isEmpty() ? "(not set)" : config.wifi.ssid.c_str());
    ESP_LOGI(TAG, "Spotify: client_id=%s tokens=%s",
             config.spotify.clientId.isEmpty() ? "no" : "yes",
             config.spotify.accessToken.isEmpty() ? "no" : "yes");
    ESP_LOGI(TAG, "Display: brightness=%d%% orientation=%d",
             config.display.brightness, config.display.orientation);
    ESP_LOGI(TAG, "Device: %s", config.device.deviceId.c_str());
}
