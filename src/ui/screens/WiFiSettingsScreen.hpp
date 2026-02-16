/**
 * @file WiFiSettingsScreen.hpp
 * @brief WiFi Settings Screen
 *
 * WiFi network configuration: scan, connect, static IP settings.
 */

#ifndef WIFI_SETTINGS_SCREEN_HPP
#define WIFI_SETTINGS_SCREEN_HPP

#include <lvgl.h>
#include "MenuManager.hpp"
#include "RuntimeConfig.hpp"
#include <vector>

namespace ui {

/**
 * @brief Network scan result
 */
struct NetworkInfo {
    String ssid;
    int rssi;
    bool secured;
    bool connected;
    
    NetworkInfo() : rssi(0), secured(false), connected(false) {}
};

/**
 * @brief WiFi Settings Screen Class
 */
class WiFiSettingsScreen {
public:
    WiFiSettingsScreen(lv_obj_t* parent);
    ~WiFiSettingsScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }
    
    /**
     * @brief Start network scan
     */
    void startScan();
    
    /**
     * @brief Update scan results
     */
    void updateScanResults(const std::vector<NetworkInfo>& networks);
    
    /**
     * @brief Show connect dialog
     */
    void showConnectDialog(const NetworkInfo& network);
    
    /**
     * @brief Update connection status
     */
    void updateConnectionStatus(bool connected, const String& ssid);
    
    /**
     * @brief Show loading state
     */
    void setLoading(bool loading);
    
private:
    void createUI();
    void createHeader();
    void createStatusSection();
    void createNetworkList();
    void createStaticIPSection();
    void createNetworkItem(const NetworkInfo& network, int yPos);
    int getSignalStrength(int rssi);
    const char* getSecurityIcon(bool secured);
    
    lv_obj_t* screen;
    lv_obj_t* statusCard;
    lv_obj_t* statusIcon;
    lv_obj_t* statusLabel;
    lv_obj_t* ssidLabel;
    lv_obj_t* ipLabel;
    lv_obj_t* networkList;
    lv_obj_t* scanButton;
    lv_obj_t* staticIPSection;
    lv_obj_t* staticIPSwitch;
    
    std::vector<NetworkInfo> networks;
    bool isScanning;
    bool connected;
    String currentSSID;
};

} // namespace ui

#endif // WIFI_SETTINGS_SCREEN_HPP
