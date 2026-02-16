/**
 * @file DisplaySettingsScreen.hpp
 * @brief Display Settings Screen
 *
 * Brightness, rotation, and screensaver configuration.
 */

#ifndef DISPLAY_SETTINGS_SCREEN_HPP
#define DISPLAY_SETTINGS_SCREEN_HPP

#include <lvgl.h>
#include "MenuManager.hpp"
#include "RuntimeConfig.hpp"

namespace ui {

/**
 * @brief Display Settings Screen Class
 */
class DisplaySettingsScreen {
public:
    DisplaySettingsScreen(lv_obj_t* parent);
    ~DisplaySettingsScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }
    
    /**
     * @brief Update brightness display
     */
    void setBrightness(uint8_t brightness);
    
    /**
     * @brief Update orientation display
     */
    void setOrientation(uint8_t rotation);
    
    /**
     * @brief Update screensaver settings display
     */
    void setScreensaverEnabled(bool enabled);
    void setScreensaverTimeout(uint16_t minutes);
    
    /**
     * @brief Get current settings
     */
    uint8_t getBrightness() const { return currentBrightness; }
    uint8_t getOrientation() const { return currentOrientation; }
    bool getScreensaverEnabled() const { return screensaverEnabled; }
    uint16_t getScreensaverTimeout() const { return screensaverTimeout; }
    
private:
    void createUI();
    void createHeader();
    void createBrightnessSection();
    void createOrientationSection();
    void createScreensaverSection();
    void createOtherSection();
    void onBrightnessChanged(lv_obj_t* slider, int32_t value);
    void onScreensaverToggleChanged(lv_obj_t* obj, lv_event_t event);
    const char* getOrientationName(uint8_t rotation);
    
    lv_obj_t* screen;
    lv_obj_t* brightnessSlider;
    lv_obj_t* brightnessLabel;
    lv_obj_t* orientationLabel;
    lv_obj_t* orientationBtn;
    lv_obj_t* screensaverSwitch;
    lv_obj_t* screensaverTimeoutBtn;
    lv_obj_t* timeoutLabel;
    
    uint8_t currentBrightness;
    uint8_t currentOrientation;
    bool screensaverEnabled;
    uint16_t screensaverTimeout;
};

} // namespace ui

#endif // DISPLAY_SETTINGS_SCREEN_HPP
