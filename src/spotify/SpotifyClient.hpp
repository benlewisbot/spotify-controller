/**
 * @file SpotifyClient.hpp
 * @brief Spotify API Client
 *
 * Handles all Spotify API interactions including
 * playback control, track info, and image loading.
 */

#ifndef SPOTIFY_CLIENT_HPP
#define SPOTIFY_CLIENT_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "AuthManager.hpp"
#include "errors.h"
#include "result.h"

// Spotify API endpoints
#define SPOTIFY_API_BASE "https://api.spotify.com/v1"
#define SPOTIFY_TOKEN_URL "https://accounts.spotify.com/api/token"

// SPOTIFY_SCOPES is defined in AuthManager.hpp (included above)

// Poll interval for now playing updates
#define SPOTIFY_POLL_INTERVAL_MS 2000

/**
 * @brief Spotify Client Class
 */
class SpotifyClient {
public:
    /**
     * @brief Track information structure
     */
    struct TrackInfo {
        String id;
        String uri;
        String title;
        String artist;
        String album;
        String albumId;
        String coverUrl;
        String coverUrlSmall;
        String coverUrlLarge;

        bool isPlaying;
        int progressMs;
        int durationMs;

        int volumePercent;

        bool saved;
        bool explicitContent;

        // Playback state (from /me/player)
        bool shuffleState;
        int repeatMode;  // 0=off, 1=context, 2=track

        TrackInfo()
            : isPlaying(false)
            , progressMs(0)
            , durationMs(0)
            , volumePercent(50)
            , saved(false)
            , explicitContent(false)
            , shuffleState(false)
            , repeatMode(0) {
        }
    };

    /**
     * @brief Playlist information structure
     */
    struct PlaylistInfo {
        String id;
        String uri;
        String name;
        String owner;
        String coverUrl;
        int trackCount;
        bool isCollaborative;
    };

    /**
     * @brief Device information structure
     */
    struct DeviceInfo {
        String id;
        String name;
        String type;
        bool isActive;
        int volumePercent;
    };

    SpotifyClient(AuthManager* auth);
    ~SpotifyClient();

    /**
     * @brief Initialize the client
     */
    void init();

    /**
     * @brief Set access and refresh tokens
     */
    void setTokens(const String& accessToken, const String& refreshToken);

    /**
     * @brief Get access token
     */
    String getAccessToken() const { return accessToken; }

    /**
     * @brief Get refresh token
     */
    String getRefreshToken() const { return refreshToken; }

    /**
     * @brief Check if authenticated
     */
    bool isAuthenticated() const { return !accessToken.isEmpty(); }

    /**
     * @brief Update now playing info
     * @return true if successful
     */
    bool updateNowPlaying();

    /**
     * @brief Update now playing info with error details
     * @return Status with error information
     */
    Status updateNowPlayingEx();

    /**
     * @brief Get current track info
     */
    TrackInfo getCurrentTrack() const { return currentTrack; }

    // Playback controls (bool - backward compatibility)
    bool play();
    bool pause();
    bool togglePlay();
    bool nextTrack();
    bool previousTrack();
    bool seek(int positionMs);

    // Playback controls (Status - with error details)
    Status playEx();
    Status pauseEx();
    Status togglePlayEx();
    Status nextTrackEx();
    Status previousTrackEx();
    Status seekEx(int positionMs);

    // Shuffle and repeat
    bool setShuffle(bool state);
    bool setRepeat(const String& state); // "off", "context", "track"
    Status setShuffleEx(bool state);
    Status setRepeatEx(const String& state);

    // Volume control
    bool setVolume(int volumePercent);
    bool adjustVolume(int delta);
    int getVolume();

    // Volume control (Status - with error details)
    Status setVolumeEx(int volumePercent);

    // Track management
    bool saveTrack(const String& trackId);
    bool removeTrack(const String& trackId);
    bool isTrackSaved(const String& trackId);

    // Track management (Status - with error details)
    Status saveTrackEx(const String& trackId);
    Status removeTrackEx(const String& trackId);
    Result<bool> isTrackSavedEx(const String& trackId);

