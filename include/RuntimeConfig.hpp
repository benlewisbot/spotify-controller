/**
 * @file RuntimeConfig.hpp
 * @brief Runtime Configuration Management for Spotify Controller
 *
 * Handles persistent storage of all application settings using LittleFS.
 * Provides type-safe configuration with default values.
 */

#ifndef RUNTIME_CONFIG_HPP
#define RUNTIME_CONFIG_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <esp_log.h>

// Configuration file path
#define RUNTIME_CONFIG_FILE "/spotify_config.json"

// Default values
#define DEFAULT_BRIGHTNESS 75
#define DEFAULT_SCREENSAVER_TIMEOUT 5 // minutes
#define DEFAULT_AUTO_RECONNECT true
#define DEFAULT_TOUCH_SOUND true
#define DEFAULT_ROTATION 0 // 0=Portrait, 1=Landscape

// Configuration structure
struct RuntimeConfig {
    // Display settings
    struct Display {
        uint8_t brightness;           // 0-100
        uint8_t rotation;             // 0=Portrait, 1=Landscape
        uint16_t screensaverTimeout;  // minutes, 0 = disabled
        bool screensaverEnabled;
        
        Display() : 
            brightness(DEFAULT_BRIGHTNESS),
            rotation(DEFAULT_ROTATION),
            screensaverTimeout(DEFAULT_SCREENSAVER_TIMEOUT),
            screensaverEnabled(true) {}
    } display;
    
    // WiFi settings
    struct WiFi {
        bool autoReconnect;
        int connectionTimeout;        // seconds
        String lastSSID;
        
        WiFi() : 
            autoReconnect(DEFAULT_AUTO_RECONNECT),
            connectionTimeout(10) {}
    } wifi;
    
    // Spotify settings
    struct Spotify {
        bool autoPlay;
        bool showExplicit;
        String deviceName;
        String userId;
        String userEmail;
        
        Spotify() : 
            autoPlay(true),
            showExplicit(true),
            deviceName("SpotifyController") {}
    } spotify;
    
    // Interface settings
    struct Interface {
        bool touchSound;
        bool hapticFeedback;
        bool showBatteryLevel;
        int uiScale;                  // 80, 100, 120
        
        Interface() : 
            touchSound(DEFAULT_TOUCH_SOUND),
            hapticFeedback(true),
            showBatteryLevel(true),
            uiScale(100) {}
    } interface;
    
    // System settings
    struct System {
        String firmwareVersion;
        String buildDate;
        String deviceId;
        unsigned long bootCount;
        
        System() : 
            firmwareVersion("1.0.0"),
            bootCount(0) {
            buildDate = __DATE__;
            buildDate += " ";
            buildDate += __TIME__;
        }
    } system;
    
    // JSON serialization
    void toJson(JsonObject doc) {
        JsonObject d = doc.createNestedObject("display");
        d["brightness"] = display.brightness;
        d["rotation"] = display.rotation;
        d["screensaverTimeout"] = display.screensaverTimeout;
        d["screensaverEnabled"] = display.screensaverEnabled;
        
        JsonObject w = doc.createNestedObject("wifi");
        w["autoReconnect"] = wifi.autoReconnect;
        w["connectionTimeout"] = wifi.connectionTimeout;
        w["lastSSID"] = wifi.lastSSID;
        
        JsonObject s = doc.createNestedObject("spotify");
        s["autoPlay"] = spotify.autoPlay;
        s["showExplicit"] = spotify.showExplicit;
        s["deviceName"] = spotify.deviceName;
        s["userId"] = spotify.userId;
        s["userEmail"] = spotify.userEmail;
        
        JsonObject i = doc.createNestedObject("interface");
        i["touchSound"] = interface.touchSound;
        i["hapticFeedback"] = interface.hapticFeedback;
        i["showBatteryLevel"] = interface.showBatteryLevel;
        i["uiScale"] = interface.uiScale;
        
        JsonObject sys = doc.createNestedObject("system");
        sys["firmwareVersion"] = system.firmwareVersion;
        sys["buildDate"] = system.buildDate;
        sys["deviceId"] = system.deviceId;
        sys["bootCount"] = system.bootCount;
    }
    
