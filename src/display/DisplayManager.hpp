/**
 * @file DisplayManager.hpp
 * @brief Display Manager for LVGL integration
 *
 * Manages the display hardware and provides LVGL interface.
 * Handles multiple display types via abstract interface.
 */

#ifndef DISPLAY_MANAGER_HPP
#define DISPLAY_MANAGER_HPP

#include "Display.hpp"
#include <lvgl.h>

// Display type selection (compile-time)
#define DISPLAY_TYPE_AUTO 0
#define DISPLAY_TYPE_ILI9341 1
#define DISPLAY_TYPE_ILI9488 2
#define DISPLAY_TYPE_ST7789 3
#define DISPLAY_TYPE_ST7796U 4

// Set your display type here
#define DISPLAY_TYPE DISPLAY_TYPE_AUTO

// Display resolution defaults
#define DEFAULT_DISPLAY_WIDTH 320
#define DEFAULT_DISPLAY_HEIGHT 480
#define DISPLAY_BUFFER_SIZE (DEFAULT_DISPLAY_WIDTH * 40)  // 40 lines buffer

/**
 * @brief Display Manager Class
 *
 * Singleton pattern. Manages LVGL integration with display hardware.
 */
class DisplayManager {
public:
    /**
     * @brief Get the singleton instance
     */
    static DisplayManager* getInstance() {
        static DisplayManager instance;
        return &instance;
    }

    // Delete copy constructor and assignment operator
    DisplayManager(const DisplayManager&) = delete;
    DisplayManager& operator=(const DisplayManager&) = delete;

    /**
     * @brief Initialize the display manager
     * @return true if successful, false otherwise
     */
    bool init();

    /**
     * @brief Update LVGL (call this periodically)
     */
    void update();

    /**
     * @brief Get LVGL display driver
     */
    lv_display_t* getLvglDisplay() { return display; }

    /**
     * @brief Get LVGL input device driver
     */
    lv_indev_t* getLvglInputDevice() { return indev; }

    /**
     * @brief Get display width
     */
    int16_t getWidth() const { return width; }

    /**
     * @brief Get display height
     */
    int16_t getHeight() const { return height; }

    /**
     * @brief Check if display is initialized
     */
    bool isInitialized() const { return initialized; }

    /**
     * @brief Set brightness
     */
    void setBrightness(uint8_t brightness) {
        if (displayImpl) {
            displayImpl->setBrightness(brightness);
        }
    }

    /**
     * @brief Get brightness
     */
    uint8_t getBrightness() const {
        if (displayImpl) {
            return displayImpl->getBrightness();
        }
        return 100;
    }

    /**
     * @brief Set orientation
     */
    void setOrientation(bool portrait) {
        if (displayImpl) {
            displayImpl->setOrientation(portrait);
        }
    }

    /**
     * @brief Turn display on/off
     */
    void setPower(bool on) {
        if (displayImpl) {
            displayImpl->setPower(on);
        }
    }

private:
    DisplayManager();
    ~DisplayManager();

    /**
     * @brief Auto-detect and create display driver
     */
    bool createDisplayDriver();

    /**
     * @brief Auto-detect and create touch driver
     */
    bool createTouchDriver();

    /**
     * @brief Initialize LVGL
     */
    bool initLVGL();

    /**
     * @brief LVGL flush callback
     */
    static void flushCallback(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);

    /**
     * @brief LVGL touch callback
     */
    static void touchCallback(lv_indev_t* indev, lv_indev_data_t* data);

    /**
     * @brief LVGL flush ready callback
     */
    static void flushReadyCallback(lv_display_t* disp);

    /**
     * @brief LVGL render callback
     */
    static void renderCallback(lv_timer_t* timer);

    // Member variables
    bool initialized;
    DisplayInterface* displayImpl;
    TouchInterface* touchImpl;
    lv_display_t* display;
    lv_indev_t* indev;

    int16_t width;
    int16_t height;

    // Touch state
    int16_t lastTouchX;
    int16_t lastTouchY;
    bool lastTouchPressed;

    // Update tracking
    unsigned long lastUpdateMs;
};

#endif // DISPLAY_MANAGER_HPP
