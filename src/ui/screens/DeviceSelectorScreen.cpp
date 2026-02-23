/**
 * @file DeviceSelectorScreen.cpp
 * @brief Device Selector Screen Implementation
 *
 * Displays available Spotify Connect devices and handles
 * playback transfer to a selected device.
 */

#include "DeviceSelectorScreen.hpp"
#include "MenuManager.hpp"

#define MARGIN          16
#define HEADER_HEIGHT   60
#define ITEM_HEIGHT     60
#define ITEM_GAP        8
#define ITEM_RADIUS     12

// Colors
#define COLOR_BG        0x121212
#define COLOR_HEADER    0x181818
#define COLOR_SURFACE   0x282828
#define COLOR_GREEN     0x1DB954
#define COLOR_TEXT      0xFFFFFF
#define COLOR_SECONDARY 0xB3B3B3

namespace ui {

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

DeviceSelectorScreen::DeviceSelectorScreen(lv_obj_t* parent)
    : screen(nullptr)
    , backBtn(nullptr)
    , titleLabel(nullptr)
    , refreshBtn(nullptr)
    , contentContainer(nullptr)
    , spinnerContainer(nullptr)
    , spinner(nullptr)
    , loadingLabel(nullptr)
    , emptyContainer(nullptr)
    , emptyLabel(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_BG), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

DeviceSelectorScreen::~DeviceSelectorScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

// ---------------------------------------------------------------------------
// UI Creation
// ---------------------------------------------------------------------------

void DeviceSelectorScreen::createUI() {
    createHeader();
    createContentArea();
    createLoadingSpinner();
    createEmptyMessage();
}

void DeviceSelectorScreen::createHeader() {
    lv_obj_t* header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_PCT(100), HEADER_HEIGHT);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(COLOR_HEADER), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, MARGIN, 0);

    // ---- Back button ----
    backBtn = lv_button_create(header);
    lv_obj_set_size(backBtn, 40, 40);
    lv_obj_set_style_bg_opa(backBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(backBtn, 0, 0);
    lv_obj_set_style_radius(backBtn, 20, 0);

    lv_obj_t* backLabel = lv_label_create(backBtn);
    lv_obj_center(backLabel);
    lv_obj_set_style_text_font(backLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(backLabel, lv_color_white(), 0);
    lv_label_set_text_static(backLabel, LV_SYMBOL_LEFT);

    lv_obj_add_event_cb(backBtn, onBackClicked, LV_EVENT_CLICKED, this);

    // ---- Title ----
    titleLabel = lv_label_create(header);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_label_set_text_static(titleLabel, "Devices");
    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);

    // ---- Refresh button ----
    refreshBtn = lv_button_create(header);
    lv_obj_set_size(refreshBtn, 40, 40);
    lv_obj_align(refreshBtn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_opa(refreshBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(refreshBtn, 0, 0);
    lv_obj_set_style_radius(refreshBtn, 20, 0);

    lv_obj_t* refreshLabel = lv_label_create(refreshBtn);
    lv_obj_center(refreshLabel);
    lv_obj_set_style_text_font(refreshLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(refreshLabel, lv_color_white(), 0);
    lv_label_set_text_static(refreshLabel, LV_SYMBOL_REFRESH);

    lv_obj_add_event_cb(refreshBtn, onRefreshClicked, LV_EVENT_CLICKED, this);
}

void DeviceSelectorScreen::createContentArea() {
    contentContainer = lv_obj_create(screen);
    lv_obj_set_size(contentContainer, LV_PCT(100), LV_PCT(100) - HEADER_HEIGHT);
    lv_obj_set_pos(contentContainer, 0, HEADER_HEIGHT);
    lv_obj_set_style_bg_opa(contentContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(contentContainer, 0, 0);
    lv_obj_set_style_pad_all(contentContainer, MARGIN, 0);
    lv_obj_set_style_pad_row(contentContainer, ITEM_GAP, 0);
    lv_obj_set_flex_flow(contentContainer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(contentContainer, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(contentContainer, LV_DIR_VER);
}

void DeviceSelectorScreen::createLoadingSpinner() {
    spinnerContainer = lv_obj_create(screen);
    lv_obj_set_size(spinnerContainer, LV_PCT(100), LV_PCT(100) - HEADER_HEIGHT);
    lv_obj_set_pos(spinnerContainer, 0, HEADER_HEIGHT);
    lv_obj_set_style_bg_opa(spinnerContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(spinnerContainer, 0, 0);

    spinner = lv_spinner_create(spinnerContainer);
    lv_obj_set_size(spinner, 48, 48);
    lv_obj_center(spinner);
    lv_spinner_set_anim_params(spinner, 1000, 270);
    lv_obj_set_style_arc_color(spinner, lv_color_hex(COLOR_GREEN), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(spinner, lv_color_hex(COLOR_SURFACE), LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(spinner, 4, LV_PART_MAIN);

    loadingLabel = lv_label_create(spinnerContainer);
    lv_obj_set_style_text_font(loadingLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(loadingLabel, lv_color_hex(COLOR_SECONDARY), 0);
    lv_label_set_text_static(loadingLabel, "Searching for devices...");
    lv_obj_align(loadingLabel, LV_ALIGN_CENTER, 0, 40);

    // Hidden by default
    lv_obj_add_flag(spinnerContainer, LV_OBJ_FLAG_HIDDEN);
}

void DeviceSelectorScreen::createEmptyMessage() {
    emptyContainer = lv_obj_create(screen);
    lv_obj_set_size(emptyContainer, LV_PCT(100), LV_PCT(100) - HEADER_HEIGHT);
    lv_obj_set_pos(emptyContainer, 0, HEADER_HEIGHT);
    lv_obj_set_style_bg_opa(emptyContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(emptyContainer, 0, 0);

    emptyLabel = lv_label_create(emptyContainer);
    lv_obj_set_style_text_font(emptyLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(emptyLabel, lv_color_hex(COLOR_SECONDARY), 0);
    lv_label_set_text_static(emptyLabel, "No devices found");
    lv_obj_center(emptyLabel);

    // Hidden by default
    lv_obj_add_flag(emptyContainer, LV_OBJ_FLAG_HIDDEN);
}

// ---------------------------------------------------------------------------
// Device List Management
// ---------------------------------------------------------------------------

void DeviceSelectorScreen::clearDeviceList() {
    lv_obj_clean(contentContainer);
}

void DeviceSelectorScreen::populateDeviceList(
        const std::vector<SpotifyClient::DeviceInfo>& devices) {
    clearDeviceList();
    cachedDevices = devices;

    for (int i = 0; i < (int)devices.size(); ++i) {
        createDeviceItem(contentContainer, devices[i], i);
    }
}

void DeviceSelectorScreen::createDeviceItem(
        lv_obj_t* parent,
        const SpotifyClient::DeviceInfo& device,
        int index) {

    // Item container (button for click handling)
    lv_obj_t* item = lv_button_create(parent);
    lv_obj_set_size(item, LV_PCT(100), ITEM_HEIGHT);
    lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_SURFACE), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(item, ITEM_RADIUS, 0);
    lv_obj_set_style_border_width(item, 0, 0);
    lv_obj_set_style_pad_left(item, MARGIN, 0);
    lv_obj_set_style_pad_right(item, MARGIN, 0);
    lv_obj_set_style_pad_top(item, 10, 0);
    lv_obj_set_style_pad_bottom(item, 10, 0);

    // Pressed state feedback
    lv_obj_set_style_bg_color(item, lv_color_hex(0x3E3E3E), LV_STATE_PRESSED);

    // ---- Device name ----
    lv_obj_t* nameLabel = lv_label_create(item);
    lv_obj_set_style_text_font(nameLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_width(nameLabel, LV_PCT(85));
    lv_label_set_long_mode(nameLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(nameLabel, device.name.c_str());

    if (device.isActive) {
        // Active device: green text
        lv_obj_set_style_text_color(nameLabel, lv_color_hex(COLOR_GREEN), 0);
    } else {
        lv_obj_set_style_text_color(nameLabel, lv_color_white(), 0);
    }

    lv_obj_align(nameLabel, LV_ALIGN_LEFT_MID, 0, -8);

    // ---- Device type ----
    lv_obj_t* typeLabel = lv_label_create(item);
    lv_obj_set_style_text_font(typeLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(typeLabel, lv_color_hex(COLOR_SECONDARY), 0);
    lv_obj_set_width(typeLabel, LV_PCT(70));
    lv_label_set_long_mode(typeLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(typeLabel, device.type.c_str());
    lv_obj_align(typeLabel, LV_ALIGN_LEFT_MID, 0, 10);

    // ---- Active indicator (small green dot) ----
    if (device.isActive) {
        lv_obj_t* dot = lv_obj_create(item);
        lv_obj_set_size(dot, 8, 8);
        lv_obj_set_style_radius(dot, 4, 0);
        lv_obj_set_style_bg_color(dot, lv_color_hex(COLOR_GREEN), 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dot, 0, 0);
        lv_obj_align(dot, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_clear_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    }

    // Store the device index in user data so the click handler can look it up.
    // We encode the index as a pointer-sized integer.
    lv_obj_add_event_cb(item, onDeviceItemClicked,
                        LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(item, reinterpret_cast<void*>(static_cast<intptr_t>(index)));
}

// ---------------------------------------------------------------------------
// Loading / Empty State Helpers
// ---------------------------------------------------------------------------

void DeviceSelectorScreen::showLoading() {
    lv_obj_add_flag(contentContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(emptyContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(spinnerContainer, LV_OBJ_FLAG_HIDDEN);
}

void DeviceSelectorScreen::hideLoading() {
    lv_obj_add_flag(spinnerContainer, LV_OBJ_FLAG_HIDDEN);
}

void DeviceSelectorScreen::showEmptyMessage() {
    lv_obj_add_flag(contentContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(spinnerContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(emptyContainer, LV_OBJ_FLAG_HIDDEN);
}

void DeviceSelectorScreen::hideEmptyMessage() {
    lv_obj_add_flag(emptyContainer, LV_OBJ_FLAG_HIDDEN);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void DeviceSelectorScreen::refreshDevices() {
    showLoading();

    // Schedule the network call on the main loop so the spinner can render.
    App::getInstance().scheduleTask([this]() {
        SpotifyClient* spotify = App::getInstance().getSpotifyClient();
        if (!spotify) {
            hideLoading();
            showEmptyMessage();
            return;
        }

        std::vector<SpotifyClient::DeviceInfo> devices = spotify->getDevices();

        hideLoading();

        if (devices.empty()) {
            showEmptyMessage();
            clearDeviceList();
        } else {
            hideEmptyMessage();
            lv_obj_clear_flag(contentContainer, LV_OBJ_FLAG_HIDDEN);
            populateDeviceList(devices);
        }
    });
}

// ---------------------------------------------------------------------------
// Event Handlers
// ---------------------------------------------------------------------------

void DeviceSelectorScreen::onBackClicked(lv_event_t* e) {
    (void)e;
    MenuManager::getInstance().goBack();
}

void DeviceSelectorScreen::onRefreshClicked(lv_event_t* e) {
    auto* self = static_cast<DeviceSelectorScreen*>(lv_event_get_user_data(e));
    if (self) {
        self->refreshDevices();
    }
}

void DeviceSelectorScreen::onDeviceItemClicked(lv_event_t* e) {
    auto* self = static_cast<DeviceSelectorScreen*>(lv_event_get_user_data(e));
    if (!self) return;

    lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
    int index = static_cast<int>(reinterpret_cast<intptr_t>(lv_obj_get_user_data(target)));

    if (index < 0 || index >= (int)self->cachedDevices.size()) return;

    const SpotifyClient::DeviceInfo& device = self->cachedDevices[index];

    // Don't transfer if already the active device
    if (device.isActive) return;

    SpotifyClient* spotify = App::getInstance().getSpotifyClient();
    if (!spotify) return;

    // Show loading while transferring
    MenuManager::getInstance().showLoading("Transferring...");

    App::getInstance().scheduleTask([self, deviceId = device.id]() {
        SpotifyClient* spotify = App::getInstance().getSpotifyClient();
        if (spotify) {
            bool success = spotify->setDevice(deviceId);
            MenuManager::getInstance().hideLoading();

            if (success) {
                // Refresh the list to reflect the new active device
                self->refreshDevices();
            } else {
                MenuManager::getInstance().showError("Failed to transfer playback");
            }
        } else {
            MenuManager::getInstance().hideLoading();
            MenuManager::getInstance().showError("Spotify client unavailable");
        }
    });
}

} // namespace ui