    // JSON deserialization with NULL SAFETY
    bool fromJson(const JsonObject doc) {
        if (!doc.containsKey("display")) return false;

        // Display - NULL SAFETY: Check for null values
        display.brightness = doc["display"]["brightness"] | DEFAULT_BRIGHTNESS;
        display.rotation = doc["display"]["rotation"] | DEFAULT_ROTATION;
        display.screensaverTimeout = doc["display"]["screensaverTimeout"] | DEFAULT_SCREENSAVER_TIMEOUT;
        display.screensaverEnabled = doc["display"]["screensaverEnabled"] | true;

        // WiFi - NULL SAFETY: Check for null values
        wifi.autoReconnect = doc["wifi"]["autoReconnect"] | DEFAULT_AUTO_RECONNECT;
        wifi.connectionTimeout = doc["wifi"]["connectionTimeout"] | 10;
        const char* ssidVal = doc["wifi"]["lastSSID"];
        wifi.lastSSID = (ssidVal != nullptr) ? ssidVal : "";

        // Spotify - NULL SAFETY: Check for null values
        spotify.autoPlay = doc["spotify"]["autoPlay"] | true;
        spotify.showExplicit = doc["spotify"]["showExplicit"] | true;
        const char* deviceNameVal = doc["spotify"]["deviceName"];
        spotify.deviceName = (deviceNameVal != nullptr) ? deviceNameVal : "SpotifyController";
        const char* userIdVal = doc["spotify"]["userId"];
        spotify.userId = (userIdVal != nullptr) ? userIdVal : "";
        const char* emailVal = doc["spotify"]["userEmail"];
        spotify.userEmail = (emailVal != nullptr) ? emailVal : "";

        // Interface - NULL SAFETY: Check for null values
        interface.touchSound = doc["interface"]["touchSound"] | DEFAULT_TOUCH_SOUND;
        interface.hapticFeedback = doc["interface"]["hapticFeedback"] | true;
        interface.showBatteryLevel = doc["interface"]["showBatteryLevel"] | true;
        interface.uiScale = doc["interface"]["uiScale"] | 100;

        // System - NULL SAFETY: Check for null values
        const char* firmwareVal = doc["system"]["firmwareVersion"];
        system.firmwareVersion = (firmwareVal != nullptr) ? firmwareVal : "1.0.0";
        const char* buildDateVal = doc["system"]["buildDate"];
        system.buildDate = (buildDateVal != nullptr) ? buildDateVal : "";
        const char* deviceIdVal = doc["system"]["deviceId"];
        system.deviceId = (deviceIdVal != nullptr) ? deviceIdVal : "";
        system.bootCount = doc["system"]["bootCount"] | 0;

        return true;
    }
};

/**
 * @brief Runtime Configuration Manager
 *
 * Singleton class for managing configuration persistence.
 */
class RuntimeConfigManager {
public:
    /**
     * @brief Get singleton instance
     */
    static RuntimeConfigManager& getInstance() {
        static RuntimeConfigManager instance;
        return instance;
    }
    
    // Delete copy constructor and assignment operator
    RuntimeConfigManager(const RuntimeConfigManager&) = delete;
    RuntimeConfigManager& operator=(const RuntimeConfigManager&) = delete;
    
    /**
     * @brief Initialize configuration system
     */
    bool begin() {
        configMutex = xSemaphoreCreateMutex();
        if (configMutex == nullptr) {
            ESP_LOGE("RtCfg", "Failed to create mutex");
            return false;
        }

        // LittleFS is already mounted by ConfigManager::init()
        // No need to call LittleFS.begin() again

        if (!load()) {
            save();  // Create default config file
        }

        config.system.bootCount++;
        save();

        ESP_LOGI("RtCfg", "Boot #%lu", config.system.bootCount);
        return true;
    }
    
    /**
     * @brief Get configuration
     */
    RuntimeConfig& getConfig() { return config; }
    
    /**
     * @brief Save configuration to file
     */
    bool save() {
        if (configMutex == nullptr || xSemaphoreTake(configMutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
            ESP_LOGE("RtCfg", "Mutex acquire failed");
            return false;
        }

        DynamicJsonDocument doc(2048);
        config.toJson(doc.as<JsonObject>());

        File file = LittleFS.open(RUNTIME_CONFIG_FILE, "w");
        if (!file) {
            ESP_LOGE("RtCfg", "Failed to open config for writing");
            xSemaphoreGive(configMutex);
            return false;
        }

        serializeJson(doc, file);
        file.close();
        xSemaphoreGive(configMutex);
        return true;
    }
    
    /**
     * @brief Load configuration from file
     */
    bool load() {
        if (!LittleFS.exists(RUNTIME_CONFIG_FILE)) {
            return false;
        }
        
        File file = LittleFS.open(RUNTIME_CONFIG_FILE, "r");
        if (!file) {
            return false;
        }
        
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        
        if (error) {
            ESP_LOGW("RtCfg", "Parse error: %s", error.c_str());
            return false;
        }
        
        if (!config.fromJson(doc.as<JsonObject>())) {
            ESP_LOGW("RtCfg", "Invalid config format");
            return false;
        }
        
        return true;
    }
    
    /**
     * @brief Reset to defaults
     */
    bool reset() {
        config = RuntimeConfig(); // Reset to defaults
        return save();
    }
    
    /**
     * @brief Export configuration as JSON string
     */
    String exportJson() {
        DynamicJsonDocument doc(2048);
        config.toJson(doc.as<JsonObject>());
        String json;
        serializeJson(doc, json);
        return json;
    }

    /**
     * @brief Import configuration from JSON string
     */
    bool importJson(const String& json) {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
            return false;
        }
        return config.fromJson(doc.as<JsonObject>()) && save();
    }
    
    /**
     * @brief Set device ID
     */
    void setDeviceId(const String& id) {
        config.system.deviceId = id;
        save();
    }
    
    /**
     * @brief Set Spotify user info
     */
    void setSpotifyUserInfo(const String& userId, const String& email) {
        config.spotify.userId = userId;
        config.spotify.userEmail = email;
        save();
    }
    
    /**
     * @brief Set last connected SSID
     */
    void setLastSSID(const String& ssid) {
        config.wifi.lastSSID = ssid;
        save();
    }
    
private:
    RuntimeConfigManager() = default;
    ~RuntimeConfigManager() = default;

    RuntimeConfig config;

    // Mutex for thread-safe config operations
    SemaphoreHandle_t configMutex;
};

#endif // RUNTIME_CONFIG_HPP
