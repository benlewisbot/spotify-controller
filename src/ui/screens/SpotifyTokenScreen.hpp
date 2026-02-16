/**
 * @file SpotifyTokenScreen.hpp
 * @brief Spotify Token Management Screen
 *
 * View, refresh, and re-authenticate Spotify OAuth2 tokens.
 */

#ifndef SPOTIFY_TOKEN_SCREEN_HPP
#define SPOTIFY_TOKEN_SCREEN_HPP

#include <lvgl.h>
#include "MenuManager.hpp"
#include "RuntimeConfig.hpp"

namespace ui {

/**
 * @brief Token info structure
 */
struct TokenInfo {
    String accessToken;
    String refreshToken;
    unsigned long expiresAt;         // For backwards compatibility (deprecated)
    unsigned long tokenAcquiredAt;   // When token was acquired (overflow-safe)
    unsigned long tokenValidForMs;   // How long token is valid in ms
    bool isValid;
    String userId;
    String email;

    TokenInfo() : expiresAt(0), tokenAcquiredAt(0), tokenValidForMs(0), isValid(false) {}

    /**
     * @brief Check if token is expired (overflow-safe)
     */
    bool isExpired() const {
        unsigned long elapsed = millis() - tokenAcquiredAt;
        return elapsed >= tokenValidForMs;
    }

    /**
     * @brief Get remaining time in milliseconds (overflow-safe)
     */
    unsigned long getRemainingMs() const {
        unsigned long elapsed = millis() - tokenAcquiredAt;
        if (elapsed >= tokenValidForMs) {
            return 0;
        }
        return tokenValidForMs - elapsed;
    }
};

/**
 * @brief Spotify Token Screen Class
 */
class SpotifyTokenScreen {
public:
    SpotifyTokenScreen(lv_obj_t* parent);
    ~SpotifyTokenScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }
    
    /**
     * @brief Update token info display
     */
    void updateTokenInfo(const TokenInfo& info);
    
    /**
     * @brief Show token refresh in progress
     */
    void setRefreshing(bool refreshing);
    
    /**
     * @brief Show re-auth dialog
     */
    void showReauthDialog();
    
    /**
     * @brief Show token details
     */
    void showTokenDetails();
    
private:
    void createUI();
    void createHeader();
    void createAccountSection();
    void createTokenSection();
    void createActionsSection();
    String formatTimeRemaining(unsigned long expiresAt);
    
    lv_obj_t* screen;
    lv_obj_t* accountCard;
    lv_obj_t* emailLabel;
    lv_obj_t* userIdLabel;
    lv_obj_t* tokenCard;
    lv_obj_t* tokenStatusLabel;
    lv_obj_t* expiresLabel;
    lv_obj_t* refreshTokenBtn;
    lv_obj_t* reauthBtn;
    lv_obj_t* detailsBtn;
    
    TokenInfo tokenInfo;
    bool isRefreshing;
};

} // namespace ui

#endif // SPOTIFY_TOKEN_SCREEN_HPP
