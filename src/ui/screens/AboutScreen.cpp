/**
 * @file AboutScreen.cpp
 * @brief About Screen Implementation
 */

#include "AboutScreen.hpp"
#include "MenuManager.hpp"
#include "../../display/themes/SpotifyTheme.hpp"

#define MARGIN 16
#define ITEM_HEIGHT 50

namespace ui {

AboutScreen::AboutScreen(lv_obj_t* parent)
    : screen(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
    
    // Register with MenuManager
    auto& menuManager = MenuManager::getInstance();
    menuManager.registerScreen(ScreenType::ABOUT, screen);
}

AboutScreen::~AboutScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void AboutScreen::createUI() {
    createHeader();
    createLogoSection();
    createDeviceInfoSection();
    createFirmwareSection();
    createActionButtons();
}

void AboutScreen::createHeader() {
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
    lv_label_set_text_static(titleLabel, "About");

    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);
}

void AboutScreen::createLogoSection() {
    lv_obj_t* card = lv_obj_create(screen);
    lv_obj_set_size(card, LV_PCT(100) - (MARGIN * 2), 120);
    lv_obj_set_pos(card, MARGIN, 60);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, MARGIN, 0);

    // Spotify-style logo
    lv_obj_t* logo = lv_label_create(card);
    lv_obj_set_size(logo, 80, 80);
    lv_obj_align(logo, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(logo, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(logo, lv_color_hex(0x1DB954), 0);
    lv_label_set_text_static(logo, LV_SYMBOL_AUDIO);

    // App name
    lv_obj_t* appName = lv_label_create(card);
    lv_obj_align(appName, LV_ALIGN_LEFT_MID, 90, -15);
    lv_obj_set_style_text_font(appName, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(appName, lv_color_white(), 0);
    lv_label_set_text_static(appName, "Spotify Controller");

    // Version
    lv_obj_t* version = lv_label_create(card);
    lv_obj_align(version, LV_ALIGN_LEFT_MID, 90, 15);
    lv_obj_set_style_text_font(version, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(version, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(version, "Version 1.0.0");
}

void AboutScreen::createDeviceInfoSection() {
    // Section label
    lv_obj_t* sectionLabel = lv_label_create(screen);
    lv_obj_set_pos(sectionLabel, MARGIN, 188);
    lv_obj_set_style_text_font(sectionLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(sectionLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(sectionLabel, "Device Information");

    // Device ID card
    lv_obj_t* card = lv_obj_create(screen);
    lv_obj_set_size(card, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(card, MARGIN, 208);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, MARGIN, 0);

    // Icon
    lv_obj_t* icon = lv_label_create(card);
    lv_obj_set_size(icon, 40, 40);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(icon, LV_SYMBOL_SETTINGS);

    // Label
    lv_obj_t* label = lv_label_create(card);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 50, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text_static(label, "Device ID");

    // Device ID value
    deviceIdLabel = lv_label_create(card);
    lv_obj_align(deviceIdLabel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_font(deviceIdLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(deviceIdLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(deviceIdLabel, "ESP32-S3");
}

void AboutScreen::createFirmwareSection() {
    // Firmware info card
    lv_obj_t* card = lv_obj_create(screen);
    lv_obj_set_size(card, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(card, MARGIN, 266);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, MARGIN, 0);

    // Icon
    lv_obj_t* icon = lv_label_create(card);
    lv_obj_set_size(icon, 40, 40);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(icon, LV_SYMBOL_SD_CARD);

    // Label
    lv_obj_t* label = lv_label_create(card);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 50, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text_static(label, "Firmware");

    firmwareLabel = lv_label_create(card);
    lv_obj_align(firmwareLabel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_font(firmwareLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(firmwareLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(firmwareLabel, "1.0.0");

    // Build info card
    lv_obj_t* buildCard = lv_obj_create(screen);
    lv_obj_set_size(buildCard, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(buildCard, MARGIN, 324);
    lv_obj_set_style_bg_color(buildCard, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(buildCard, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(buildCard, 12, 0);
    lv_obj_set_style_border_width(buildCard, 0, 0);
    lv_obj_set_style_pad_all(buildCard, MARGIN, 0);

    // Icon
    lv_obj_t* buildIcon = lv_label_create(buildCard);
    lv_obj_set_size(buildIcon, 40, 40);
    lv_obj_align(buildIcon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(buildIcon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(buildIcon, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(buildIcon, LV_SYMBOL_NEW_LINE);

    // Label
    lv_obj_t* buildLabel = lv_label_create(buildCard);
    lv_obj_align(buildLabel, LV_ALIGN_LEFT_MID, 50, 0);
    lv_obj_set_style_text_font(buildLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(buildLabel, lv_color_white(), 0);
    lv_label_set_text_static(buildLabel, "Build Date");

    buildLabel = lv_label_create(buildCard);
    lv_obj_align(buildLabel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_font(buildLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(buildLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text(buildLabel, __DATE__);
}

void AboutScreen::createActionButtons() {
    // System info button
    lv_obj_t* sysInfoBtn = lv_button_create(screen);
    lv_obj_set_size(sysInfoBtn, LV_PCT(100) - (MARGIN * 2), 50);
    lv_obj_set_pos(sysInfoBtn, MARGIN, 392);
    lv_obj_set_style_bg_color(sysInfoBtn, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(sysInfoBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(sysInfoBtn, 12, 0);
    lv_obj_set_style_border_width(sysInfoBtn, 0, 0);

    lv_obj_t* sysInfoLabel = lv_label_create(sysInfoBtn);
    lv_obj_center(sysInfoLabel);
    lv_obj_set_style_text_font(sysInfoLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(sysInfoLabel, lv_color_white(), 0);
    lv_label_set_text_static(sysInfoLabel, LV_SYMBOL_HOME "  System Information");

    // Legal/About button
    lv_obj_t* legalBtn = lv_button_create(screen);
    lv_obj_set_size(legalBtn, LV_PCT(100) - (MARGIN * 2), 50);
    lv_obj_set_pos(legalBtn, MARGIN, 454);
    lv_obj_set_style_bg_color(legalBtn, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(legalBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(legalBtn, 12, 0);
    lv_obj_set_style_border_width(legalBtn, 0, 0);

    lv_obj_t* legalLabel = lv_label_create(legalBtn);
    lv_obj_center(legalLabel);
    lv_obj_set_style_text_font(legalLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(legalLabel, lv_color_white(), 0);
    lv_label_set_text_static(legalLabel, LV_SYMBOL_LIST "  Legal & Licenses");
}

void AboutScreen::updateDeviceInfo() {
    auto& config = RuntimeConfigManager::getInstance().getConfig();
    
    // Update device ID from config
    if (!config.system.deviceId.isEmpty()) {
        lv_label_set_text(deviceIdLabel, config.system.deviceId.c_str());
    }
    
    // Update firmware info
    lv_label_set_text(firmwareLabel, config.system.firmwareVersion.c_str());
}

void AboutScreen::showFirmwareInfo() {
    auto& config = RuntimeConfigManager::getInstance().getConfig();
    auto& menuManager = MenuManager::getInstance();
    
    String message = "Version: ";
    message += config.system.firmwareVersion;
    message += "\nBuild: ";
    message += config.system.buildDate;
    message += "\nBoot Count: ";
    message += String(config.system.bootCount);
    
    menuManager.showSuccess(message);
}

void AboutScreen::showSystemInfo() {
    auto& menuManager = MenuManager::getInstance();
    
    String message = "Chip: ";
    message += getChipModel();
    message += "\nFlash: ";
    message += getFlashSize();
    message += "\nTotal Heap: ";
    message += getTotalHeap();
    message += "\nFree Heap: ";
    message += getFreeHeap();
    message += "\nUptime: ";
    message += getUptime();
    
    menuManager.showSuccess(message);
}

String AboutScreen::getChipModel() {
#if CONFIG_IDF_TARGET_ESP32S3
    return "ESP32-S3";
#elif CONFIG_IDF_TARGET_ESP32
    return "ESP32";
#elif CONFIG_IDF_TARGET_ESP32C3
    return "ESP32-C3";
#else
    return "Unknown";
#endif
}

String AboutScreen::getFlashSize() {
    return String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB";
}

String AboutScreen::getTotalHeap() {
    return String(ESP.getHeapSize() / 1024) + " KB";
}

String AboutScreen::getFreeHeap() {
    return String(ESP.getFreeHeap() / 1024) + " KB";
}

String AboutScreen::getUptime() {
    unsigned long uptime = millis() / 1000;
    
    int hours = uptime / 3600;
    int minutes = (uptime % 3600) / 60;
    int seconds = uptime % 60;
    
    String result = "";
    if (hours > 0) {
        result += String(hours) + "h ";
    }
    if (minutes > 0 || hours > 0) {
        result += String(minutes) + "m ";
    }
    result += String(seconds) + "s";
    
    return result;
}

} // namespace ui
