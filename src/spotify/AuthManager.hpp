/**
 * @file AuthManager.hpp
 * @brief Spotify OAuth 2.0 Authentication Manager
 *
 * Handles OAuth 2.0 authentication flow with PKCE support.
 * Provides captive portal for initial device setup.
 */

#ifndef AUTH_MANAGER_HPP
#define AUTH_MANAGER_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>

// Spotify Auth endpoints
#define SPOTIFY_AUTH_URL "https://accounts.spotify.com/authorize"
#define SPOTIFY_TOKEN_URL "https://accounts.spotify.com/api/token"

// Auth server settings
#define AUTH_SERVER_PORT 8080
#define AUTH_TIMEOUT_MS 300000  // 5 minutes

/**
 * @brief Auth State Enum
 */
enum class AuthState {
    NONE,
    WAITING_FOR_AUTH,
    AUTHENTICATED,
    ERROR
};

/**
 * @brief Spotify Authentication Manager Class
 */
class AuthManager {
public:
    AuthManager();
    ~AuthManager();

    /**
     * @brief Initialize with client credentials
     */
    void init(const String& clientId, const String& clientSecret);

    /**
     * @brief Update auth state (call periodically)
     */
    void update();

    /**
     * @brief Start auth server for captive portal
     */
    void startAuthServer();

    /**
     * @brief Stop auth server
     */
    void stopAuthServer();

    /**
     * @brief Get auth state
     */
    AuthState getState() const { return state; }

    /**
     * @brief Check if authenticated
     */
    bool isAuthenticated() const { return state == AuthState::AUTHENTICATED; }

    /**
     * @brief Get access token
     */
    String getAccessToken() const { return accessToken; }

    /**
     * @brief Get refresh token
     */
    String getRefreshToken() const { return refreshToken; }

    /**
     * @brief Get authorization URL
     */
    String getAuthUrl();

    /**
     * @brief Exchange auth code for tokens
     */
    bool exchangeCodeForTokens(const String& code);

    /**
     * @brief Refresh access token
     */
    String refreshAccessToken(const String& refreshToken);

    /**
     * @brief Generate PKCE code verifier
     */
    String generateCodeVerifier();

    /**
     * @brief Generate PKCE code challenge
     */
    String generateCodeChallenge(const String& verifier);

    /**
     * @brief Generate random state string
     */
    String generateState();

private:
    /**
     * @brief Handle web server requests
     */
    void handleWebServer();

    /**
     * @brief Serve index page
     */
    void handleIndex();

    /**
     * @brief Handle callback from Spotify
     */
    void handleCallback();

    /**
     * @brief Generate secure random string
     */
    String secureRandom(size_t length);

    /**
     * @brief Base64 URL encode
     */
    String base64UrlEncode(const String& input);

    /**
     * @brief Base64 URL decode
     */
    String base64UrlDecode(const String& input);

    /**
     * @brief SHA-256 hash
     */
    String sha256(const String& input);

    // Client credentials
    String clientId;
    String clientSecret;

    // Tokens
    String accessToken;
    String refreshToken;
    unsigned long tokenExpiryTime;

    // PKCE
    String codeVerifier;
    String codeChallenge;
    String state;

    // Web server
    WebServer* authServer;

    // State
    AuthState state;
    unsigned long authStartTime;

    bool initialized;
};

#endif // AUTH_MANAGER_HPP
