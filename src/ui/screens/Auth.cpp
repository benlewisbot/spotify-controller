/**
 * @file Auth.cpp
 * @brief Authentication Screen Implementation
 */

#include "Auth.hpp"
#include "../display/themes/SpotifyTheme.hpp"

#define MARGIN 24

AuthScreen::AuthScreen(lv_obj_t* parent)
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

AuthScreen::~AuthScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void AuthScreen::createUI() {
    // Title
    lv_obj_t* titleLabel = lv_label_create(screen);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, MARGIN);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_obj_set_style_text_align(titleLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_static(titleLabel, "Connect Spotify");

    // Instructions
    lv_obj_t* instrLabel = lv_label_create(screen);
    lv_obj_align_to(instrLabel, titleLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    lv_obj_set_style_text_font(instrLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(instrLabel, lv_color_hex(0xB3B3B3), 0);
    lv_obj_set_style_text_align(instrLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(instrLabel, LV_PCT(80));
    lv_label_set_text_static(instrLabel,
        "Scan the QR code or visit the URL below to connect your Spotify account.");

    // QR Code placeholder
    qrCodeImage = lv_image_create(screen);
    lv_obj_set_size(qrCodeImage, 200, 200);
    lv_obj_align(qrCodeImage, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(qrCodeImage, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(qrCodeImage, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(qrCodeImage, 8, 0);

    // QR code label
    lv_obj_t* qrLabel = lv_label_create(qrCodeImage);
    lv_obj_center(qrLabel);
    lv_obj_set_style_text_font(qrLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(qrLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(qrLabel, "QR Code");

    // Status/URL
    statusLabel = lv_label_create(screen);
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -MARGIN);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x727272), 0);
    lv_obj_set_style_text_align(statusLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(statusLabel, LV_PCT(80));
    lv_label_set_text_static(statusLabel, "Waiting for connection...");
}

void AuthScreen::setAuthUrl(const String& url) {
    authUrl = url;

    // Update status with URL
    String message = "URL: " + url;
    lv_label_set_text(statusLabel, message.c_str());

    // TODO: Generate actual QR code
    // For now, we show a placeholder
}

void AuthScreen::setStatus(const String& message) {
    lv_label_set_text(statusLabel, message.c_str());
}

} // namespace ui
