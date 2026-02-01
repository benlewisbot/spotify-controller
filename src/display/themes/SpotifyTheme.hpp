/**
 * @file SpotifyTheme.hpp
 * @brief Spotify-inspired Theme for LVGL
 *
 * Dark theme inspired by Spotify app with Apple Liquid Glass effects.
 */

#ifndef SPOTIFY_THEME_HPP
#define SPOTIFY_THEME_HPP

#include <lvgl.h>

// Spotify-inspired color palette
#define COLOR_SPOTIFY_BG           lv_color_hex(0x121212)
#define COLOR_SPOTIFY_SURFACE      lv_color_hex(0x282828)
#define COLOR_SPOTIFY_ELEVATED     lv_color_hex(0x3E3E3E)
#define COLOR_SPOTIFY_PRIMARY      lv_color_hex(0x1DB954)
#define COLOR_SPOTIFY_ACCENT       lv_color_hex(0x1ED760)
#define COLOR_SPOTIFY_TEXT         lv_color_hex(0xFFFFFF)
#define COLOR_SPOTIFY_TEXT_SEC     lv_color_hex(0xB3B3B3)
#define COLOR_SPOTIFY_TEXT_DIM     lv_color_hex(0x727272)
#define COLOR_SPOTIFY_PROGRESS     lv_color_hex(0x535353)
#define COLOR_SPOTIFY_ERROR        lv_color_hex(0xE91429)

// Typography scale
#define FONT_SIZE_HEADER 24
#define FONT_SIZE_TITLE 18
#define FONT_SIZE_BODY 14
#define FONT_SIZE_CAPTION 12

// Spacing scale
#define SPACING_XS 4
#define SPACING_SM 8
#define SPACING_MD 16
#define SPACING_LG 24
#define SPACING_XL 32

// Border radius
#define RADIUS_SM 4
#define RADIUS_MD 8
#define RADIUS_LG 16
#define RADIUS_XL 24

/**
 * @brief Spotify Theme Class
 *
 * Applies Spotify-inspired styling to LVGL widgets.
 */
class SpotifyTheme {
public:
    /**
     * @brief Apply Spotify theme to LVGL
     */
    static void apply();

    /**
     * @brief Get color from palette
     */
    static lv_color_t getColor(const char* name) {
        if (strcmp(name, "bg") == 0) return COLOR_SPOTIFY_BG;
        if (strcmp(name, "surface") == 0) return COLOR_SPOTIFY_SURFACE;
        if (strcmp(name, "elevated") == 0) return COLOR_SPOTIFY_ELEVATED;
        if (strcmp(name, "primary") == 0) return COLOR_SPOTIFY_PRIMARY;
        if (strcmp(name, "accent") == 0) return COLOR_SPOTIFY_ACCENT;
        if (strcmp(name, "text") == 0) return COLOR_SPOTIFY_TEXT;
        if (strcmp(name, "text_sec") == 0) return COLOR_SPOTIFY_TEXT_SEC;
        if (strcmp(name, "text_dim") == 0) return COLOR_SPOTIFY_TEXT_DIM;
        if (strcmp(name, "progress") == 0) return COLOR_SPOTIFY_PROGRESS;
        return lv_color_white();
    }

private:
    /**
     * @brief Style the screen
     */
    static void styleScreen();

    /**
     * @brief Style buttons
     */
    static void styleButton();

    /**
     * @brief Style labels
     */
    static void styleLabel();

    /**
     * @brief Style progress bars/sliders
     */
    static void styleProgressBar();

    /**
     * @brief Style image (album art)
     */
    static void styleImage();

    /**
     * @brief Style lists (playlist browser)
     */
    static void styleList();

    // Style objects
    static lv_style_t styleScreenBg;
    static lv_style_t styleButtonNormal;
    static lv_style_t styleButtonPressed;
    static lv_style_t styleButtonPrimary;
    static lv_style_t styleLabelHeader;
    static lv_style_t styleLabelTitle;
    static lv_style_t styleLabelBody;
    static lv_style_t styleLabelCaption;
    static lv_style_t styleProgressBarBg;
    static lv_style_t styleProgressBarInd;
    static lv_style_t styleSliderKnob;
    static lv_style_t styleImageCard;
    static lv_style_t styleListBg;
    static lv_style_t styleListItem;

    static bool initialized;
};

#endif // SPOTIFY_THEME_HPP
