/**
 * @file WindowManager.cpp
 * @brief Window Manager Implementation
 * 
 * Manages all UI screens: NowPlaying, Auth, Settings + sub-screens.
 * Handles navigation between screens via MenuManager.
 */

#include "WindowManager.hpp"
#include "screens/NowPlaying.hpp"
#include "screens/Auth.hpp"
#include "screens/Settings.hpp"
#include "screens/WiFiSettingsScreen.hpp"
#include "screens/SpotifyTokenScreen.hpp"
#include "screens/DisplaySettingsScreen.hpp"
#include "screens/AboutScreen.hpp"

WindowManager::WindowManager(DisplayManager* dm)
    : displayManager(dm)
    , root(nullptr)
    , currentScreen(nullptr)
    , nowPlayingScreen(nullptr)
    , authScreen(nullptr)
    , settingsScreen(nullptr)
    , wifiSettingsScreen(nullptr)
    , spotifyTokenScreen(nullptr)
    , displaySettingsScreen(nullptr)
    , aboutScreen(nullptr)
    , menuManager(nullptr)
    , initialized(false) {
}

WindowManager::~WindowManager() {
    delete nowPlayingScreen;
    delete authScreen;
    delete settingsScreen;
    delete wifiSettingsScreen;
    delete spotifyTokenScreen;
    delete displaySettingsScreen;
    delete aboutScreen;
}

void WindowManager::init() {
    if (initialized) return;
    
    root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(root, 0, 0);
    lv_obj_set_style_bg_color(root, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_scrollbar_mode(root, LV_SCROLLBAR_MODE_OFF);

    // Initialize MenuManager
    menuManager = &MenuManager::getInstance();
    menuManager->init(root);

    // Set back button callback to return to NowPlaying when at settings root
    menuManager->setBackButtonCallback([this]() {
        showNowPlaying();
    });

    // Initialize all settings sub-screens
    initSettingsScreens();

    initialized = true;
}

void WindowManager::initSettingsScreens() {
    // Create settings screens - they register themselves with MenuManager
    settingsScreen = new ui::SettingsScreen(root);
    lv_obj_add_flag(settingsScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);

    wifiSettingsScreen = new ui::WiFiSettingsScreen(root);
    lv_obj_add_flag(wifiSettingsScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    menuManager->registerScreen(ScreenType::WIFI_SETTINGS, wifiSettingsScreen->getScreen());

    spotifyTokenScreen = new ui::SpotifyTokenScreen(root);
    lv_obj_add_flag(spotifyTokenScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    menuManager->registerScreen(ScreenType::SPOTIFY_TOKEN, spotifyTokenScreen->getScreen());

    displaySettingsScreen = new ui::DisplaySettingsScreen(root);
    lv_obj_add_flag(displaySettingsScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    menuManager->registerScreen(ScreenType::DISPLAY_SETTINGS, displaySettingsScreen->getScreen());

    aboutScreen = new ui::AboutScreen(root);
    lv_obj_add_flag(aboutScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    menuManager->registerScreen(ScreenType::ABOUT, aboutScreen->getScreen());
}

void WindowManager::update() {
    if (!initialized) return;

    // Update display manager (LVGL timer handler)
    if (displayManager) {
        displayManager->update();
    }

    // Update NowPlaying screen if visible
    if (nowPlayingScreen && currentScreen == nowPlayingScreen->getScreen()) {
        nowPlayingScreen->update();
    }
}

void WindowManager::showNowPlaying() {
    // Hide all settings screens
    if (settingsScreen) lv_obj_add_flag(settingsScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    if (wifiSettingsScreen) lv_obj_add_flag(wifiSettingsScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    if (spotifyTokenScreen) lv_obj_add_flag(spotifyTokenScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    if (displaySettingsScreen) lv_obj_add_flag(displaySettingsScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    if (aboutScreen) lv_obj_add_flag(aboutScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    if (authScreen) lv_obj_add_flag(authScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);

    if (!nowPlayingScreen) {
        nowPlayingScreen = new ui::NowPlayingScreen(root);
        // Wire menu button to show settings
        nowPlayingScreen->onMenuPressed = [this]() {
            showSettings();
        };
    }

    lv_obj_clear_flag(nowPlayingScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(nowPlayingScreen->getScreen());
    currentScreen = nowPlayingScreen->getScreen();

    // Show placeholder track info
    SpotifyClient::TrackInfo track;
    track.title = "Ready to Play";
    track.artist = "Connect to Spotify";
    track.durationMs = 0;
    track.progressMs = 0;
    track.isPlaying = false;
    nowPlayingScreen->updateTrackInfo(track);
}

void WindowManager::showAuthScreen() {
    clearScreen();
    
    if (!authScreen) {
        authScreen = new ui::AuthScreen(root);
    }
    lv_obj_clear_flag(authScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(authScreen->getScreen());
    currentScreen = authScreen->getScreen();
    authScreen->setStatus("Please authenticate with Spotify");
}

void WindowManager::showSettings() {
    // Hide NowPlaying
    if (nowPlayingScreen) {
        lv_obj_add_flag(nowPlayingScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    }
    if (authScreen) {
        lv_obj_add_flag(authScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
    }

    // Show settings main via MenuManager
    if (menuManager && settingsScreen) {
        menuManager->clearHistory();
        lv_obj_clear_flag(settingsScreen->getScreen(), LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(settingsScreen->getScreen());
        currentScreen = settingsScreen->getScreen();
    }
}

void WindowManager::goBack() {
    if (menuManager && menuManager->canGoBack()) {
        menuManager->goBack();
    } else {
        showNowPlaying();
    }
}

lv_obj_t* WindowManager::createScreen() {
    lv_obj_t* screen = lv_obj_create(root);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(screen, 0, 0);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
    return screen;
}

void WindowManager::clearScreen() {
    if (currentScreen) {
        lv_obj_add_flag(currentScreen, LV_OBJ_FLAG_HIDDEN);
    }
    currentScreen = nullptr;
}

void WindowManager::transitionTo(lv_obj_t* newScreen) {
    if (currentScreen && currentScreen != newScreen) {
        lv_obj_add_flag(currentScreen, LV_OBJ_FLAG_HIDDEN);
    }
    if (newScreen) {
        lv_obj_clear_flag(newScreen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(newScreen);
    }
    currentScreen = newScreen;
}
