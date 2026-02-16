/**
 * @file WindowManager.cpp
 * @brief Window Manager Implementation (Minimal Version)
 */

#include "WindowManager.hpp"

// Disabled for minimal build:
// #include "screens/NowPlaying.hpp"
// #include "screens/Auth.hpp"
// #include "screens/Settings.hpp"
// #include "screens/WiFiSettingsScreen.hpp"
// #include "screens/SpotifyTokenScreen.hpp"
// #include "screens/DisplaySettingsScreen.hpp"
// #include "screens/AboutScreen.hpp"

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
    // Cleanup handled by LVGL
}

void WindowManager::init() {
    if (initialized) return;
    
    // Create root container
    root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(root, 0, 0);
    lv_obj_set_style_bg_color(root, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_scrollbar_mode(root, LV_SCROLLBAR_MODE_OFF);

    // Create a simple placeholder label
    lv_obj_t* label = lv_label_create(root);
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x1DB954), 0);
    lv_label_set_text_static(label, "Spotify Controller");
    
    // Status label below
    lv_obj_t* statusLabel = lv_label_create(root);
    lv_obj_align_to(statusLabel, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(statusLabel, "Minimal build - UI screens disabled");

    initialized = true;
}

void WindowManager::update() {
    // LVGL update is handled by DisplayManager
}

void WindowManager::showNowPlaying() {
    // Disabled for minimal build
}

void WindowManager::showAuthScreen() {
    // Disabled for minimal build
}

void WindowManager::showSettings() {
    // Disabled for minimal build
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
