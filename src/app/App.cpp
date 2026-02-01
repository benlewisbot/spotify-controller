/**
 * @file App.cpp
 * @brief Main Application Controller Implementation
 */

#include "App.hpp"

// Include subsystem headers
#include "../config/Config.hpp"
#include "../network/WiFiManager.hpp"
#include "../display/DisplayManager.hpp"
#include "../spotify/SpotifyClient.hpp"
#include "../spotify/AuthManager.hpp"
#include "../ui/WindowManager.hpp"
#include "../ui/screens/NowPlaying.hpp"

#include <WiFi.h>

App::App()
    : initialized(false)
    , state(AppState::INIT)
    , configManager(nullptr)
    , wifiManager(nullptr)
    , displayManager(nullptr)
    , authManager(nullptr)
    , spotifyClient(nullptr)
    , windowManager(nullptr) {
}

App::~App() {
    // Cleanup subsystems in reverse order
    delete windowManager;
    delete spotifyClient;
    delete authManager;
    delete displayManager;
    delete wifiManager;
    delete configManager;
}

bool App::init() {
    Serial.println("\n========================================");
    Serial.println("  Spotify Controller ESP32");
    Serial.println("  Version: 1.0.0 (Phase 1 MVP)");
    Serial.println("========================================\n");

    // Initialize subsystems in order
    setState(AppState::INIT);

    if (!initConfig()) {
        Serial.println("❌ Config initialization failed");
        return false;
    }
    Serial.println("✅ Config initialized");

    if (!initLogger()) {
        Serial.println("❌ Logger initialization failed");
        return false;
    }
    Serial.println("✅ Logger initialized");

    if (!initWiFi()) {
        Serial.println("❌ WiFi initialization failed");
        return false;
    }
    Serial.println("✅ WiFi initialized");

    if (!initDisplay()) {
        Serial.println("❌ Display initialization failed");
        return false;
    }
    Serial.println("✅ Display initialized");

    if (!initSpotify()) {
        Serial.println("❌ Spotify initialization failed");
        return false;
    }
    Serial.println("✅ Spotify initialized");

    if (!initUI()) {
        Serial.println("❌ UI initialization failed");
        return false;
    }
    Serial.println("✅ UI initialized");

    // Register event handlers
    registerEventHandlers();

    initialized = true;
    setState(AppState::READY);

    Serial.println("\n========================================");
    Serial.println("  🎵 Spotify Controller Ready!");
    Serial.println("========================================\n");

    return true;
}

void App::loop() {
    if (!initialized) {
        return;
    }

    // Execute scheduled tasks
    executeScheduledTasks();

    // Update window manager (LVGL tasks)
    if (windowManager) {
        windowManager->update();
    }

    // Poll WiFi status
    if (wifiManager) {
        wifiManager->update();
    }

    // Poll Spotify status (periodically)
    if (spotifyClient && state == AppState::NOW_PLAYING) {
        static unsigned long lastPoll = 0;
        if (millis() - lastPoll >= SPOTIFY_POLL_INTERVAL_MS) {
            spotifyClient->updateNowPlaying();
            lastPoll = millis();
        }
    }
}

void App::setState(AppState newState) {
    if (state != newState) {
        Serial.printf("🔄 State change: %d -> %d\n", state, newState);
        state = newState;

        // Publish state change event
        eventBus.publish(Event(EventType::STATE_CHANGED, static_cast<int>(newState)));
    }
}

void App::scheduleTask(std::function<void()> task, unsigned long delayMs) {
    ScheduledTask st;
    st.task = task;
    st.executeTime = millis() + delayMs;
    scheduledTasks.push_back(st);
}

void App::refreshUI() {
    if (windowManager) {
        eventBus.publish(Event(EventType::UI_REFRESH));
    }
}

// Initialization methods

bool App::initConfig() {
    configManager = new ConfigManager();
    return configManager->init();
}

bool App::initLogger() {
    // Logger is already initialized via Serial
    return true;
}

