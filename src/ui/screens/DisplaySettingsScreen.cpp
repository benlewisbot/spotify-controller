/**
 * @file DisplaySettingsScreen.cpp
 * @brief Display Settings Screen Implementation
 */

#include "DisplaySettingsScreen.hpp"
#include "MenuManager.hpp"
#include "../../display/themes/SpotifyTheme.hpp"

#define MARGIN 16
#define ITEM_HEIGHT 60

namespace ui {

DisplaySettingsScreen::DisplaySettingsScreen(lv_obj_t* parent)
    : screen(nullptr)
    , currentBrightness(75)
    , currentOrientation(0)
    , screensaverEnabled(true)
    , screensaverTimeout(5) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
    
    // Register with MenuManager
    auto& menuManager = MenuManager::getInstance();
    menuManager.registerScreen(ScreenType::DISPLAY_SETTINGS, screen);
}

DisplaySettingsScreen::~DisplaySettingsScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void DisplaySettingsScreen::createUI() {
    createHeader();
    createBrightnessSection();
    createOrientationSection();
    createScreensaverSection();
    createOtherSection();
}

void DisplaySettingsScreen::createHeader() {
    lv_obj_t* header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_PCT(100), 60);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x181818), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, MARGIN, 0);

    // Back button with event handler
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
    lv_label_set_text_static(titleLabel, "Display Settings");

    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);
}

