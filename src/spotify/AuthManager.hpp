/**
 * @file AuthManager.hpp
 * @brief Spotify OAuth 2.0 Authentication Manager
 *
 * Handles OAuth 2.0 authentication flow with PKCE support.
 * Provides captive portal for initial device setup (WiFi + Spotify).
 */

#ifndef AUTH_MANAGER_HPP
#define AUTH_MANAGER_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiClientSecure.h>
#include <esp_log.h>
#include <functional>

// Spotify Auth endpoints
#define SPOTIFY_AUTH_URL "https://accounts.spotify.com/authorize"
#define SPOTIFY_TOKEN_URL "https://accounts.spotify.com/api/token"

// Spotify OAuth Scopes
#define SPOTIFY_SCOPES \
    "user-read-playback-state " \
    "user-modify-playback-state " \
    "user-read-currently-playing " \
    "user-read-playback-position " \
    "user-library-read " \
    "user-library-modify " \
    "playlist-read-private " \
    "playlist-read-collaborative"

// Auth server settings
#define AUTH_SERVER_PORT 80
#define AUTH_TIMEOUT_MS 600000  // 10 minutes

// Spotify requires HTTPS redirect URIs except for loopback addresses.
// We use 127.0.0.1 loopback — the browser will fail to connect there,
// but the user copies the redirected URL and pastes it into the device's web page.
#define SPOTIFY_REDIRECT_URI "http://127.0.0.1:8888/callback"

/**
 * @brief Auth State Enum
 */
enum class AuthState {
    NONE,
    SETUP_WIFI,          // AP mode: waiting for WiFi credentials
    SETUP_CONNECTING,    // Connecting to WiFi after setup
    WAITING_FOR_AUTH,    // WiFi connected: waiting for Spotify OAuth
    AUTHENTICATED,
    ERROR
};

/**
 * @brief Callback types for setup events
 */
using WiFiCredentialsCb = std::function<void(const String& ssid, const String& password)>;
using ClientIdCb = std::function<void(const String& clientId)>;

/**
 * @brief Spotify Authentication Manager Class
 */
class AuthManager {
public:
    AuthManager();
    ~AuthManager();

    void init(const String& clientId, const String& clientSecret);
    void update();

    // Server lifecycle
    void startAuthServer();
    void startSetupServer();   // AP mode captive portal
    void stopAuthServer();

    // State
    AuthState getState() const { return state; }
    bool isAuthenticated() const { return state == AuthState::AUTHENTICATED; }

    // Tokens
    String getAccessToken() const { return accessToken; }
    String getRefreshToken() const { return refreshToken; }

    // Auth URL
    String getAuthUrl();

    // Token operations
    bool exchangeCodeForTokens(const String& code);
    String refreshAccessToken(const String& refreshToken);
    bool isTokenExpired() const;

    // PKCE helpers
    String generateCodeVerifier();
    String generateCodeChallenge(const String& verifier);
    String generateState();

    // Set client ID (from setup form)
    void setClientId(const String& id) { clientId = id; }

    // Callbacks for setup events
    void onWiFiCredentials(WiFiCredentialsCb cb) { wifiCredentialsCb = cb; }
    void onClientIdSet(ClientIdCb cb) { clientIdCb = cb; }

    // Notify that WiFi connected (called by App after credentials applied)
    void onWiFiConnected();

private:
    // Web handlers
    void handleWebServer();
    void handleSetupPage();      // GET / in AP mode
    void handleSetupSave();      // POST /save in AP mode
    void handleIndex();          // GET / in STA mode (Spotify OAuth)
    void handleCallback();       // GET /callback from Spotify
    void handleSubmitCode();     // POST /submit-code (manual URL paste, JSON body)
    void handleAuthStatus();     // GET /auth-status (JSON polling endpoint)
    void handleCaptiveRedirect(); // Redirect all unknown to /

    // Crypto helpers
    String secureRandom(size_t length);
    String base64UrlEncode(const String& input);
    String base64UrlDecode(const String& input);
    String sha256(const String& input);

    // Client credentials
    String clientId;
    String clientSecret;

    // Tokens
    String accessToken;
    String refreshToken;
    unsigned long tokenAcquiredAt;
    unsigned long tokenValidForMs;
    unsigned long tokenExpiryTime;

    // PKCE
    String codeVerifier;
    String codeChallenge;
    String oauthState;

    // Web server + DNS
    WebServer* authServer;
    DNSServer* dnsServer;

    // State
    AuthState state;
    unsigned long authStartTime;
    bool initialized;
    bool apMode;   // true = captive portal mode, false = STA OAuth mode

    // Callbacks
    WiFiCredentialsCb wifiCredentialsCb;
    ClientIdCb clientIdCb;
};

#endif // AUTH_MANAGER_HPP
