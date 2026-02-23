/**
 * @file SplashScreen.hpp
 * @brief Splash Screen
 *
 * Startup splash screen with branding, version info, status text,
 * and a loading spinner for the 480x480 display.
 */

#ifndef SPLASH_SCREEN_HPP
#define SPLASH_SCREEN_HPP

#include <lvgl.h>

namespace ui {

/**
 * @brief Splash Screen Class
 *
 * Shown during application startup while WiFi connects
 * and Spotify authentication is performed.
 */
class SplashScreen {
public:
    SplashScreen(lv_obj_t* parent);
    ~SplashScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

    /**
     * @brief Update the status message
     * @param status Status text (e.g. "Connecting to WiFi...")
     */
    void setStatus(const char* status);

    /**
     * @brief Hide the splash screen
     */
    void hide();

private:
    void createUI();
    void createBranding();
    void createVersionLabel();
    void createStatusLabel();
    void createSpinner();

    lv_obj_t* screen;
    lv_obj_t* titleLabel;
    lv_obj_t* subtitleLabel;
    lv_obj_t* versionLabel;
    lv_obj_t* statusLabel;
    lv_obj_t* spinner;
};

} // namespace ui

#endif // SPLASH_SCREEN_HPP
