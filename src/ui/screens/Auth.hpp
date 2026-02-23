/**
 * @file Auth.hpp
 * @brief Authentication Screen
 *
 * Shows QR code or instructions for Spotify OAuth authentication.
 */

#ifndef AUTH_SCREEN_HPP
#define AUTH_SCREEN_HPP

#include <lvgl.h>
#include <Arduino.h>

namespace ui {

/**
 * @brief Authentication Screen Class
 *
 * Shown when Spotify authentication is required.
 */
class AuthScreen {
public:
    AuthScreen(lv_obj_t* parent);
    ~AuthScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

    /**
     * @brief Set authentication URL
     */
    void setAuthUrl(const String& url);

    /**
     * @brief Update status message
     */
    void setStatus(const String& message);

    /**
     * @brief Update the displayed IP address and instructions
     * @param ip Device IP address (e.g., "192.168.2.106" or "192.168.4.1")
     * @param isAPMode true if device is in AP mode, false if on home WiFi
     */
    void setDeviceInfo(const String& ip, bool isAPMode);

private:
    void createUI();
    void createQRCode(const String& url);

    lv_obj_t* screen;
    lv_obj_t* statusLabel;
    lv_obj_t* instrLabel;
    lv_obj_t* urlLabel;
    lv_obj_t* qrCodeImage;

    String authUrl;
};

} // namespace ui

#endif // AUTH_SCREEN_HPP