    // Device management
    std::vector<DeviceInfo> getDevices();
    bool setDevice(const String& deviceId);
    DeviceInfo getCurrentDevice();

    // Device management (Result/Status - with error details)
    Result<std::vector<DeviceInfo>> getDevicesEx();
    Status setDeviceEx(const String& deviceId);

    // Playlists
    std::vector<PlaylistInfo> getPlaylists();
    PlaylistInfo getPlaylist(const String& playlistId);
    bool playPlaylist(const String& playlistId, const String& deviceId = "");
    bool playTrack(const String& trackUri, const String& deviceId = "");

    // Playlists (Result/Status - with error details)
    Result<std::vector<PlaylistInfo>> getPlaylistsEx();
    Result<PlaylistInfo> getPlaylistEx(const String& playlistId);
    Status playPlaylistEx(const String& playlistId, const String& deviceId = "");
    Status playTrackEx(const String& trackUri, const String& deviceId = "");

    // Search
    struct SearchResult {
        std::vector<TrackInfo> tracks;
        std::vector<PlaylistInfo> playlists;
    };
    SearchResult search(const String& query, int limit = 20);

    // Search (Result - with error details)
    Result<SearchResult> searchEx(const String& query, int limit = 20);

    // Image loading
    bool downloadImage(const String& url, const String& path);

    // Image loading (Status - with error details)
    Status downloadImageEx(const String& url, const String& path);

private:
    /**
     * @brief Make authenticated HTTP GET request
     */
    bool httpGet(const String& endpoint, JsonDocument& doc, int expectedCode = 200);

    /**
     * @brief Make authenticated HTTP GET request with error details
     */
    HttpResult httpGetEx(const String& endpoint);

    /**
     * @brief Make authenticated HTTP PUT request
     */
    bool httpPut(const String& endpoint, const String& body = "", int expectedCode = 204);

    /**
     * @brief Make authenticated HTTP PUT request with error details
     */
    HttpResult httpPutEx(const String& endpoint, const String& body = "");

    /**
     * @brief Make authenticated HTTP POST request
     */
    bool httpPost(const String& endpoint, const String& body = "", int expectedCode = 201);

    /**
     * @brief Make authenticated HTTP POST request with error details
     */
    HttpResult httpPostEx(const String& endpoint, const String& body = "");

    /**
     * @brief Make authenticated HTTP DELETE request
     */
    bool httpDelete(const String& endpoint, int expectedCode = 200);

    /**
     * @brief Make authenticated HTTP DELETE request with error details
     */
    HttpResult httpDeleteEx(const String& endpoint);

    /**
     * @brief Enforce rate limiting
     */
    void enforceRateLimit();

    /**
     * @brief Refresh access token if needed
     */
    bool ensureValidToken();

    /**
     * @brief Check and refresh token if expired
     */
    bool refreshTokenIfNeeded();

    /**
     * @brief Parse track from JSON
     */
    TrackInfo parseTrack(JsonObject trackJson);

    /**
     * @brief Parse playlist from JSON
     */
    PlaylistInfo parsePlaylist(JsonObject playlistJson);

    /**
     * @brief URL-encode a string for query parameters
     */
    static String urlEncode(const String& str);

    // Auth manager
    AuthManager* authManager;

    // Tokens
    String accessToken;
    String refreshToken;
    unsigned long tokenAcquiredAt;   // When token was acquired (overflow-safe)
    unsigned long tokenValidForMs;   // How long token is valid in ms
    unsigned long tokenExpiryTime;   // For backwards compatibility (deprecated)

    // HTTP client
    WiFiClientSecure client;
    HTTPClient http;

    // Current state
    TrackInfo currentTrack;
    DeviceInfo currentDevice;

    // Rate limiting
    unsigned long lastRequestTime;
    static constexpr unsigned long MIN_REQUEST_INTERVAL_MS = 100; // Min 100ms between requests

    // State tracking
    bool initialized;
};

#endif // SPOTIFY_CLIENT_HPP
