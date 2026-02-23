/**
 * @file WiFiSettingsScreen.cpp
 * @brief WiFi Settings Screen Implementation
 */

#include "WiFiSettingsScreen.hpp"
#include "MenuManager.hpp"
#include "../../display/themes/SpotifyTheme.hpp"
#include "../../network/WiFiManager.hpp"
#include "../../app/App.hpp"
#include <WiFi.h>
#include <esp_log.h>

static const char* TAG = "WiFiSettings";

#define MARGIN 16
#define ITEM_HEIGHT 56

namespace ui {

WiFiSettingsScreen::WiFiSettingsScreen(lv_obj_t* parent)
    : screen(nullptr)
    , isScanning(false)
    , connected(false) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
    
    // Register with MenuManager
    auto& menuManager = MenuManager::getInstance();
    menuManager.registerScreen(ScreenType::WIFI_SETTINGS, screen);

    // Sync current WiFi status on creation
    syncCurrentStatus();
}

WiFiSettingsScreen::~WiFiSettingsScreen() {
    if (scanTimer) {
        lv_timer_delete(scanTimer);
        scanTimer = nullptr;
    }
    if (passwordOverlay) {
        lv_obj_del(passwordOverlay);
        passwordOverlay = nullptr;
    }
    if (screen) {
        lv_obj_del(screen);
    }
}

void WiFiSettingsScreen::createUI() {
    createHeader();
    createStatusSection();
    createNetworkList();
}

void WiFiSettingsScreen::createHeader() {
    lv_obj_t* header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_PCT(100), 60);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x181818), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, MARGIN, 0);

    // Back button with event handler
    lv_obj_t* backBtn = lv_button_create(header);
    lv_obj_set_size(backBtn, 40, 40);
    lv_obj_set_style_bg_opa(backBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(backBtn, 0, 0);
    lv_obj_set_style_radius(backBtn, 20, 0);

    lv_obj_t* backLabel = lv_label_create(backBtn);
    lv_obj_center(backLabel);
    lv_obj_set_style_text_font(backLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(backLabel, lv_color_white(), 0);
    lv_label_set_text_static(backLabel, LV_SYMBOL_LEFT);

    // Add click handler for back button
    lv_obj_add_event_cb(backBtn, [](lv_event_t* e) {
        auto& menuManager = MenuManager::getInstance();
        menuManager.goBack();
    }, LV_EVENT_CLICKED, nullptr);

    // Title
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_label_set_text_static(titleLabel, "WiFi Settings");

    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);
}

