/**
 * @file Auth.cpp
 * @brief Authentication Screen Implementation
 */

#include "Auth.hpp"
#include "../../display/themes/SpotifyTheme.hpp"

#define MARGIN 24

ui::AuthScreen::AuthScreen(lv_obj_t* parent)
    : screen(nullptr)
    , statusLabel(nullptr)
    , qrCodeImage(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

ui::AuthScreen::~AuthScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void ui::AuthScreen::createUI() {
    // Title
    lv_obj_t* titleLabel = lv_label_create(screen);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, MARGIN + 10);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_text_align(titleLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_static(titleLabel, "Setup Required");

    // Instructions — dynamic, updated by setDeviceInfo()
    instrLabel = lv_label_create(screen);
    lv_obj_align_to(instrLabel, titleLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_style_text_font(instrLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(instrLabel, lv_color_hex(0xB3B3B3), 0);
    lv_obj_set_style_text_align(instrLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(instrLabel, LV_PCT(85));
    lv_label_set_text_static(instrLabel,
        "1. Connect phone to WiFi:\n"
        "   SpotifyController\n\n"
        "2. Open setup page at address\n"
        "   shown below\n\n"
        "3. Enter WiFi credentials &\n"
        "   Spotify Client ID");

    // URL box — dynamic IP
    qrCodeImage = lv_obj_create(screen);
    lv_obj_set_size(qrCodeImage, LV_PCT(85), 70);
    lv_obj_align(qrCodeImage, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_bg_color(qrCodeImage, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(qrCodeImage, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(qrCodeImage, 12, 0);
    lv_obj_set_style_border_width(qrCodeImage, 0, 0);
    lv_obj_set_style_pad_all(qrCodeImage, 10, 0);

    urlLabel = lv_label_create(qrCodeImage);
    lv_obj_center(urlLabel);
    lv_obj_set_style_text_font(urlLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(urlLabel, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_text_align(urlLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_static(urlLabel, "http://192.168.4.1");

    // Status
    statusLabel = lv_label_create(screen);
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -MARGIN);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x727272), 0);
    lv_obj_set_style_text_align(statusLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(statusLabel, LV_PCT(80));
    lv_label_set_text_static(statusLabel, "Waiting for setup...");
}

void ui::AuthScreen::setAuthUrl(const String& url) {
    authUrl = url;

    // Update status with URL
    String message = "URL: " + url;
    lv_label_set_text(statusLabel, message.c_str());

    // TODO: Generate actual QR code
    // For now, we show a placeholder
}

void ui::AuthScreen::setStatus(const String& message) {
    lv_label_set_text(statusLabel, message.c_str());
}

void ui::AuthScreen::setDeviceInfo(const String& ip, bool isAPMode) {
    // Update URL display
    String url = "http://" + ip + "/";
    if (urlLabel) {
        lv_label_set_text(urlLabel, url.c_str());
    }

    // Update instructions based on mode
    if (instrLabel) {
        if (isAPMode) {
            lv_label_set_text(instrLabel,
                "1. Connect phone to WiFi:\n"
                "   SpotifyController\n\n"
                "2. Open setup page at address\n"
                "   shown below\n\n"
                "3. Enter WiFi credentials &\n"
                "   Spotify Client ID");
        } else {
            lv_label_set_text(instrLabel,
                "Open the address below on\n"
                "your phone (same WiFi network)\n"
                "to complete Spotify login.");
        }
    }
}
