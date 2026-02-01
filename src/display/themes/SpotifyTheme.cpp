/**
 * @file SpotifyTheme.cpp
 * @brief Spotify-inspired Theme Implementation
 */

#include "SpotifyTheme.hpp"

// Static member initialization
lv_style_t SpotifyTheme::styleScreenBg;
lv_style_t SpotifyTheme::styleButtonNormal;
lv_style_t SpotifyTheme::styleButtonPressed;
lv_style_t SpotifyTheme::styleButtonPrimary;
lv_style_t SpotifyTheme::styleLabelHeader;
lv_style_t SpotifyTheme::styleLabelTitle;
lv_style_t SpotifyTheme::styleLabelBody;
lv_style_t SpotifyTheme::styleLabelCaption;
lv_style_t SpotifyTheme::styleProgressBarBg;
lv_style_t SpotifyTheme::styleProgressBarInd;
lv_style_t SpotifyTheme::styleSliderKnob;
lv_style_t SpotifyTheme::styleImageCard;
lv_style_t SpotifyTheme::styleListBg;
lv_style_t SpotifyTheme::styleListItem;

bool SpotifyTheme::initialized = false;

void SpotifyTheme::apply() {
    if (initialized) {
        return;
    }

    // Initialize all styles
    lv_style_init(&styleScreenBg);
    lv_style_init(&styleButtonNormal);
    lv_style_init(&styleButtonPressed);
    lv_style_init(&styleButtonPrimary);
    lv_style_init(&styleLabelHeader);
    lv_style_init(&styleLabelTitle);
    lv_style_init(&styleLabelBody);
    lv_style_init(&styleLabelCaption);
    lv_style_init(&styleProgressBarBg);
    lv_style_init(&styleProgressBarInd);
    lv_style_init(&styleSliderKnob);
    lv_style_init(&styleImageCard);
    lv_style_init(&styleListBg);
    lv_style_init(&styleListItem);

    // Style screen background
    lv_style_set_bg_color(&styleScreenBg, COLOR_SPOTIFY_BG);
    lv_style_set_bg_opa(&styleScreenBg, LV_OPA_COVER);
    lv_style_set_text_color(&styleScreenBg, COLOR_SPOTIFY_TEXT);

    // Style normal button
    lv_style_set_radius(&styleButtonNormal, RADIUS_MD);
    lv_style_set_bg_color(&styleButtonNormal, COLOR_SPOTIFY_ELEVATED);
    lv_style_set_bg_opa(&styleButtonNormal, LV_OPA_COVER);
    lv_style_set_text_color(&styleButtonNormal, COLOR_SPOTIFY_TEXT);
    lv_style_set_border_width(&styleButtonNormal, 0);
    lv_style_set_pad_all(&styleButtonNormal, SPACING_SM);

    // Style pressed button
    lv_style_set_radius(&styleButtonPressed, RADIUS_MD);
    lv_style_set_bg_color(&styleButtonPressed, COLOR_SPOTIFY_SURFACE);
    lv_style_set_bg_opa(&styleButtonPressed, LV_OPA_COVER);
    lv_style_set_transform_scale_x(&styleButtonPressed, 950);
    lv_style_set_transform_scale_y(&styleButtonPressed, 950);
    lv_style_set_transition_time(&styleButtonPressed, 150);
    lv_style_set_transition_prop(&styleButtonPressed, LV_STYLE_TRANSFORM_SCALE_X);

    // Style primary button (Play, etc.)
    lv_style_set_radius(&styleButtonPrimary, RADIUS_MD);
    lv_style_set_bg_color(&styleButtonPrimary, COLOR_SPOTIFY_PRIMARY);
    lv_style_set_bg_opa(&styleButtonPrimary, LV_OPA_COVER);
    lv_style_set_text_color(&styleButtonPrimary, COLOR_SPOTIFY_TEXT);
    lv_style_set_border_width(&styleButtonPrimary, 0);

    // Style header label
    lv_style_set_text_font(&styleLabelHeader, &lv_font_montserrat_24);
    lv_style_set_text_color(&styleLabelHeader, COLOR_SPOTIFY_TEXT);

    // Style title label
    lv_style_set_text_font(&styleLabelTitle, &lv_font_montserrat_18);
    lv_style_set_text_color(&styleLabelTitle, COLOR_SPOTIFY_TEXT);

    // Style body label
    lv_style_set_text_font(&styleLabelBody, &lv_font_montserrat_14);
    lv_style_set_text_color(&styleLabelBody, COLOR_SPOTIFY_TEXT_SEC);

    // Style caption label
    lv_style_set_text_font(&styleLabelCaption, &lv_font_montserrat_12);
    lv_style_set_text_color(&styleLabelCaption, COLOR_SPOTIFY_TEXT_DIM);

    // Style progress bar background
    lv_style_set_radius(&styleProgressBarBg, 2);
    lv_style_set_bg_color(&styleProgressBarBg, COLOR_SPOTIFY_PROGRESS);
    lv_style_set_bg_opa(&styleProgressBarBg, LV_OPA_COVER);
    lv_style_set_height(&styleProgressBarBg, 4);

    // Style progress bar indicator
    lv_style_set_radius(&styleProgressBarInd, 2);
    lv_style_set_bg_color(&styleProgressBarInd, COLOR_SPOTIFY_TEXT_SEC);
    lv_style_set_bg_opa(&styleProgressBarInd, LV_OPA_COVER);

    // Style slider knob (for volume)
    lv_style_set_radius(&styleSliderKnob, 8);
    lv_style_set_bg_color(&styleSliderKnob, COLOR_SPOTIFY_TEXT);
    lv_style_set_bg_opa(&styleSliderKnob, LV_OPA_COVER);
    lv_style_set_width(&styleSliderKnob, 16);
    lv_style_set_height(&styleSliderKnob, 16);

    // Style image card (album art)
    lv_style_set_radius(&styleImageCard, RADIUS_LG);
    lv_style_set_border_width(&styleImageCard, 1);
    lv_style_set_border_color(&styleImageCard, COLOR_SPOTIFY_ELEVATED);
    lv_style_set_border_opa(&styleImageCard, LV_OPA_50);
    lv_style_set_shadow_width(&styleImageCard, 10);
    lv_style_set_shadow_ofs_x(&styleImageCard, 0);
    lv_style_set_shadow_ofs_y(&styleImageCard, 4);
    lv_style_set_shadow_color(&styleImageCard, lv_color_black());
    lv_style_set_shadow_opa(&styleImageCard, LV_OPA_30);

    // Style list background
    lv_style_set_bg_color(&styleListBg, COLOR_SPOTIFY_BG);
    lv_style_set_bg_opa(&styleListBg, LV_OPA_COVER);
    lv_style_set_pad_all(&styleListBg, 0);

    // Style list item
    lv_style_set_bg_color(&styleListItem, LV_TRANSP_COLOR);
    lv_style_set_bg_opa(&styleListItem, LV_OPA_TRANSP);
    lv_style_set_pad_hor(&styleListItem, SPACING_MD);
    lv_style_set_pad_ver(&styleListItem, SPACING_SM);
    lv_style_set_border_width(&styleListItem, 0);
    lv_style_set_radius(&styleListItem, 0);

    // Apply screen style to default
    lv_obj_set_style_bg_color(lv_scr_act(), COLOR_SPOTIFY_BG, 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);

    initialized = true;
}
