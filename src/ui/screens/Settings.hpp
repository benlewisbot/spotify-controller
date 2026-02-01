/**
 * @file Settings.hpp
 * @brief Settings Screen
 *
 * Application settings: display, WiFi, Spotify, etc.
 */

#ifndef SETTINGS_SCREEN_HPP
#define SETTINGS_SCREEN_HPP

#include <lvgl.h>

namespace ui {

/**
 * @brief Settings Screen Class
 */
class SettingsScreen {
public:
    SettingsScreen(lv_obj_t* parent);
    ~SettingsScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

private:
    void createUI();
    void createSection(const String& title);
    void createSettingItem(const String& label, const String& value, bool withToggle = false);

    lv_obj_t* screen;
    lv_obj_t* settingsList;
};

} // namespace ui

#endif // SETTINGS_SCREEN_HPP