void WiFiSettingsScreen::createStatusSection() {
    statusCard = lv_obj_create(screen);
    lv_obj_set_size(statusCard, LV_PCT(100) - (MARGIN * 2), 80);
    lv_obj_set_pos(statusCard, MARGIN, 60);
    lv_obj_set_style_bg_color(statusCard, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(statusCard, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(statusCard, 12, 0);
    lv_obj_set_style_border_width(statusCard, 0, 0);
    lv_obj_set_style_pad_all(statusCard, MARGIN, 0);

    // Status icon
    statusIcon = lv_label_create(statusCard);
    lv_obj_set_size(statusIcon, 40, 40);
    lv_obj_align(statusIcon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(statusIcon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(statusIcon, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(statusIcon, LV_SYMBOL_WIFI);

    // Status label
    statusLabel = lv_label_create(statusCard);
    lv_obj_align(statusLabel, LV_ALIGN_LEFT_MID, 50, -10);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_white(), 0);
    lv_label_set_text_static(statusLabel, "Not Connected");

    // SSID label
    ssidLabel = lv_label_create(statusCard);
    lv_obj_align(ssidLabel, LV_ALIGN_LEFT_MID, 50, 12);
    lv_obj_set_style_text_font(ssidLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ssidLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(ssidLabel, "Tap Scan to find networks");

    // IP label
    ipLabel = lv_label_create(statusCard);
    lv_obj_align(ipLabel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_font(ipLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ipLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(ipLabel, "No IP");
}

void WiFiSettingsScreen::createNetworkList() {
    // Scan button
    scanButton = lv_button_create(screen);
    lv_obj_set_size(scanButton, LV_PCT(100) - (MARGIN * 2), 50);
    lv_obj_set_pos(scanButton, MARGIN, 155);
    lv_obj_set_style_bg_color(scanButton, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_bg_opa(scanButton, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(scanButton, 12, 0);
    lv_obj_set_style_border_width(scanButton, 0, 0);

    lv_obj_t* scanLabel = lv_label_create(scanButton);
    lv_obj_center(scanLabel);
    lv_obj_set_style_text_font(scanLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(scanLabel, lv_color_white(), 0);
    lv_label_set_text_static(scanLabel, LV_SYMBOL_REFRESH "  Scan for Networks");

    // Scan button click handler
    lv_obj_add_event_cb(scanButton, [](lv_event_t* e) {
        WiFiSettingsScreen* self = static_cast<WiFiSettingsScreen*>(lv_event_get_user_data(e));
        if (self) {
            self->startScan();
        }
    }, LV_EVENT_CLICKED, this);

    // Network list (scrollable container)
    networkList = lv_obj_create(screen);
    lv_obj_set_size(networkList, LV_PCT(100), LV_PCT(100) - 215);
    lv_obj_set_pos(networkList, 0, 215);
    lv_obj_set_style_bg_opa(networkList, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(networkList, 0, 0);
    lv_obj_set_style_pad_all(networkList, MARGIN, 0);
    lv_obj_set_scrollbar_mode(networkList, LV_SCROLLBAR_MODE_AUTO);

    // Empty state
    lv_obj_t* emptyLabel = lv_label_create(networkList);
    lv_obj_align(emptyLabel, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_text_font(emptyLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(emptyLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(emptyLabel, "Tap Scan to find available networks");
}

void WiFiSettingsScreen::createStaticIPSection() {
    // Static IP settings (can be expanded)
    staticIPSection = lv_obj_create(screen);
    lv_obj_set_size(staticIPSection, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_style_bg_color(staticIPSection, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(staticIPSection, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(staticIPSection, 12, 0);
    lv_obj_set_style_border_width(staticIPSection, 0, 0);
    lv_obj_set_style_pad_all(staticIPSection, MARGIN, 0);

    lv_obj_t* label = lv_label_create(staticIPSection);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text_static(label, "Static IP");

    staticIPSwitch = lv_obj_create(staticIPSection);
    lv_obj_align(staticIPSwitch, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(staticIPSwitch, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_bg_color(staticIPSwitch, lv_color_hex(0x1DB954), LV_PART_INDICATOR);
}

void WiFiSettingsScreen::syncCurrentStatus() {
    if (WiFi.status() == WL_CONNECTED) {
        updateConnectionStatus(true, WiFi.SSID());
    } else {
        WiFiManager* wm = App::getInstance().getWiFiManager();
        if (wm && wm->getState() == WiFiState::AP_MODE) {
            lv_obj_set_style_text_color(statusIcon, lv_color_hex(0xFFC107), 0);
            lv_label_set_text(statusLabel, "AP Mode");
            lv_label_set_text(ssidLabel, wm->getSSID().c_str());
            lv_label_set_text(ipLabel, wm->getIP().c_str());
        }
        // else: stays at default "Not Connected"
    }
}

void WiFiSettingsScreen::startScan() {
    if (isScanning) return;

    isScanning = true;
    networks.clear();

    ESP_LOGI(TAG, "Starting WiFi scan...");

    // Update button text to indicate scanning
    lv_obj_t* btnLabel = lv_obj_get_child(scanButton, 0);
    if (btnLabel) {
        lv_label_set_text_static(btnLabel, LV_SYMBOL_REFRESH "  Scanning...");
    }
    // Disable scan button while scanning
    lv_obj_clear_flag(scanButton, LV_OBJ_FLAG_CLICKABLE);

    // Start async WiFi scan (non-blocking)
    WiFi.scanNetworks(true);

    // Poll for scan completion using an LVGL timer (every 500ms)
    if (scanTimer) {
        lv_timer_delete(scanTimer);
        scanTimer = nullptr;
    }
    scanTimer = lv_timer_create([](lv_timer_t* timer) {
        WiFiSettingsScreen* self = static_cast<WiFiSettingsScreen*>(lv_timer_get_user_data(timer));
        if (!self) return;

        int16_t result = WiFi.scanComplete();
        if (result == WIFI_SCAN_RUNNING) {
            return; // Still scanning, wait for next tick
        }

        // Scan finished (or failed)
        self->scanTimer = nullptr;
        lv_timer_delete(timer);

        // Re-enable scan button
        lv_obj_add_flag(self->scanButton, LV_OBJ_FLAG_CLICKABLE);

        if (result == WIFI_SCAN_FAILED || result < 0) {
            self->isScanning = false;
            ESP_LOGW("WiFiSettings", "Scan failed");
            lv_obj_t* btnLabel = lv_obj_get_child(self->scanButton, 0);
            if (btnLabel) {
                lv_label_set_text_static(btnLabel, LV_SYMBOL_REFRESH "  Scan Failed - Retry");
            }
            return;
        }

        ESP_LOGI("WiFiSettings", "Scan found %d networks", result);

        // Collect results
        std::vector<NetworkInfo> found;
        found.reserve(result);
        String currentSSID = WiFi.SSID();
        for (int i = 0; i < result; i++) {
            NetworkInfo info;
            info.ssid = WiFi.SSID(i);
            if (info.ssid.isEmpty()) continue; // Skip hidden networks
            info.rssi = WiFi.RSSI(i);
            info.secured = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
            info.connected = (WiFi.status() == WL_CONNECTED && info.ssid == currentSSID);
            found.push_back(info);
        }

        WiFi.scanDelete(); // Free scan memory
        self->updateScanResults(found);
    }, 500, this);
}

void WiFiSettingsScreen::updateScanResults(const std::vector<NetworkInfo>& newNetworks) {
    networks = newNetworks;
    isScanning = false;

    // Update button label (child of scanButton)
    lv_obj_t* btnLabel = lv_obj_get_child(scanButton, 0);
    if (btnLabel) {
        lv_label_set_text_static(btnLabel, LV_SYMBOL_REFRESH "  Scan Again");
    }

    // Clear existing items
    lv_obj_clean(networkList);

    if (networks.empty()) {
        lv_obj_t* emptyLabel = lv_label_create(networkList);
        lv_obj_align(emptyLabel, LV_ALIGN_TOP_MID, 0, 40);
        lv_obj_set_style_text_font(emptyLabel, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(emptyLabel, lv_color_hex(0x727272), 0);
        lv_label_set_text_static(emptyLabel, "No networks found");
        return;
    }

    // Add network items
    int yPos = 0;
    for (const auto& network : networks) {
        createNetworkItem(network, yPos);
        yPos += ITEM_HEIGHT + 8;
    }
}

void WiFiSettingsScreen::createNetworkItem(const NetworkInfo& network, int yPos) {
    lv_obj_t* item = lv_obj_create(networkList);
    lv_obj_set_size(item, LV_PCT(100) - (MARGIN * 2), ITEM_HEIGHT);
    lv_obj_set_pos(item, MARGIN, yPos);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(item, 12, 0);
    lv_obj_set_style_border_width(item, 0, 0);
    lv_obj_set_style_pad_all(item, MARGIN, 0);

    // Signal strength indicator
    lv_obj_t* signalIcon = lv_label_create(item);
    lv_obj_set_size(signalIcon, 30, 30);
    lv_obj_align(signalIcon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(signalIcon, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(signalIcon, lv_color_hex(getSignalStrength(network.rssi)), 0);
    lv_label_set_text_fmt(signalIcon, "%s", LV_SYMBOL_WIFI);

    // SSID
    lv_obj_t* ssid = lv_label_create(item);
    lv_obj_align(ssid, LV_ALIGN_LEFT_MID, 40, -8);
    lv_obj_set_style_text_font(ssid, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(ssid, lv_color_white(), 0);
    lv_label_set_text(ssid, network.ssid.c_str());

    // Security icon + signal info
    lv_obj_t* infoLabel = lv_label_create(item);
    lv_obj_align(infoLabel, LV_ALIGN_LEFT_MID, 40, 8);
    lv_obj_set_style_text_font(infoLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(infoLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_fmt(infoLabel, "%s  %d dBm",
                          network.secured ? "Secured" : "Open",
                          network.rssi);

    // Connect button (or "Connected" indicator)
    if (network.connected) {
        lv_obj_t* connLabel = lv_label_create(item);
        lv_obj_align(connLabel, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_style_text_font(connLabel, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(connLabel, lv_color_hex(0x1DB954), 0);
        lv_label_set_text_static(connLabel, "Connected");
    } else {
        lv_obj_t* connectBtn = lv_button_create(item);
        lv_obj_set_size(connectBtn, 80, 32);
        lv_obj_align(connectBtn, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_style_bg_color(connectBtn, lv_color_hex(0x1DB954), 0);
        lv_obj_set_style_bg_opa(connectBtn, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(connectBtn, 8, 0);
        lv_obj_set_style_border_width(connectBtn, 0, 0);

        lv_obj_t* btnLabel = lv_label_create(connectBtn);
        lv_obj_center(btnLabel);
        lv_obj_set_style_text_font(btnLabel, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(btnLabel, lv_color_white(), 0);
        lv_label_set_text_static(btnLabel, "Connect");

        // Store network info for the click handler
        // We allocate a small struct on the heap with the SSID and secured flag.
        // It's freed when the networkList is cleaned (items destroyed).
        struct ConnectData {
            WiFiSettingsScreen* self;
            String ssid;
            bool secured;
        };
        ConnectData* data = new ConnectData{this, network.ssid, network.secured};

        lv_obj_add_event_cb(connectBtn, [](lv_event_t* e) {
            ConnectData* d = static_cast<ConnectData*>(lv_event_get_user_data(e));
            if (!d) return;
            ESP_LOGI("WiFiSettings", "Connect tapped: %s (secured=%d)",
                     d->ssid.c_str(), d->secured);
            if (d->secured) {
                d->self->createPasswordDialog(d->ssid);
            } else {
                d->self->connectToNetwork(d->ssid, "");
            }
        }, LV_EVENT_CLICKED, data);

        // Clean up ConnectData when the button is deleted
        lv_obj_add_event_cb(connectBtn, [](lv_event_t* e) {
            ConnectData* d = static_cast<ConnectData*>(lv_event_get_user_data(e));
            delete d;
        }, LV_EVENT_DELETE, data);
    }
}

void WiFiSettingsScreen::createPasswordDialog(const String& ssid) {
    pendingSSID = ssid;

    // Remove existing dialog if any
    if (passwordOverlay) {
        lv_obj_del(passwordOverlay);
        passwordOverlay = nullptr;
    }

    // Full-screen overlay
    passwordOverlay = lv_obj_create(lv_screen_active());
    lv_obj_set_size(passwordOverlay, 480, 480);
    lv_obj_set_pos(passwordOverlay, 0, 0);
    lv_obj_set_style_bg_color(passwordOverlay, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(passwordOverlay, LV_OPA_80, 0);
    lv_obj_set_style_border_width(passwordOverlay, 0, 0);
    lv_obj_move_foreground(passwordOverlay);

    // Dialog card
    lv_obj_t* card = lv_obj_create(passwordOverlay);
    lv_obj_set_size(card, 400, 280);
    lv_obj_center(card);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 20, 0);
    lv_obj_set_scrollbar_mode(card, LV_SCROLLBAR_MODE_OFF);

    // Title
    lv_obj_t* title = lv_label_create(card);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text_fmt(title, "Connect to %s", ssid.c_str());
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

    // Password label
    lv_obj_t* passLabel = lv_label_create(card);
    lv_obj_set_style_text_font(passLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(passLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(passLabel, "Password:");
    lv_obj_align(passLabel, LV_ALIGN_TOP_LEFT, 0, 40);

    // Password text area (LVGL keyboard-ready)
    lv_obj_t* passInput = lv_textarea_create(card);
    lv_obj_set_size(passInput, 360, 44);
    lv_obj_align(passInput, LV_ALIGN_TOP_MID, 0, 65);
    lv_textarea_set_placeholder_text(passInput, "Enter WiFi password");
    lv_textarea_set_password_mode(passInput, true);
    lv_textarea_set_one_line(passInput, true);
    lv_obj_set_style_bg_color(passInput, lv_color_hex(0x404040), 0);
    lv_obj_set_style_text_color(passInput, lv_color_white(), 0);
    lv_obj_set_style_border_color(passInput, lv_color_hex(0x1DB954), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(passInput, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(passInput, 8, 0);

    // On-screen keyboard
    lv_obj_t* kb = lv_keyboard_create(card);
    lv_obj_set_size(kb, 360, 130);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_textarea(kb, passInput);

    // Connect button row
    lv_obj_t* btnRow = lv_obj_create(card);
    lv_obj_set_size(btnRow, 360, 40);
    lv_obj_align(btnRow, LV_ALIGN_TOP_MID, 0, 115);
    lv_obj_set_style_bg_opa(btnRow, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btnRow, 0, 0);
    lv_obj_set_style_pad_all(btnRow, 0, 0);

    // Cancel button
    lv_obj_t* cancelBtn = lv_button_create(btnRow);
    lv_obj_set_size(cancelBtn, 170, 40);
    lv_obj_align(cancelBtn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(cancelBtn, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(cancelBtn, 8, 0);
    lv_obj_set_style_border_width(cancelBtn, 0, 0);

    lv_obj_t* cancelLabel = lv_label_create(cancelBtn);
    lv_obj_center(cancelLabel);
    lv_obj_set_style_text_font(cancelLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(cancelLabel, lv_color_white(), 0);
    lv_label_set_text_static(cancelLabel, "Cancel");

    lv_obj_add_event_cb(cancelBtn, [](lv_event_t* e) {
        WiFiSettingsScreen* self = static_cast<WiFiSettingsScreen*>(lv_event_get_user_data(e));
        if (self && self->passwordOverlay) {
            lv_obj_del(self->passwordOverlay);
            self->passwordOverlay = nullptr;
        }
    }, LV_EVENT_CLICKED, this);

    // Connect button
    lv_obj_t* connectBtn = lv_button_create(btnRow);
    lv_obj_set_size(connectBtn, 170, 40);
    lv_obj_align(connectBtn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(connectBtn, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_radius(connectBtn, 8, 0);
    lv_obj_set_style_border_width(connectBtn, 0, 0);

    lv_obj_t* connLabel = lv_label_create(connectBtn);
    lv_obj_center(connLabel);
    lv_obj_set_style_text_font(connLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(connLabel, lv_color_white(), 0);
    lv_label_set_text_static(connLabel, "Connect");

    // Store passInput pointer for the connect callback
    struct DialogCtx {
        WiFiSettingsScreen* self;
        lv_obj_t* input;
    };
    DialogCtx* ctx = new DialogCtx{this, passInput};

    lv_obj_add_event_cb(connectBtn, [](lv_event_t* e) {
        DialogCtx* ctx = static_cast<DialogCtx*>(lv_event_get_user_data(e));
        if (!ctx) return;

        const char* pw = lv_textarea_get_text(ctx->input);
        String password = pw ? String(pw) : "";

        ESP_LOGI("WiFiSettings", "Connecting to %s with password (len=%d)",
                 ctx->self->pendingSSID.c_str(), password.length());

        // Close dialog
        if (ctx->self->passwordOverlay) {
            lv_obj_del(ctx->self->passwordOverlay);
            ctx->self->passwordOverlay = nullptr;
        }

        ctx->self->connectToNetwork(ctx->self->pendingSSID, password);
        delete ctx;
    }, LV_EVENT_CLICKED, ctx);
}

void WiFiSettingsScreen::connectToNetwork(const String& ssid, const String& password) {
    currentSSID = ssid;

    auto& menuManager = MenuManager::getInstance();
    menuManager.showLoading("Connecting to " + ssid + "...");

    ESP_LOGI(TAG, "Connecting to: %s", ssid.c_str());

    // Use WiFiManager if available for proper state tracking
    WiFiManager* wm = App::getInstance().getWiFiManager();
    if (wm) {
        wm->connect(ssid, password);
    } else {
        // Fallback: direct WiFi connect
        if (WiFi.status() == WL_CONNECTED) {
            WiFi.disconnect();
        }
        WiFi.mode(WIFI_STA);
        if (password.isEmpty()) {
            WiFi.begin(ssid.c_str());
        } else {
            WiFi.begin(ssid.c_str(), password.c_str());
        }
    }

    // Poll connection status non-blocking via LVGL timer (every 500ms, up to 20s)
    struct ConnectCtx {
        WiFiSettingsScreen* self;
        uint32_t attempts;
    };
    ConnectCtx* ctx = new ConnectCtx{this, 0};

    lv_timer_create([](lv_timer_t* timer) {
        ConnectCtx* ctx = static_cast<ConnectCtx*>(lv_timer_get_user_data(timer));
        if (!ctx) { lv_timer_delete(timer); return; }

        ctx->attempts++;
        wl_status_t status = WiFi.status();

        if (status == WL_CONNECTED) {
            auto& mm = MenuManager::getInstance();
            mm.hideLoading();
            mm.showSuccess("Connected to " + ctx->self->currentSSID);
            ctx->self->connected = true;
            ctx->self->updateConnectionStatus(true, ctx->self->currentSSID);
            ESP_LOGI("WiFiSettings", "Connected! IP: %s",
                     WiFi.localIP().toString().c_str());
            delete ctx;
            lv_timer_delete(timer);
        } else if (status == WL_CONNECT_FAILED || status == WL_NO_SSID_AVAIL || ctx->attempts >= 40) {
            auto& mm = MenuManager::getInstance();
            mm.hideLoading();
            mm.showError("Failed to connect to " + ctx->self->currentSSID);
            ctx->self->connected = false;
            ctx->self->updateConnectionStatus(false, ctx->self->currentSSID);
            ESP_LOGW("WiFiSettings", "Connection failed (status=%d, attempts=%d)",
                     (int)status, ctx->attempts);
            delete ctx;
            lv_timer_delete(timer);
        }
        // else: still connecting, wait for next tick
    }, 500, ctx);
}

void WiFiSettingsScreen::showConnectDialog(const NetworkInfo& network) {
    if (network.secured) {
        createPasswordDialog(network.ssid);
    } else {
        connectToNetwork(network.ssid, "");
    }
}

int WiFiSettingsScreen::getSignalStrength(int rssi) {
    if (rssi >= -50) return 0x1DB954; // Excellent - Green
    if (rssi >= -60) return 0x4CAF50; // Good
    if (rssi >= -70) return 0xFFC107; // Fair - Yellow
    return 0xF44336; // Poor - Red
}

const char* WiFiSettingsScreen::getSecurityIcon(bool secured) {
    return secured ? LV_SYMBOL_SETTINGS : LV_SYMBOL_IMAGE;
}

void WiFiSettingsScreen::updateConnectionStatus(bool isConn, const String& ssid) {
    connected = isConn;
    currentSSID = ssid;
    
    if (connected) {
        lv_obj_set_style_text_color(statusIcon, lv_color_hex(0x1DB954), 0);
        lv_label_set_text(statusLabel, "Connected");
        lv_label_set_text_fmt(ssidLabel, "%s", ssid.c_str());
        lv_label_set_text(ipLabel, WiFi.localIP().toString().c_str());
    } else {
        lv_obj_set_style_text_color(statusIcon, lv_color_hex(0xB3B3B3), 0);
        lv_label_set_text(statusLabel, "Not Connected");
        lv_label_set_text_static(ssidLabel, "Tap Scan to find networks");
        lv_label_set_text_static(ipLabel, "No IP");
    }
}

void WiFiSettingsScreen::setLoading(bool loading) {
    if (loading) {
        lv_obj_add_flag(networkList, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(networkList, LV_OBJ_FLAG_HIDDEN);
    }
}

} // namespace ui
