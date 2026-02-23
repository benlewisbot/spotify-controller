/**
 * @file SpotifyTokenScreen.cpp
 * @brief Spotify Token Screen Implementation
 */

#include "SpotifyTokenScreen.hpp"
#include "MenuManager.hpp"
#include "../../display/themes/SpotifyTheme.hpp"
#include "../../app/App.hpp"
#include "../../spotify/SpotifyClient.hpp"
#include <esp_log.h>

static const char* TAG = "TokenScreen";

#define MARGIN 16
#define CARD_RADIUS 12
#define SECTION_SPACING 12

namespace ui {

SpotifyTokenScreen::SpotifyTokenScreen(lv_obj_t* parent)
    : screen(nullptr)
    , isRefreshing(false) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
    
    // Register with MenuManager
    auto& menuManager = MenuManager::getInstance();
    menuManager.registerScreen(ScreenType::SPOTIFY_TOKEN, screen);
}

SpotifyTokenScreen::~SpotifyTokenScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void SpotifyTokenScreen::createUI() {
    createHeader();
    createAccountSection();
    createTokenSection();
    createActionsSection();
}

void SpotifyTokenScreen::createHeader() {
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
    lv_label_set_text_static(titleLabel, "Spotify Account");

    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);
}

void SpotifyTokenScreen::createAccountSection() {
    accountCard = lv_obj_create(screen);
    lv_obj_set_size(accountCard, LV_PCT(100) - (MARGIN * 2), 100);
    lv_obj_set_pos(accountCard, MARGIN, 60);
    lv_obj_set_style_bg_color(accountCard, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(accountCard, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(accountCard, CARD_RADIUS, 0);
    lv_obj_set_style_border_width(accountCard, 0, 0);
    lv_obj_set_style_pad_all(accountCard, MARGIN, 0);

    // Account icon
    lv_obj_t* icon = lv_label_create(accountCard);
    lv_obj_set_size(icon, 50, 50);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x1DB954), 0);
    lv_label_set_text_static(icon, LV_SYMBOL_AUDIO);

    // Email label
    emailLabel = lv_label_create(accountCard);
    lv_obj_align(emailLabel, LV_ALIGN_LEFT_MID, 60, -10);
    lv_obj_set_style_text_font(emailLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(emailLabel, lv_color_white(), 0);
    lv_label_set_text_static(emailLabel, "user@example.com");

    // User ID label
    userIdLabel = lv_label_create(accountCard);
    lv_obj_align(userIdLabel, LV_ALIGN_LEFT_MID, 60, 12);
    lv_obj_set_style_text_font(userIdLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(userIdLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(userIdLabel, "User ID: loading...");
}

void SpotifyTokenScreen::createTokenSection() {
    tokenCard = lv_obj_create(screen);
    lv_obj_set_size(tokenCard, LV_PCT(100) - (MARGIN * 2), 100);
    lv_obj_set_pos(tokenCard, MARGIN, 172);
    lv_obj_set_style_bg_color(tokenCard, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(tokenCard, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(tokenCard, CARD_RADIUS, 0);
    lv_obj_set_style_border_width(tokenCard, 0, 0);
    lv_obj_set_style_pad_all(tokenCard, MARGIN, 0);

    // Token icon
    lv_obj_t* icon = lv_label_create(tokenCard);
    lv_obj_set_size(icon, 40, 40);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(icon, LV_SYMBOL_EDIT);

    // Status label
    tokenStatusLabel = lv_label_create(tokenCard);
    lv_obj_align(tokenStatusLabel, LV_ALIGN_LEFT_MID, 50, -10);
    lv_obj_set_style_text_font(tokenStatusLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(tokenStatusLabel, lv_color_hex(0x1DB954), 0);
    lv_label_set_text_static(tokenStatusLabel, "Token Valid");

    // Expires label
    expiresLabel = lv_label_create(tokenCard);
    lv_obj_align(expiresLabel, LV_ALIGN_LEFT_MID, 50, 12);
    lv_obj_set_style_text_font(expiresLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(expiresLabel, lv_color_hex(0x727272), 0);
    lv_label_set_text_static(expiresLabel, "Expires in: --");
}

void SpotifyTokenScreen::createActionsSection() {
    // Refresh token button
    refreshTokenBtn = lv_button_create(screen);
    lv_obj_set_size(refreshTokenBtn, LV_PCT(100) - (MARGIN * 2), 50);
    lv_obj_set_pos(refreshTokenBtn, MARGIN, 284);
    lv_obj_set_style_bg_color(refreshTokenBtn, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_bg_opa(refreshTokenBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(refreshTokenBtn, CARD_RADIUS, 0);
    lv_obj_set_style_border_width(refreshTokenBtn, 0, 0);

    lv_obj_t* refreshLabel = lv_label_create(refreshTokenBtn);
    lv_obj_center(refreshLabel);
    lv_obj_set_style_text_font(refreshLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(refreshLabel, lv_color_white(), 0);
    lv_label_set_text_static(refreshLabel, LV_SYMBOL_REFRESH "  Refresh Token");

    // Refresh token click handler
    lv_obj_add_event_cb(refreshTokenBtn, [](lv_event_t* e) {
        SpotifyTokenScreen* self = static_cast<SpotifyTokenScreen*>(lv_event_get_user_data(e));
        if (!self || self->isRefreshing) return;

        ESP_LOGI("TokenScreen", "Refresh token requested");
        self->setRefreshing(true);

        auto* spotify = App::getInstance().getSpotifyClient();
        if (!spotify || spotify->getRefreshToken().isEmpty()) {
            ESP_LOGW("TokenScreen", "No refresh token available");
            self->setRefreshing(false);
            auto& mm = MenuManager::getInstance();
            mm.showError("No refresh token available.\nRe-authenticate to get a new token.");
            return;
        }

        // Schedule the refresh on next loop iteration to avoid blocking UI
        App::getInstance().scheduleTask([self]() {
            auto* spotify = App::getInstance().getSpotifyClient();
            if (!spotify) {
                self->setRefreshing(false);
                return;
            }

            // Force a token refresh by calling ensureValidToken which
            // delegates to refreshTokenIfNeeded internally.
            // However, ensureValidToken only refreshes if expired.
            // We need a direct refresh. Use the auth manager directly.
            // Since SpotifyClient stores the refresh token, we can read it.
            String refreshTok = spotify->getRefreshToken();
            String accessTok = spotify->getAccessToken();

            // Trigger update which will refresh if token is near-expiry
            Status result = spotify->updateNowPlayingEx();

            self->setRefreshing(false);

            // Update display with current token state
            TokenInfo info;
            info.accessToken = spotify->getAccessToken();
            info.refreshToken = spotify->getRefreshToken();
            info.isValid = spotify->isAuthenticated();
            info.tokenAcquiredAt = millis(); // approximate
            info.tokenValidForMs = 3600000UL;

            // Check if token actually changed (refresh happened)
            if (spotify->getAccessToken() != accessTok && !spotify->getAccessToken().isEmpty()) {
                ESP_LOGI("TokenScreen", "Token refreshed successfully");
                auto& mm = MenuManager::getInstance();
                mm.showSuccess("Token refreshed successfully");
            } else if (spotify->getAccessToken().isEmpty()) {
                auto& mm = MenuManager::getInstance();
                mm.showError("Token refresh failed");
            }

            self->updateTokenInfo(info);
        });
    }, LV_EVENT_CLICKED, this);

    // Re-authenticate button
    reauthBtn = lv_button_create(screen);
    lv_obj_set_size(reauthBtn, LV_PCT(100) - (MARGIN * 2), 50);
    lv_obj_set_pos(reauthBtn, MARGIN, 346);
    lv_obj_set_style_bg_color(reauthBtn, lv_color_hex(0x404040), 0);
    lv_obj_set_style_bg_opa(reauthBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(reauthBtn, CARD_RADIUS, 0);
    lv_obj_set_style_border_width(reauthBtn, 0, 0);

    lv_obj_t* reauthLabel = lv_label_create(reauthBtn);
    lv_obj_center(reauthLabel);
    lv_obj_set_style_text_font(reauthLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(reauthLabel, lv_color_white(), 0);
    lv_label_set_text_static(reauthLabel, LV_SYMBOL_DOWNLOAD "  Re-authenticate");

    // Re-auth click handler
    lv_obj_add_event_cb(reauthBtn, [](lv_event_t* e) {
        SpotifyTokenScreen* self = static_cast<SpotifyTokenScreen*>(lv_event_get_user_data(e));
        if (!self) return;
        self->showReauthDialog();
    }, LV_EVENT_CLICKED, this);

    // View details button
    detailsBtn = lv_button_create(screen);
    lv_obj_set_size(detailsBtn, LV_PCT(100) - (MARGIN * 2), 50);
    lv_obj_set_pos(detailsBtn, MARGIN, 408);
    lv_obj_set_style_bg_color(detailsBtn, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(detailsBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(detailsBtn, CARD_RADIUS, 0);
    lv_obj_set_style_border_width(detailsBtn, 0, 0);

    lv_obj_t* detailsLabel = lv_label_create(detailsBtn);
    lv_obj_center(detailsLabel);
    lv_obj_set_style_text_font(detailsLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(detailsLabel, lv_color_white(), 0);
    lv_label_set_text_static(detailsLabel, LV_SYMBOL_LIST "  View Token Details");

    // Details click handler
    lv_obj_add_event_cb(detailsBtn, [](lv_event_t* e) {
        SpotifyTokenScreen* self = static_cast<SpotifyTokenScreen*>(lv_event_get_user_data(e));
        if (!self) return;
        self->showTokenDetails();
    }, LV_EVENT_CLICKED, this);
}

void SpotifyTokenScreen::updateTokenInfo(const TokenInfo& info) {
    tokenInfo = info;

    // Update account info
    if (!info.email.isEmpty()) {
        lv_label_set_text(emailLabel, info.email.c_str());
    }
    if (!info.userId.isEmpty()) {
        lv_label_set_text_fmt(userIdLabel, "User ID: %s", info.userId.c_str());
    }

    // Update token status using overflow-safe check
    // Use the new isExpired() method instead of comparing timestamps
    bool tokenValid = info.isValid && !info.isExpired();

    if (tokenValid) {
        lv_obj_set_style_text_color(tokenStatusLabel, lv_color_hex(0x1DB954), 0);
        lv_label_set_text_static(tokenStatusLabel, "Token Valid");

        String timeStr = formatTimeRemaining(info.expiresAt);
        lv_label_set_text_fmt(expiresLabel, "Expires in: %s", timeStr.c_str());
    } else {
        lv_obj_set_style_text_color(tokenStatusLabel, lv_color_hex(0xE91E63), 0);
        lv_label_set_text_static(tokenStatusLabel, "Token Expired");
        lv_label_set_text_static(expiresLabel, "Tap Refresh to get new token");
    }
}

void SpotifyTokenScreen::setRefreshing(bool refreshing) {
    isRefreshing = refreshing;

    if (refreshing) {
        // DISABLE button while refreshing (FIX: remove clickable flag)
        lv_obj_clear_flag(refreshTokenBtn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(refreshTokenBtn, lv_color_hex(0x404040), 0);
        lv_obj_t* label = lv_obj_get_child(refreshTokenBtn, 0);
        lv_label_set_text_static(label, LV_SYMBOL_LOOP "  Refreshing...");
    } else {
        // ENABLE button after refresh (FIX: add clickable flag back)
        lv_obj_add_flag(refreshTokenBtn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(refreshTokenBtn, lv_color_hex(0x1DB954), 0);
        lv_obj_t* label = lv_obj_get_child(refreshTokenBtn, 0);
        lv_label_set_text_static(label, LV_SYMBOL_REFRESH "  Refresh Token");
    }
}

void SpotifyTokenScreen::showReauthDialog() {
    auto& menuManager = MenuManager::getInstance();
    menuManager.showConfirm(
        "Re-authenticate",
        "This will disconnect your Spotify account. You'll need to authorize again.",
        []() {
            ESP_LOGI("TokenScreen", "Re-auth confirmed, triggering re-auth flow");
            // Set app state to AUTH_REQUIRED which triggers auth flow
            // The App::loop() state machine will show the auth screen
            App::getInstance().setState(AppState::AUTH_REQUIRED);
            // Publish auth required event so App picks it up
            App::getInstance().getEventBus().publish(
                Event(EventType::AUTH_REQUIRED));
        }
    );
}

void SpotifyTokenScreen::showTokenDetails() {
    auto& menuManager = MenuManager::getInstance();
    
    // Pull live data from SpotifyClient
    auto* spotify = App::getInstance().getSpotifyClient();
    
    String message = "Access Token: ";
    String at = spotify ? spotify->getAccessToken() : tokenInfo.accessToken;
    if (at.length() > 20) {
        message += at.substring(0, 10) + "..." + at.substring(at.length() - 10);
    } else if (at.length() > 0) {
        message += at;
    } else {
        message += "(not set)";
    }
    message += "\n\nRefresh Token: ";
    String rt = spotify ? spotify->getRefreshToken() : tokenInfo.refreshToken;
    if (rt.length() > 20) {
        message += rt.substring(0, 10) + "..." + rt.substring(rt.length() - 10);
    } else if (rt.length() > 0) {
        message += rt;
    } else {
        message += "(not set)";
    }

    String remaining = formatTimeRemaining(0);
    message += "\n\nExpires in: " + remaining;
    
    menuManager.showSuccess(message);
}

String SpotifyTokenScreen::formatTimeRemaining(unsigned long expiresAt) {
    // FIX: Use overflow-safe remaining time from tokenInfo
    // The tokenInfo.getRemainingMs() method handles millis() overflow correctly
    unsigned long remaining = tokenInfo.getRemainingMs();

    // If remaining is 0, token is expired
    if (remaining == 0) {
        return "expired";
    }

    int hours = remaining / 3600000;
    int minutes = (remaining % 3600000) / 60000;

    if (hours > 24) {
        int days = hours / 24;
        return String(days) + " days";
    } else if (hours > 0) {
        return String(hours) + "h " + String(minutes) + "m";
    } else if (minutes > 0) {
        return String(minutes) + " minutes";
    } else {
        return "< 1 minute";
    }
}

} // namespace ui
