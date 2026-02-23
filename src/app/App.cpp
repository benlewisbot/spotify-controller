/**
 * @file App.cpp
 * @brief Main Application Controller Implementation
 *
 * Manages initialization, main loop, and subsystem coordination.
 * Boot flow: Config -> Display -> Splash -> WiFi -> Spotify -> UI
 */

#include "App.hpp"
#include <esp_log.h>

// Include subsystem headers
#include "../config/Config.hpp"
#include "../network/WiFiManager.hpp"
#include "../display/DisplayManager.hpp"
#include "../spotify/SpotifyClient.hpp"
#include "../spotify/AuthManager.hpp"
#include "../ui/WindowManager.hpp"
#include "../ui/screens/NowPlaying.hpp"
#include "../ui/screens/SplashScreen.hpp"
#include "RuntimeConfig.hpp"

#include <WiFi.h>

// Splash screen (managed separately since it appears before WindowManager)
static ui::SplashScreen* splashScreen = nullptr;

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
    delete windowManager;
    delete spotifyClient;
    delete authManager;
    // displayManager is a singleton, don't delete
    delete wifiManager;
    // configManager is a singleton (getInstance()), don't delete
    if (splashScreen) {
        delete splashScreen;
        splashScreen = nullptr;
    }
}

bool App::init() {
    ESP_LOGI("APP", "Spotify Controller v1.0.0");

    setState(AppState::INIT);

    // 1. Config
    if (!initConfig()) {
        ESP_LOGE("APP", "Config init failed");
        return false;
    }

    // 2. Logger
    if (!initLogger()) {
        ESP_LOGE("APP", "Logger init failed");
        return false;
    }

    // 3. Display
    if (!initDisplay()) {
        ESP_LOGE("APP", "Display init failed");
        return false;
    }

    // 4. Splash screen
    splashScreen = new ui::SplashScreen(lv_scr_act());
    splashScreen->setStatus("Connecting to WiFi...");
    displayManager->update();

    // 5. WiFi
    splashScreen->setStatus("Connecting to WiFi...");
    displayManager->update();

    if (!initWiFi()) {
        ESP_LOGW("APP", "WiFi failed, starting AP mode");
        splashScreen->setStatus("Starting setup mode...");
        displayManager->update();
    }

    // 6. Spotify
    splashScreen->setStatus("Initializing Spotify...");
    displayManager->update();

    if (!initSpotify()) {
        ESP_LOGE("APP", "Spotify init failed");
        return false;
    }

    // 7. UI
    splashScreen->setStatus("Loading UI...");
    displayManager->update();

    if (!initUI()) {
        ESP_LOGE("APP", "UI init failed");
        return false;
    }

    // 8. Hide splash, show main screen
    splashScreen->hide();
    registerEventHandlers();

    initialized = true;
    setState(AppState::READY);
    ESP_LOGI("APP", "Ready");

    return true;
}

void App::loop() {
    if (!initialized) {
        return;
    }

    // Execute scheduled tasks
    executeScheduledTasks();

    // Update window manager (LVGL tasks + NowPlaying polling)
    if (windowManager) {
        windowManager->update();
    }

    // Poll WiFi status
    if (wifiManager) {
        wifiManager->update();
    }

    // Update auth manager (handles captive portal + OAuth web server)
    if (authManager) {
        authManager->update();

        // Detect WiFi connection during captive portal setup
        static bool wasConnected = false;
        bool nowConnected = (WiFi.status() == WL_CONNECTED);
        if (nowConnected && !wasConnected) {
            authManager->onWiFiConnected();
        }
        wasConnected = nowConnected;

        // Check if auth completed
        if (state == AppState::AUTH_REQUIRED && authManager->isAuthenticated()) {
            onSpotifyAuthenticated();
        }
    }

    // Poll Spotify status (periodically)
    if (spotifyClient && state == AppState::NOW_PLAYING) {
        static unsigned long lastPoll = 0;
        unsigned long now = millis();
        if (now - lastPoll >= SPOTIFY_POLL_INTERVAL_MS) {
            spotifyClient->updateNowPlaying();
            lastPoll = now;
        }
    }
}

