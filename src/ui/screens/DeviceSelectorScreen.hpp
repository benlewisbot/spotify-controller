/**
 * @file DeviceSelectorScreen.hpp
 * @brief Device Selector Screen
 *
 * Lists available Spotify Connect devices and allows
 * transferring playback to a selected device.
 */

#ifndef DEVICE_SELECTOR_SCREEN_HPP
#define DEVICE_SELECTOR_SCREEN_HPP

#include <lvgl.h>
#include <vector>
#include "MenuManager.hpp"
#include "../../spotify/SpotifyClient.hpp"
#include "../../app/App.hpp"

namespace ui {

/**
 * @brief Device Selector Screen Class
 *
 * Displays a scrollable list of Spotify Connect devices.
 * Active device is highlighted in green. Tapping a device
 * transfers playback via SpotifyClient::setDevice().
 */
class DeviceSelectorScreen {
public:
    DeviceSelectorScreen(lv_obj_t* parent);
    ~DeviceSelectorScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

    /**
     * @brief Refresh the device list from Spotify API
     */
    void refreshDevices();

private:
    void createUI();
    void createHeader();
    void createContentArea();
    void createLoadingSpinner();
    void createEmptyMessage();

    /**
     * @brief Populate the list with device entries
     */
    void populateDeviceList(const std::vector<SpotifyClient::DeviceInfo>& devices);

    /**
     * @brief Clear all items from the device list
     */
    void clearDeviceList();

    /**
     * @brief Create a single device list item
     */
    void createDeviceItem(lv_obj_t* parent, const SpotifyClient::DeviceInfo& device, int index);

    /**
     * @brief Show the loading state
     */
    void showLoading();

    /**
     * @brief Hide the loading state
     */
    void hideLoading();

    /**
     * @brief Show the empty message
     */
    void showEmptyMessage();

    /**
     * @brief Hide the empty message
     */
    void hideEmptyMessage();

    /**
     * @brief Handle device item tap
     */
    static void onDeviceItemClicked(lv_event_t* e);

    /**
     * @brief Handle refresh button tap
     */
    static void onRefreshClicked(lv_event_t* e);

    /**
     * @brief Handle back button tap
     */
    static void onBackClicked(lv_event_t* e);

    // Screen root
    lv_obj_t* screen;

    // Header elements
    lv_obj_t* backBtn;
    lv_obj_t* titleLabel;
    lv_obj_t* refreshBtn;

    // Content area
    lv_obj_t* contentContainer;

    // Loading spinner
    lv_obj_t* spinnerContainer;
    lv_obj_t* spinner;
    lv_obj_t* loadingLabel;

    // Empty state
    lv_obj_t* emptyContainer;
    lv_obj_t* emptyLabel;

    // Cached device list for click handler lookups
    std::vector<SpotifyClient::DeviceInfo> cachedDevices;
};

} // namespace ui

#endif // DEVICE_SELECTOR_SCREEN_HPP
