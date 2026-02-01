/**
 * @file Config.hpp
 * @brief Configuration Manager
 *
 * Manages application configuration stored in LittleFS.
 * Handles WiFi credentials, Spotify tokens, and UI preferences.
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// Configuration file path
#define CONFIG_FILE "/config.json"

// Configuration defaults
#define DEFAULT_WIFI_SSID ""
#define DEFAULT_WIFI_PASSWORD ""
#define DEFAULT_SPOTIFY_CLIENT_ID ""
#define DEFAULT_SPOTIFY_CLIENT_SECRET ""
#define DEFAULT_DISPLAY_ORIENTATION 0  // 0 = Landscape, 1 = Portrait
#define DEFAULT_BRIGHTNESS 75
#define DEFAULT_SCREENSAVER_TIMEOUT 5  // minutes
#define DEFAULT_VOLUME_LIMIT 80

/**
 * @brief Configuration Manager Class
 *
 * Singleton pattern for managing configuration.
 * Loads from/saves to LittleFS.
 */
class ConfigManager {
public:
    /**
     * @brief Get the singleton instance
     */
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    /**
     * @brief Initialize the configuration manager
     * @return true if successful, false otherwise
     */
    bool init();

    /**
     * @brief Load configuration from file
     * @return true if successful, false otherwise
     */
    bool load();

    /**
     * @brief Save configuration to file
     * @return true if successful, false otherwise
     */
    bool save();

    /**
     * @brief Reset configuration to defaults
     */
    void reset();

    // WiFi Configuration
    void setWiFiSSID(const String& ssid) { config.wifi.ssid = ssid; }
    void setWiFiPassword(const String& password) { config.wifi.password = password; }
    String getWiFiSSID() const { return config.wifi.ssid; }
    String getWiFiPassword() const { return config.wifi.password; }

    // Spotify Configuration
    void setSpotifyClientId(const String& id) { config.spotify.clientId = id; }
    void setSpotifyClientSecret(const String& secret) { config.spotify.clientSecret = secret; }
    String getSpotifyClientId() const { return config.spotify.clientId; }
    String getSpotifyClientSecret() const { return config.spotify.clientSecret; }

    // Token Management
    void saveTokens(const String& accessToken, const String& refreshToken);
    String getAccessToken() const { return config.spotify.accessToken; }
    String getRefreshToken() const { return config.spotify.refreshToken; }
    bool hasStoredTokens() const {
        return !config.spotify.accessToken.isEmpty() || !config.spotify.refreshToken.isEmpty();
    }

    // Display Configuration
    void setDisplayOrientation(int orientation) { config.display.orientation = orientation; }
    int getDisplayOrientation() const { return config.display.orientation; }

    void setBrightness(int brightness) {
        config.display.brightness = constrain(brightness, 0, 100);
    }
    int getBrightness() const { return config.display.brightness; }

    void setScreensaverEnabled(bool enabled) { config.display.screensaver.enabled = enabled; }
    bool getScreensaverEnabled() const { return config.display.screensaver.enabled; }

    void setScreensaverTimeout(int timeout) {
        config.display.screensaver.timeoutMinutes = constrain(timeout, 1, 60);
    }
    int getScreensaverTimeout() const { return config.display.screensaver.timeoutMinutes; }

    // Volume Configuration
    void setVolumeLimit(int limit) {
        config.volume.limit = constrain(limit, 0, 100);
    }
    int getVolumeLimit() const { return config.volume.limit; }

    // Device Configuration
    String getDeviceId() const { return config.device.deviceId; }
    void generateDeviceId();

    /**
     * @brief Check if configuration has been set up
     * @return true if WiFi is configured
     */
    bool isConfigured() const {
        return !config.wifi.ssid.isEmpty();
    }

    /**
     * @brief Print current configuration to Serial
     */
    void printConfig() const;

private:
    ConfigManager();
    ~ConfigManager();

    /**
     * @brief Create default configuration
     */
    void createDefaults();

    /**
     * @brief Parse JSON to config struct
     */
    bool parseFromJson(const JsonDocument& doc);

    /**
     * @brief Serialize config struct to JSON
     */
    bool serializeToJson(JsonDocument& doc) const;

    // Configuration structure
    struct {
        struct {
            String ssid;
            String password;
        } wifi;

        struct {
            String clientId;
            String clientSecret;
            String accessToken;
            String refreshToken;
        } spotify;

        struct {
            int orientation;
            int brightness;
            struct {
                bool enabled;
                int timeoutMinutes;
            } screensaver;
        } display;

        struct {
            int limit;
        } volume;

        struct {
            String deviceId;
        } device;
    } config;

    bool initialized;
};

#endif // CONFIG_HPP