void App::setState(AppState newState) {
    if (state != newState) {
        ESP_LOGI("APP", "State: %d -> %d", (int)state, (int)newState);
        state = newState;
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

// ===== Initialization Methods =====

bool App::initConfig() {
    // Use the singleton instance — DisplayManager and other code also use
    // ConfigManager::getInstance(), so we must use the same object.
    configManager = &ConfigManager::getInstance();
    bool configOk = configManager->init();
    bool runtimeConfigOk = RuntimeConfigManager::getInstance().begin();
    return configOk && runtimeConfigOk;
}

bool App::initLogger() {
    return true;
}

bool App::initWiFi() {
    wifiManager = new WiFiManager();

    eventBus.subscribe(EventType::WIFI_CONNECTED,
        [this](const Event& e) { this->onWiFiConnected(); });

    eventBus.subscribe(EventType::WIFI_DISCONNECTED,
        [this](const Event& e) { this->onWiFiDisconnected(); });

    String ssid = configManager->getWiFiSSID();
    String password = configManager->getWiFiPassword();

    // If no WiFi credentials, start AP mode for captive portal setup
    if (ssid.isEmpty()) {
        ESP_LOGI("APP", "No WiFi creds, starting AP mode");
        wifiManager->startAPMode("SpotifyController");
        return true;
    }

    // Try to connect (non-blocking start)
    wifiManager->connect(ssid, password);

    // Wait for connection with splash screen updates
    unsigned long startMs = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startMs < 15000) {
        delay(250);
        if (displayManager) displayManager->update();
    }

    if (WiFi.status() == WL_CONNECTED) {
        ESP_LOGI("APP", "WiFi connected: %s", WiFi.localIP().toString().c_str());
        return true;
    }

    ESP_LOGW("APP", "WiFi timeout, starting AP mode");
    wifiManager->startAPMode("SpotifyController");
    return true;
}

bool App::initDisplay() {
    displayManager = DisplayManager::getInstance();

    if (!displayManager->init()) {
        return false;
    }

    delay(100);
    return true;
}

bool App::initSpotify() {
    authManager = new AuthManager();
    authManager->init(
        configManager->getSpotifyClientId(),
        configManager->getSpotifyClientSecret()
    );

    // Register callbacks for captive portal setup
    authManager->onWiFiCredentials([this](const String& ssid, const String& password) {
        ESP_LOGI("APP", "Setup: WiFi creds received for '%s'", ssid.c_str());
        configManager->setWiFiSSID(ssid);
        configManager->setWiFiPassword(password);
        configManager->save();

        // Connect to WiFi (switch from AP to STA+AP mode temporarily)
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(ssid.c_str(), password.c_str());
    });

    authManager->onClientIdSet([this](const String& id) {
        ESP_LOGI("APP", "Setup: Spotify client ID set");
        configManager->setSpotifyClientId(id);
        configManager->save();
    });

    spotifyClient = new SpotifyClient(authManager);
    spotifyClient->init();

    // Check for stored tokens
    if (configManager->hasStoredTokens()) {
        ESP_LOGI("APP", "Using stored Spotify tokens");
        spotifyClient->setTokens(
            configManager->getAccessToken(),
            configManager->getRefreshToken()
        );
    } else {
        ESP_LOGI("APP", "No tokens, auth required");
        setState(AppState::AUTH_REQUIRED);

        // In AP mode: start captive portal setup; in STA mode: start OAuth
        if (wifiManager && wifiManager->getState() == WiFiState::AP_MODE) {
            authManager->startSetupServer();
        } else {
            authManager->startAuthServer();
        }
    }

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
    windowManager = new WindowManager(displayManager);
    windowManager->init();

    if (state == AppState::AUTH_REQUIRED) {
        windowManager->showAuthScreen();
    } else {
        windowManager->showNowPlaying();
        setState(AppState::NOW_PLAYING);
    }

    return true;
}

// ===== Event Handlers =====

void App::onWiFiConnected() {
    ESP_LOGI("APP", "WiFi connected, IP: %s", WiFi.localIP().toString().c_str());

    if (state == AppState::AUTH_REQUIRED && authManager) {
        authManager->startAuthServer();
    }
}

void App::onWiFiDisconnected() {
    ESP_LOGW("APP", "WiFi disconnected");
}

void App::onSpotifyAuthenticated() {
    ESP_LOGI("APP", "Spotify authenticated");

    // Bridge tokens from AuthManager to SpotifyClient
    if (authManager && spotifyClient) {
        spotifyClient->setTokens(
            authManager->getAccessToken(),
            authManager->getRefreshToken()
        );
    }

    // Save tokens to config
    if (authManager && configManager) {
        configManager->saveTokens(
            authManager->getAccessToken(),
            authManager->getRefreshToken()
        );
    }

    // Stop auth server
    if (authManager) {
        authManager->stopAuthServer();
    }

    // Show now playing
    if (windowManager) {
        windowManager->showNowPlaying();
    }

    setState(AppState::NOW_PLAYING);
}

void App::onSpotifyAuthError() {
    ESP_LOGE("APP", "Spotify auth error");
    setState(AppState::AUTH_REQUIRED);

    if (windowManager) {
        windowManager->showAuthScreen();
    }
}

void App::onPlaybackChanged() {
    refreshUI();
}

void App::onTrackChanged() {
    refreshUI();
}

void App::registerEventHandlers() {
    // Event handlers registered in init methods
}

void App::executeScheduledTasks() {
    unsigned long now = millis();

    for (auto it = scheduledTasks.begin(); it != scheduledTasks.end(); ) {
        if (now >= it->executeTime) {
            it->task();
            it = scheduledTasks.erase(it);
        } else {
            ++it;
        }
    }
}
