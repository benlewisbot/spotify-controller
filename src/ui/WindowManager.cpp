/**
 * @file WindowManager.cpp
 * @brief Window Manager Implementation
 */

#include "WindowManager.hpp"
#include "screens/NowPlaying.hpp"
#include "screens/Auth.hpp"
#include "screens/Settings.hpp"

WindowManager::WindowManager(DisplayManager* dm)
    : displayManager(dm)
    , root(nullptr)
    , currentScreen(nullptr)
    , nowPlayingScreen(nullptr)
    , authScreen(nullptr)
    , settingsScreen(nullptr)
    , initialized(false) {
}

WindowManager::~WindowManager() {
    delete nowPlayingScreen;
    delete authScreen;
    delete settingsScreen;
}

void WindowManager::init() {
    if (initialized) {
        return;
    }

    Serial.println("🖼  Initializing WindowManager...");

    // Apply theme
    extern void spotify_theme_apply();
    spotify_theme_apply();

    // Create root object
    root = lv_obj_create(NULL);
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(root, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);

    // Load screen
    lv_scr_load(root);

    initialized = true;
    Serial.println("✅ WindowManager initialized");
}

void WindowManager::update() {
    // LVGL tasks are handled by DisplayManager
    if (nowPlayingScreen) {
        nowPlayingScreen->update();
    }
}

void WindowManager::showNowPlaying() {
    Serial.println("🖼  Showing Now Playing screen");

    if (!nowPlayingScreen) {
        nowPlayingScreen = new ui::NowPlayingScreen(root);
    }

    transitionTo(nowPlayingScreen->getScreen());
}

void WindowManager::showAuthScreen() {
    Serial.println("🖼  Showing Authentication screen");

    if (!authScreen) {
        authScreen = new ui::AuthScreen(root);
    }

    transitionTo(authScreen->getScreen());
}

void WindowManager::showSettings() {
    Serial.println("🖼  Showing Settings screen");

    if (!settingsScreen) {
        settingsScreen = new ui::SettingsScreen(root);
    }

    transitionTo(settingsScreen->getScreen());
}

void WindowManager::goBack() {
    // Simple navigation: always go back to Now Playing
    showNowPlaying();
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
        lv_obj_del(currentScreen);
        currentScreen = nullptr;
    }
}

void WindowManager::transitionTo(lv_obj_t* newScreen) {
    // Simple fade transition
    if (currentScreen && currentScreen != newScreen) {
        lv_obj_add_flag(currentScreen, LV_OBJ_FLAG_HIDDEN);
    }

    if (newScreen) {
        lv_obj_clear_flag(newScreen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(newScreen);
    }

    currentScreen = newScreen;
}