void DisplaySettingsScreen::createBrightnessSection() {
    // Section label
    lv_obj_t* sectionLabel = lv_label_create(screen);
    lv_obj_set_pos(sectionLabel, MARGIN, 70);
    lv_obj_set_style_text_font(sectionLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(sectionLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(sectionLabel, "Brightness");

    // Brightness card
    lv_obj_t* card = lv_obj_create(screen);
    lv_obj_set_size(card, LV_PCT(100) - (MARGIN * 2), 100);
    lv_obj_set_pos(card, MARGIN, 90);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, MARGIN, 0);

    // Brightness icon
    lv_obj_t* icon = lv_label_create(card);
    lv_obj_set_size(icon, 40, 40);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFEB3B), 0);
    lv_label_set_text_static(icon, LV_SYMBOL_IMAGE);  // No LV_SYMBOL_BRIGHTNESS in LVGL 9

    // Brightness label
    brightnessLabel = lv_label_create(card);
    lv_obj_align(brightnessLabel, LV_ALIGN_LEFT_MID, 50, -8);
    lv_obj_set_style_text_font(brightnessLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(brightnessLabel, lv_color_white(), 0);
    lv_label_set_text_fmt(brightnessLabel, "Brightness: %d%%", currentBrightness);

    // Brightness slider
    brightnessSlider = lv_slider_create(card);
    lv_obj_set_size(brightnessSlider, 200, 30);
    lv_obj_align(brightnessSlider, LV_ALIGN_LEFT_MID, 50, 20);
    lv_slider_set_range(brightnessSlider, 0, 100);
    lv_slider_set_value(brightnessSlider, currentBrightness, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(brightnessSlider, lv_color_hex(0x1DB954), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(brightnessSlider, lv_color_hex(0x404040), LV_PART_MAIN);

    // Slider event handler with correct LVGL callback signature (lv_event_t*)
    lv_obj_set_user_data(brightnessSlider, this);
    lv_obj_add_event_cb(brightnessSlider, [](lv_event_t* e) {
        lv_obj_t* obj = static_cast<lv_obj_t*>(lv_event_get_target(e));
        DisplaySettingsScreen* screen = static_cast<DisplaySettingsScreen*>(lv_event_get_user_data(e));
        if (screen) {
            int32_t value = lv_slider_get_value(obj);
            screen->onBrightnessChanged(obj, value);
        }
    }, LV_EVENT_VALUE_CHANGED, this);
}

void DisplaySettingsScreen::createOrientationSection() {
    // Section label
    lv_obj_t* sectionLabel = lv_label_create(screen);
    lv_obj_set_pos(sectionLabel, MARGIN, 198);
    lv_obj_set_style_text_font(sectionLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(sectionLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(sectionLabel, "Orientation");

    // Orientation card
    lv_obj_t* card = lv_obj_create(screen);
    lv_obj_set_size(card, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(card, MARGIN, 218);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, MARGIN, 0);

    // Orientation icon
    lv_obj_t* icon = lv_label_create(card);
    lv_obj_set_size(icon, 40, 40);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(icon, LV_SYMBOL_IMAGE);

    // Orientation label
    orientationLabel = lv_label_create(card);
    lv_obj_align(orientationLabel, LV_ALIGN_LEFT_MID, 50, 0);
    lv_obj_set_style_text_font(orientationLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(orientationLabel, lv_color_white(), 0);
    lv_label_set_text(orientationLabel, getOrientationName(currentOrientation));

    // Change button
    orientationBtn = lv_button_create(card);
    lv_obj_set_size(orientationBtn, 100, 36);
    lv_obj_align(orientationBtn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(orientationBtn, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_bg_opa(orientationBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(orientationBtn, 8, 0);
    lv_obj_set_style_border_width(orientationBtn, 0, 0);

    lv_obj_t* btnLabel = lv_label_create(orientationBtn);
    lv_obj_center(btnLabel);
    lv_obj_set_style_text_font(btnLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(btnLabel, lv_color_white(), 0);
    lv_label_set_text_static(btnLabel, "Change");
}

void DisplaySettingsScreen::createScreensaverSection() {
    // Section label
    lv_obj_t* sectionLabel = lv_label_create(screen);
    lv_obj_set_pos(sectionLabel, MARGIN, 294);
    lv_obj_set_style_text_font(sectionLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(sectionLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(sectionLabel, "Screensaver");

    // Screensaver enabled card
    lv_obj_t* card = lv_obj_create(screen);
    lv_obj_set_size(card, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(card, MARGIN, 314);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, MARGIN, 0);

    // Screensaver icon
    lv_obj_t* icon = lv_label_create(card);
    lv_obj_set_size(icon, 40, 40);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(icon, LV_SYMBOL_IMAGE);

    // Label
    lv_obj_t* label = lv_label_create(card);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 50, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text_static(label, "Enable Screensaver");

    // Switch (LVGL 9: lv_switch_create)
    screensaverSwitch = lv_switch_create(card);
    lv_obj_align(screensaverSwitch, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(screensaverSwitch, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_bg_color(screensaverSwitch, lv_color_hex(0x1DB954), LV_PART_INDICATOR);

    if (screensaverEnabled) {
        lv_obj_add_state(screensaverSwitch, LV_STATE_CHECKED);
    }

    // Screensaver timeout card
    lv_obj_t* timeoutCard = lv_obj_create(screen);
    lv_obj_set_size(timeoutCard, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(timeoutCard, MARGIN, 382);
    lv_obj_set_style_bg_color(timeoutCard, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(timeoutCard, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(timeoutCard, 12, 0);
    lv_obj_set_style_border_width(timeoutCard, 0, 0);
    lv_obj_set_style_pad_all(timeoutCard, MARGIN, 0);

    // Timeout icon
    lv_obj_t* timeoutIcon = lv_label_create(timeoutCard);
    lv_obj_set_size(timeoutIcon, 40, 40);
    lv_obj_align(timeoutIcon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(timeoutIcon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(timeoutIcon, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(timeoutIcon, LV_SYMBOL_REFRESH);

    // Label
    lv_obj_t* timeoutLabel = lv_label_create(timeoutCard);
    lv_obj_align(timeoutLabel, LV_ALIGN_LEFT_MID, 50, 0);
    lv_obj_set_style_text_font(timeoutLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(timeoutLabel, lv_color_white(), 0);
    lv_label_set_text_static(timeoutLabel, "Timeout");

    // Timeout value (use class member timeoutLabel, not shadowing)
    lv_obj_t* timeoutValueLabel = lv_label_create(timeoutCard);
    lv_obj_align(timeoutValueLabel, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_text_font(timeoutValueLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(timeoutValueLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_fmt(timeoutValueLabel, "%d min", screensaverTimeout);

    // Change button
    screensaverTimeoutBtn = lv_button_create(timeoutCard);
    lv_obj_set_size(screensaverTimeoutBtn, 80, 36);
    lv_obj_align(screensaverTimeoutBtn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(screensaverTimeoutBtn, lv_color_hex(0x404040), 0);
    lv_obj_set_style_bg_opa(screensaverTimeoutBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(screensaverTimeoutBtn, 8, 0);
    lv_obj_set_style_border_width(screensaverTimeoutBtn, 0, 0);

    lv_obj_t* btnLabel2 = lv_label_create(screensaverTimeoutBtn);
    lv_obj_center(btnLabel2);
    lv_obj_set_style_text_font(btnLabel2, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(btnLabel2, lv_color_white(), 0);
    lv_label_set_text_static(btnLabel2, "Change");
}

void DisplaySettingsScreen::createOtherSection() {
    // Additional display settings can go here
}

void DisplaySettingsScreen::onBrightnessChanged(lv_obj_t* slider, int32_t value) {
    currentBrightness = value;
    lv_label_set_text_fmt(brightnessLabel, "Brightness: %d%%", currentBrightness);
    
    // Apply brightness change
    auto& config = RuntimeConfigManager::getInstance().getConfig();
    config.display.brightness = currentBrightness;
    RuntimeConfigManager::getInstance().save();
    
    // Update display brightness
    // In real implementation, call display manager
}

void DisplaySettingsScreen::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    if (brightnessSlider) {
        lv_slider_set_value(brightnessSlider, brightness, LV_ANIM_OFF);
    }
    if (brightnessLabel) {
        lv_label_set_text_fmt(brightnessLabel, "Brightness: %d%%", currentBrightness);
    }
}

void DisplaySettingsScreen::setOrientation(uint8_t rotation) {
    currentOrientation = rotation;
    if (orientationLabel) {
        lv_label_set_text(orientationLabel, getOrientationName(currentOrientation));
    }
}

void DisplaySettingsScreen::setScreensaverEnabled(bool enabled) {
    screensaverEnabled = enabled;
    if (screensaverSwitch) {
        if (enabled) {
            lv_obj_add_state(screensaverSwitch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(screensaverSwitch, LV_STATE_CHECKED);
        }
    }
}

void DisplaySettingsScreen::setScreensaverTimeout(uint16_t minutes) {
    screensaverTimeout = minutes;
    if (timeoutLabel) {
        lv_label_set_text_fmt(timeoutLabel, "%d min", screensaverTimeout);
    }
}

const char* DisplaySettingsScreen::getOrientationName(uint8_t rotation) {
    switch (rotation) {
        case 0: return "Portrait";
        case 1: return "Landscape";
        case 2: return "Inverted Portrait";
        case 3: return "Inverted Landscape";
        default: return "Portrait";
    }
}

} // namespace ui
