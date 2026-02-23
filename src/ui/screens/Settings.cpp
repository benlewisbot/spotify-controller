/**
 * @file SettingsScreen.cpp
 * @brief Main Settings Screen Implementation
 *
 * Provides the main settings menu with navigation to sub-settings.
 */

#include "Settings.hpp"
#include "MenuManager.hpp"
#include "../../display/themes/SpotifyTheme.hpp"
#include "RuntimeConfig.hpp"

#define MARGIN 16
#define ITEM_HEIGHT 60
#define SECTION_SPACING 8

namespace ui {

SettingsScreen::SettingsScreen(lv_obj_t* parent)
    : screen(nullptr)
    , settingsList(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

SettingsScreen::~SettingsScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void SettingsScreen::createUI() {
    // Header
    lv_obj_t* header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_PCT(100), 60);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x181818), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, MARGIN, 0);

    // Back button (hidden on main settings - goes back to Now Playing) with event handler
    lv_obj_t* backBtn = lv_button_create(header);
    lv_obj_set_size(backBtn, 40, 40);
    lv_obj_set_style_bg_opa(backBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(backBtn, 0, 0);
    lv_obj_set_style_radius(backBtn, 20, 0);

    lv_obj_t* backLabel = lv_label_create(backBtn);
    lv_obj_center(backLabel);
    lv_obj_set_style_text_font(backLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(backLabel, lv_color_white(), 0);
    lv_label_set_text_static(backLabel, LV_SYMBOL_LEFT);

    // Add click handler for back button
    lv_obj_add_event_cb(backBtn, [](lv_event_t* e) {
        auto& menuManager = MenuManager::getInstance();
        menuManager.goBack();
    }, LV_EVENT_CLICKED, nullptr);

    // Title
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_label_set_text_static(titleLabel, "Settings");

    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);

    // Settings container
    lv_obj_t* container = lv_obj_create(screen);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100) - 60);
    lv_obj_align(container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, MARGIN, 0);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_AUTO);

    int yPos = 0;

    // WiFi Section
    createSection(container, "Network", yPos);
    yPos += ITEM_HEIGHT + SECTION_SPACING;
    createNavigationItem(container, "WiFi Settings", "Configure network", yPos, ScreenType::WIFI_SETTINGS);
    yPos += ITEM_HEIGHT;

    // Display Section
    yPos += SECTION_SPACING;
    createSection(container, "Display", yPos);
    yPos += ITEM_HEIGHT + SECTION_SPACING;
    createNavigationItem(container, "Display Settings", "Brightness, orientation", yPos, ScreenType::DISPLAY_SETTINGS);
    yPos += ITEM_HEIGHT;

    // Spotify Section
    yPos += SECTION_SPACING;
    createSection(container, "Spotify", yPos);
    yPos += ITEM_HEIGHT + SECTION_SPACING;
    createNavigationItem(container, "Spotify Account", "Token management", yPos, ScreenType::SPOTIFY_TOKEN);
    yPos += ITEM_HEIGHT;

    // System Section
    yPos += SECTION_SPACING;
    createSection(container, "System", yPos);
    yPos += ITEM_HEIGHT + SECTION_SPACING;
    createNavigationItem(container, "About", "Device info & firmware", yPos, ScreenType::ABOUT);
    yPos += ITEM_HEIGHT;

    // Register screen with MenuManager
    auto& menuManager = MenuManager::getInstance();
    menuManager.registerScreen(ScreenType::SETTINGS_MAIN, screen);
}

void SettingsScreen::createSection(lv_obj_t* parent, const String& title, int yPos) {
    lv_obj_t* section = lv_obj_create(parent);
    lv_obj_set_size(section, LV_PCT(100) - (MARGIN * 2), 30);
    lv_obj_set_pos(section, MARGIN, yPos);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(section, 0, 0);
    lv_obj_set_style_pad_all(section, 0, 0);

    lv_obj_t* label = lv_label_create(section);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x727272), 0);
    lv_label_set_text(label, title.c_str());
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
}

void SettingsScreen::createNavigationItem(lv_obj_t* parent, const String& label, 
                                         const String& subtitle, int yPos, ScreenType targetScreen) {
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(btn, MARGIN, yPos);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_pad_all(btn, 16, 0);

    // Label
    lv_obj_t* labelObj = lv_label_create(btn);
    lv_obj_align(labelObj, LV_ALIGN_LEFT_MID, 0, -8);
    lv_obj_set_style_text_font(labelObj, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(labelObj, lv_color_white(), 0);
    lv_label_set_text(labelObj, label.c_str());

    // Subtitle
    lv_obj_t* subtitleObj = lv_label_create(btn);
    lv_obj_align(subtitleObj, LV_ALIGN_LEFT_MID, 0, 10);
    lv_obj_set_style_text_font(subtitleObj, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(subtitleObj, lv_color_hex(0x727272), 0);
    lv_label_set_text(subtitleObj, subtitle.c_str());

    // Arrow icon
    lv_obj_t* arrow = lv_label_create(btn);
    lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_font(arrow, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(arrow, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(arrow, LV_SYMBOL_RIGHT);
}

void SettingsScreen::updateWiFiStatus(const String& ssid, bool connected) {
    // Update WiFi status display when implemented
    // For now, this is a placeholder
}

void SettingsScreen::updateSpotifyAccount(const String& email) {
    // Update Spotify account display when implemented
}

} // namespace ui
