/**
 * @file MenuManager.hpp
 * @brief Screen Navigation Manager for Settings System
 *
 * Manages hierarchical menu navigation and screen transitions.
 * Maintains navigation history for back button support.
 */

#ifndef MENU_MANAGER_HPP
#define MENU_MANAGER_HPP

#include <lvgl.h>
#include <Arduino.h>
#include <vector>
#include <map>
#include <functional>

// Forward declarations
namespace ui {
    class SettingsScreen;
    class WiFiSettingsScreen;
    class SpotifyTokenScreen;
    class DisplaySettingsScreen;
    class AboutScreen;
}

/**
 * @brief Screen type enumeration
 */
enum class ScreenType {
    SETTINGS_MAIN,
    WIFI_SETTINGS,
    WIFI_SCAN,
    WIFI_CONNECT,
    SPOTIFY_TOKEN,
    SPOTIFY_REAUTH,
    DISPLAY_SETTINGS,
    BRIGHTNESS,
    ORIENTATION,
    SCREENSAVER,
    ABOUT,
    DEVICE_INFO,
    FIRMWARE_INFO
};

/**
 * @brief Navigation history entry
 *
 * NOTE: contextData is NOT automatically freed.
 * If you allocate memory for contextData, you must free it manually
 * when popping from the history stack or clearing history.
 */
struct NavigationEntry {
    ScreenType screenType;
    void* contextData;

    NavigationEntry(ScreenType type, void* data = nullptr)
        : screenType(type), contextData(data) {}
};

/**
 * @brief Menu Manager Class
 *
 * Manages screen navigation hierarchy and transitions.
 */
class MenuManager {
public:
    /**
     * @brief Get singleton instance
     */
    static MenuManager& getInstance() {
        static MenuManager instance;
        return instance;
    }
    
    // Delete copy constructor and assignment operator
    MenuManager(const MenuManager&) = delete;
    MenuManager& operator=(const MenuManager&) = delete;
    
    /**
     * @brief Initialize menu manager
     */
    void init(lv_obj_t* root);
    
    /**
     * @brief Navigate to a screen
     */
    void navigateTo(ScreenType screenType, void* contextData = nullptr);
    
    /**
     * @brief Go back to previous screen
     */
    void goBack();
    
    /**
     * @brief Navigate back to settings main
     */
    void goToHome();
    
    /**
     * @brief Get current screen type
     */
    ScreenType getCurrentScreen() const { return currentScreen; }
    
    /**
     * @brief Check if can go back
     */
    bool canGoBack() const { return !historyStack.empty(); }
    
    /**
     * @brief Get navigation history depth
     */
    size_t getHistoryDepth() const { return historyStack.size(); }
    
    /**
     * @brief Clear navigation history
     *
     * WARNING: This does not automatically free contextData memory.
     * If you allocated memory for contextData entries, free them before calling this.
     */
    void clearHistory();

    /**
     * @brief Clear navigation history and optionally free context data
     * @param freeContext If true, calls free() on all contextData pointers
     */
    void clearHistoryAndFree(bool freeContext = false);

    /**
     * @brief Remove all event handlers from dialog buttons
     *
     * Call this before adding new event handlers to prevent memory leaks.
     * Removes any existing callbacks from messageConfirmBtn and messageCancelBtn.
     */
    void clearDialogHandlers();
    
    /**
     * @brief Show loading indicator
     */
    void showLoading(const String& message = "");
    
    /**
     * @brief Hide loading indicator
     */
    void hideLoading();
    
    /**
     * @brief Show success message
     */
    void showSuccess(const String& message);
    
    /**
     * @brief Show error message
     */
    void showError(const String& message);
    
    /**
     * @brief Show confirmation dialog
     */
    void showConfirm(const String& title, const String& message,
                     std::function<void()> onConfirm,
                     std::function<void()> onCancel = nullptr);
    
    /**
     * @brief Register screen instance
     */
    template<typename T>
    void registerScreen(ScreenType type, T* screen) {
        screens[type] = screen;
    }
    
    /**
     * @brief Get screen instance
     */
    template<typename T>
    T* getScreen(ScreenType type) {
        auto it = screens.find(type);
        return (it != screens.end()) ? static_cast<T*>(it->second) : nullptr;
    }
    
    /**
     * @brief Set back button callback
     */
    void setBackButtonCallback(std::function<void()> callback) {
        backButtonCallback = callback;
    }

public:
    MenuManager() = default;
    ~MenuManager() = default;

private:
    void hideAllScreens();
    void showScreen(ScreenType screenType, void* contextData);
    void onScreenTransition(ScreenType from, ScreenType to);
    
    lv_obj_t* root;
    lv_obj_t* loadingOverlay;
    lv_obj_t* loadingSpinner;
    lv_obj_t* loadingLabel;
    
    lv_obj_t* messageOverlay;
    lv_obj_t* messageTitle;
    lv_obj_t* messageBody;
    lv_obj_t* messageConfirmBtn;
    lv_obj_t* messageCancelBtn;
    
    ScreenType currentScreen;
    std::vector<NavigationEntry> historyStack;
    
    std::map<ScreenType, void*> screens;

    std::function<void()> backButtonCallback;

    // Confirm dialog callbacks
    std::function<void()> confirmCallback;
    std::function<void()> cancelCallback;

    // Timer for auto-hide messages
    lv_timer_t* messageTimer;
};

#endif // MENU_MANAGER_HPP
