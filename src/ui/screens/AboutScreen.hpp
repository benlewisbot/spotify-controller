/**
 * @file AboutScreen.hpp
 * @brief About Screen
 *
 * Device info, firmware version, and system information.
 */

#ifndef ABOUT_SCREEN_HPP
#define ABOUT_SCREEN_HPP

#include <lvgl.h>
#include "MenuManager.hpp"
#include "RuntimeConfig.hpp"

namespace ui {

/**
 * @brief About Screen Class
 */
class AboutScreen {
public:
    AboutScreen(lv_obj_t* parent);
    ~AboutScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }
    
    /**
     * @brief Update device info display
     */
    void updateDeviceInfo();
    
    /**
     * @brief Show firmware info dialog
     */
    void showFirmwareInfo();
    
    /**
     * @brief Show system info dialog
     */
    void showSystemInfo();
    
private:
    void createUI();
    void createHeader();
    void createLogoSection();
    void createDeviceInfoSection();
    void createFirmwareSection();
    void createLegalSection();
    void createActionButtons();
    
    String getChipModel();
    String getFlashSize();
    String getTotalHeap();
    String getFreeHeap();
    String getUptime();
    
    lv_obj_t* screen;
    lv_obj_t* deviceIdLabel;
    lv_obj_t* firmwareLabel;
    lv_obj_t* buildLabel;
    lv_obj_t* uptimeLabel;
    lv_obj_t* heapLabel;
};

} // namespace ui

#endif // ABOUT_SCREEN_HPP
