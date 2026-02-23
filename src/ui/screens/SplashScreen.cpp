/**
 * @file SplashScreen.cpp
 * @brief Splash Screen Implementation
 */

#include "SplashScreen.hpp"
#include "../../display/themes/SpotifyTheme.hpp"

namespace ui {

SplashScreen::SplashScreen(lv_obj_t* parent)
    : screen(nullptr)
    , titleLabel(nullptr)
    , subtitleLabel(nullptr)
    , versionLabel(nullptr)
    , statusLabel(nullptr)
    , spinner(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, COLOR_SPOTIFY_BG, 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

SplashScreen::~SplashScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void SplashScreen::createUI() {
    createBranding();
    createVersionLabel();
    createStatusLabel();
    createSpinner();
}

void SplashScreen::createBranding() {
    // "SPOTIFY" title - centered, green, montserrat_32
    titleLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(titleLabel, COLOR_SPOTIFY_PRIMARY, 0);
    lv_obj_set_style_text_align(titleLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(titleLabel, 6, 0);
    lv_label_set_text_static(titleLabel, "SPOTIFY");
    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, -60);

    // "CONTROLLER" subtitle - below title, white, montserrat_24
    subtitleLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(subtitleLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(subtitleLabel, COLOR_SPOTIFY_TEXT, 0);
    lv_obj_set_style_text_align(subtitleLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(subtitleLabel, 4, 0);
    lv_label_set_text_static(subtitleLabel, "CONTROLLER");
    lv_obj_align_to(subtitleLabel, titleLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
}

void SplashScreen::createVersionLabel() {
    // Version text - below subtitle, secondary grey, montserrat_14
    versionLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(versionLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(versionLabel, COLOR_SPOTIFY_TEXT_SEC, 0);
    lv_obj_set_style_text_align(versionLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_static(versionLabel, "v1.0.0");
    lv_obj_align_to(versionLabel, subtitleLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
}

void SplashScreen::createStatusLabel() {
    // Status text - below version, secondary grey, montserrat_14
    statusLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(statusLabel, COLOR_SPOTIFY_TEXT_SEC, 0);
    lv_obj_set_style_text_align(statusLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(statusLabel, LV_PCT(80));
    lv_label_set_text_static(statusLabel, "Initializing...");
    lv_obj_align(statusLabel, LV_ALIGN_CENTER, 0, 60);
}

void SplashScreen::createSpinner() {
    // Loading spinner - below status text, Spotify green
    spinner = lv_spinner_create(screen);
    lv_obj_set_size(spinner, 40, 40);
    lv_obj_align_to(spinner, statusLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    // Style the spinner arc: green active indicator on dark background
    lv_obj_set_style_arc_width(spinner, 4, 0);
    lv_obj_set_style_arc_color(spinner, COLOR_SPOTIFY_ELEVATED, 0);

    lv_obj_set_style_arc_width(spinner, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(spinner, COLOR_SPOTIFY_PRIMARY, LV_PART_INDICATOR);
}

void SplashScreen::setStatus(const char* status) {
    if (statusLabel) {
        lv_label_set_text(statusLabel, status);
    }
}

void SplashScreen::hide() {
    if (screen) {
        lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);
    }
}

} // namespace ui
