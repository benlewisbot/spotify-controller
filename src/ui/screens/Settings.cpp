/**
 * @file Settings.cpp
 * @brief Settings Screen Implementation
 */

#include "Settings.hpp"
#include "../display/themes/SpotifyTheme.hpp"

#define MARGIN 16

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
    lv_obj_set_size(header, LV_PCT(100), 50);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, MARGIN, 0);

    // Back button
    lv_obj_t* backBtn = lv_button_create(header);
    lv_obj_set_size(backBtn, 40, 40);
    lv_obj_set_style_bg_opa(backBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(backBtn, 0, 0);

    lv_obj_t* backLabel = lv_label_create(backBtn);
    lv_obj_center(backLabel);
    lv_obj_set_style_text_font(backLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(backLabel, lv_color_white(), 0);
    lv_label_set_text_static(backLabel, LV_SYMBOL_LEFT);

    // Title
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_label_set_text_static(titleLabel, "Settings");

    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, 0);

    // Settings list
    settingsList = lv_list_create(screen);
    lv_obj_set_size(settingsList, LV_PCT(100), LV_PCT(100) - 50);
    lv_obj_align(settingsList, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(settingsList, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(settingsList, 0, 0);

    // WiFi Section
    lv_obj_t* wifiSection = lv_list_add_text(settingsList, "WiFi");
    lv_obj_set_style_text_font(wifiSection, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(wifiSection, lv_color_hex(0x727272), 0);

    createSettingItem("WiFi Status", "Connected");
    createSettingItem("Network", "HomeNetwork");

    // Display Section
    lv_obj_t* displaySection = lv_list_add_text(settingsList, "Display");
    lv_obj_set_style_text_font(displaySection, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(displaySection, lv_color_hex(0x727272), 0);

    createSettingItem("Orientation", "Landscape");
    createSettingItem("Brightness", "75%");
    createSettingItem("Screensaver", "5 min", true);

    // Spotify Section
    lv_obj_t* spotifySection = lv_list_add_text(settingsList, "Spotify");
    lv_obj_set_style_text_font(spotifySection, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(spotifySection, lv_color_hex(0x727272), 0);

    createSettingItem("Account", "user@email.com");
    createSettingItem("Reconnect", "Tap to reconnect");

    // Device Section
    lv_obj_t* deviceSection = lv_list_add_text(settingsList, "Device");
    lv_obj_set_style_text_font(deviceSection, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(deviceSection, lv_color_hex(0x727272), 0);

    createSettingItem("Device ID", "ESPXXXXXXXXXX");
    createSettingItem("Version", "1.0.0");
}

void SettingsScreen::createSection(const String& title) {
    lv_obj_t* section = lv_list_add_text(settingsList, title.c_str());
    lv_obj_set_style_text_font(section, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(section, lv_color_hex(0x727272), 0);
}

void SettingsScreen::createSettingItem(const String& label, const String& value, bool withToggle) {
    lv_obj_t* btn = lv_list_add_btn(settingsList, LV_SYMBOL_SETTINGS, nullptr);

    // Get the button's label
    lv_obj_t* itemLabel = lv_obj_get_child(btn, 0);

    // Create a container for the item content
    lv_obj_t* content = lv_obj_create(btn);
    lv_obj_set_size(content, LV_PCT(90), 40);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_pad_all(content, 0, 0);

    // Label
    lv_obj_t* labelObj = lv_label_create(content);
    lv_obj_align(labelObj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(labelObj, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(labelObj, lv_color_white(), 0);
    lv_label_set_text(labelObj, label.c_str());

    // Value or toggle
    if (withToggle) {
        lv_obj_t* toggle = lv_switch_create(content);
        lv_obj_align(toggle, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_style_bg_color(toggle, lv_color_hex(0x1DB954), LV_PART_INDICATOR);
    } else {
        lv_obj_t* valueLabel = lv_label_create(content);
        lv_obj_align(valueLabel, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_style_text_font(valueLabel, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(valueLabel, lv_color_hex(0xB3B3B3), 0);
        lv_label_set_text(valueLabel, value.c_str());
    }
}

} // namespace ui
