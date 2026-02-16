/**
 * @file Settings.hpp
 * @brief Main Settings Screen
 *
 * Application settings: display, WiFi, Spotify, etc.
 */

#ifndef SETTINGS_SCREEN_HPP
#define SETTINGS_SCREEN_HPP

#include <lvgl.h>
#include "MenuManager.hpp"

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
    
    /**
     * @brief Update WiFi status display
     */
    void updateWiFiStatus(const String& ssid, bool connected);
    
    /**
     * @brief Update Spotify account display
     */
    void updateSpotifyAccount(const String& email);

private:
    void createUI();
    void createSection(lv_obj_t* parent, const String& title, int yPos);
    void createNavigationItem(lv_obj_t* parent, const String& label, 
                             const String& subtitle, int yPos, ScreenType targetScreen);

    lv_obj_t* screen;
    lv_obj_t* settingsList;
};

} // namespace ui

#endif // SETTINGS_SCREEN_HPP