bool App::initWiFi() {
    wifiManager = new WiFiManager();

    // Register event handlers
    eventBus.subscribe(EventType::WIFI_CONNECTED,
        [this](const Event& e) { this->onWiFiConnected(); });

    eventBus.subscribe(EventType::WIFI_DISCONNECTED,
        [this](const Event& e) { this->onWiFiDisconnected(); });

    // Connect to WiFi
    if (!wifiManager->connect(configManager->getWiFiSSID(),
                               configManager->getWiFiPassword())) {
        Serial.println("⚠️  WiFi connection failed, will retry...");
        return false;
    }

    return true;
}

bool App::initDisplay() {
    displayManager = DisplayManager::getInstance();

    if (!displayManager->init()) {
        return false;
    }

    // Wait for display to be ready
    delay(100);

    return true;
}

bool App::initSpotify() {
    // Create authentication manager
    authManager = new AuthManager();
    authManager->init(
        configManager->getSpotifyClientId(),
        configManager->getSpotifyClientSecret()
    );

    // Create Spotify client
    spotifyClient = new SpotifyClient(authManager);

    // Check if we have stored tokens
    if (configManager->hasStoredTokens()) {
        Serial.println("🎫 Found stored tokens, attempting to use...");
        String accessToken = configManager->getAccessToken();
        String refreshToken = configManager->getRefreshToken();
        spotifyClient->setTokens(accessToken, refreshToken);
    } else {
        Serial.println("🔐 No stored tokens found, authentication required");
        setState(AppState::AUTH_REQUIRED);
    }

    // Register Spotify event handlers
    eventBus.subscribe(EventType::SPOTIFY_AUTHENTICATED,
        [this](const Event& e) { this->onSpotifyAuthenticated(); });

    eventBus.subscribe(EventType::SPOTIFY_AUTH_ERROR,
        [this](const Event& e) { this->onSpotifyAuthError(); });

    eventBus.subscribe(EventType::PLAYBACK_CHANGED,
        [this](const Event& e) { this->onPlaybackChanged(); });

    eventBus.subscribe(EventType::TRACK_CHANGED,
        [this](const Event& e) { this->onTrackChanged(); });

    return true;
}

bool App::initUI() {
    windowManager = new ui::WindowManager(displayManager);
    windowManager->init();

    // Show initial screen based on state
    if (state == AppState::AUTH_REQUIRED) {
        windowManager->showAuthScreen();
    } else {
        windowManager->showNowPlaying();
    }

    return true;
}

// Event handlers

void App::onWiFiConnected() {
    Serial.println("📶 WiFi connected!");
    Serial.printf("  IP: %s\n", WiFi.localIP().toString().c_str());

    // If we need auth, start auth server
    if (state == AppState::AUTH_REQUIRED && authManager) {
        authManager->startAuthServer();
    }
}

void App::onWiFiDisconnected() {
    Serial.println("📶 WiFi disconnected, attempting to reconnect...");
}

void App::onSpotifyAuthenticated() {
    Serial.println("✅ Spotify authenticated!");

    // Save tokens
    if (spotifyClient && configManager) {
        configManager->saveTokens(
            spotifyClient->getAccessToken(),
            spotifyClient->getRefreshToken()
        );
    }

    // Show now playing screen
    if (windowManager) {
        windowManager->showNowPlaying();
    }

    setState(AppState::NOW_PLAYING);
}

void App::onSpotifyAuthError() {
    Serial.println("❌ Spotify authentication error");
    setState(AppState::AUTH_REQUIRED);

    if (windowManager) {
        windowManager->showAuthScreen();
    }
}

void App::onPlaybackChanged() {
    // Refresh UI
    refreshUI();
}

void App::onTrackChanged() {
    // Refresh UI - track changed means we need to update album art too
    refreshUI();
}

void App::registerEventHandlers() {
    // Already registered in init methods
}

void App::executeScheduledTasks() {
    unsigned long now = millis();

    for (auto it = scheduledTasks.begin(); it != scheduledTasks.end(); ) {
        if (now >= it->executeTime) {
            // Execute task
            it->task();
            // Remove from list
            it = scheduledTasks.erase(it);
        } else {
            ++it;
        }
    }
}
