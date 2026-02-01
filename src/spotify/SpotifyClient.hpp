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

// Spotify API endpoints
#define SPOTIFY_API_BASE "https://api.spotify.com/v1"
#define SPOTIFY_TOKEN_URL "https://accounts.spotify.com/api/token"

// OAuth scopes
#define SPOTIFY_SCOPES \
    "user-read-playback-state " \
    "user-modify-playback-state " \
    "user-read-currently-playing " \
    "user-read-playback-position " \
    "user-library-read " \
    "user-library-modify " \
    "playlist-read-private " \
    "playlist-read-collaborative"

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

        TrackInfo()
            : isPlaying(false)
            , progressMs(0)
            , durationMs(0)
            , volumePercent(50)
            , saved(false)
            , explicitContent(false) {
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
     * @brief Get current track info
     */
    TrackInfo getCurrentTrack() const { return currentTrack; }

    // Playback controls
    bool play();
    bool pause();
    bool togglePlay();
    bool nextTrack();
    bool previousTrack();
    bool seek(int positionMs);

    // Volume control
    bool setVolume(int volumePercent);
    bool adjustVolume(int delta);
    int getVolume();

    // Track management
    bool saveTrack(const String& trackId);
    bool removeTrack(const String& trackId);
    bool isTrackSaved(const String& trackId);

    // Device management
    std::vector<DeviceInfo> getDevices();
    bool setDevice(const String& deviceId);
    DeviceInfo getCurrentDevice();

    // Playlists
    std::vector<PlaylistInfo> getPlaylists();
    PlaylistInfo getPlaylist(const String& playlistId);
    bool playPlaylist(const String& playlistId, const String& deviceId = "");
    bool playTrack(const String& trackUri, const String& deviceId = "");

    // Search
    struct SearchResult {
        std::vector<TrackInfo> tracks;
        std::vector<PlaylistInfo> playlists;
    };
    SearchResult search(const String& query, int limit = 20);

    // Image loading
    bool downloadImage(const String& url, const String& path);

private:
    /**
     * @brief Make authenticated HTTP GET request
     */
    bool httpGet(const String& endpoint, JsonDocument& doc, int expectedCode = 200);

    /**
     * @brief Make authenticated HTTP PUT request
     */
    bool httpPut(const String& endpoint, const String& body = "", int expectedCode = 204);

    /**
     * @brief Make authenticated HTTP POST request
     */
    bool httpPost(const String& endpoint, const String& body = "", int expectedCode = 201);

    /**
     * @brief Make authenticated HTTP DELETE request
     */
    bool httpDelete(const String& endpoint, int expectedCode = 200);

    /**
     * @brief Refresh access token if needed
     */
    bool ensureValidToken();

    /**
     * @brief Parse track from JSON
     */
    TrackInfo parseTrack(JsonObject trackJson);

    /**
     * @brief Parse playlist from JSON
     */
    PlaylistInfo parsePlaylist(JsonObject playlistJson);

    // Auth manager
    AuthManager* authManager;

    // Tokens
    String accessToken;
    String refreshToken;
    unsigned long tokenExpiryTime;

    // HTTP client
    WiFiClientSecure client;
    HTTPClient http;

    // Current state
    TrackInfo currentTrack;
    DeviceInfo currentDevice;

    // State tracking
    bool initialized;
};

#endif // SPOTIFY_CLIENT_HPP
