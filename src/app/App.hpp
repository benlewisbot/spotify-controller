/**
 * @file App.hpp
 * @brief Main Application Controller for Spotify Controller
 *
 * This is the central controller that manages all subsystems:
 * - WiFi connectivity
 * - Display and UI
 * - Spotify client
 * - Input handling
 * - State management
 */

#ifndef APP_HPP
#define APP_HPP

#include <Arduino.h>
#include "State.hpp"
#include "EventBus.hpp"

// Forward declarations
class DisplayManager;
class SpotifyClient;
class WiFiManager;
class AuthManager;
class ConfigManager;

namespace ui {
    class WindowManager;
    class NowPlayingScreen;
}

/**
 * @brief Main Application Class
 *
 * Singleton pattern for managing the entire application lifecycle.
 */
class App {
public:
    /**
     * @brief Get the singleton instance
     */
    static App& getInstance() {
        static App instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    /**
     * @brief Initialize the application
     * @return true if successful, false otherwise
     */
    bool init();

    /**
     * @brief Main application loop - call this from Arduino loop()
     */
    void loop();

    /**
     * @brief Get the current application state
     */
    AppState getState() const { return state; }

    /**
     * @brief Set a new application state
     */
    void setState(AppState newState);

    /**
     * @brief Check if the app is initialized
     */
    bool isInitialized() const { return initialized; }

    /**
     * @brief Get the event bus for publishing/subscribing to events
     */
    EventBus& getEventBus() { return eventBus; }

    /**
     * @brief Get the Spotify client
     */
    SpotifyClient* getSpotifyClient() { return spotifyClient; }

    /**
     * @brief Get the WiFi manager
     */
    WiFiManager* getWiFiManager() { return wifiManager; }

    /**
     * @brief Get the display manager
     */
    DisplayManager* getDisplayManager() { return displayManager; }

    /**
     * @brief Schedule a task to run on the main loop
     * @param task Function to execute
     * @param delayMs Delay before execution (0 = next loop)
     */
    void scheduleTask(std::function<void()> task, unsigned long delayMs = 0);

    /**
     * @brief Trigger a full UI refresh
     */
    void refreshUI();

private:
    App();
    ~App();

    /**
     * @brief Initialize subsystems in order
     */
    bool initConfig();
    bool initLogger();
    bool initWiFi();
    bool initDisplay();
    bool initSpotify();
    bool initUI();

    /**
     * @brief Handle WiFi connection state changes
     */
    void onWiFiConnected();
    void onWiFiDisconnected();

    /**
     * @brief Handle Spotify authentication state changes
     */
    void onSpotifyAuthenticated();
    void onSpotifyAuthError();

    /**
     * @brief Handle playback state changes
     */
    void onPlaybackChanged();
    void onTrackChanged();

    /**
     * @brief Register event handlers
     */
    void registerEventHandlers();

    /**
     * @brief Execute scheduled tasks
     */
    void executeScheduledTasks();

    // Member variables
    bool initialized;
    AppState state;

    EventBus eventBus;

    // Subsystems
    ConfigManager* configManager;
    WiFiManager* wifiManager;
    DisplayManager* displayManager;
    AuthManager* authManager;
    SpotifyClient* spotifyClient;
    ui::WindowManager* windowManager;

    // Task scheduling
    struct ScheduledTask {
        std::function<void()> task;
        unsigned long executeTime;
    };
    std::vector<ScheduledTask> scheduledTasks;
};

#endif // APP_HPP
