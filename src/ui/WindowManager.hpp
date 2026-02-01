/**
 * @file WindowManager.hpp
 * @brief Window/Screen Manager
 *
 * Manages UI screens and navigation.
 */

#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <lvgl.h>
#include "../display/DisplayManager.hpp"

// Forward declarations
namespace ui {
    class NowPlayingScreen;
    class AuthScreen;
    class SettingsScreen;
}

/**
 * @brief Window Manager Class
 *
 * Manages all UI screens and transitions.
 */
class WindowManager {
public:
    WindowManager(DisplayManager* dm);
    ~WindowManager();

    /**
     * @brief Initialize the window manager
     */
    void init();

    /**
     * @brief Update LVGL (call this periodically)
     */
    void update();

    /**
     * @brief Show Now Playing screen
     */
    void showNowPlaying();

    /**
     * @brief Show Authentication screen
     */
    void showAuthScreen();

    /**
     * @brief Show Settings screen
     */
    void showSettings();

    /**
     * @brief Go back to previous screen
     */
    void goBack();

    /**
     * @brief Get current screen
     */
    lv_obj_t* getCurrentScreen() const { return currentScreen; }

    /**
     * @brief Get root object
     */
    lv_obj_t* getRoot() const { return root; }

private:
    /**
     * @brief Create a new screen
     */
    lv_obj_t* createScreen();

    /**
     * @brief Clear current screen
     */
    void clearScreen();

    /**
     * @brief Transition to a new screen
     */
    void transitionTo(lv_obj_t* newScreen);

    DisplayManager* displayManager;
    lv_obj_t* root;
    lv_obj_t* currentScreen;

    ui::NowPlayingScreen* nowPlayingScreen;
    ui::AuthScreen* authScreen;
    ui::SettingsScreen* settingsScreen;

    bool initialized;
};

#endif // WINDOW_MANAGER_HPP
