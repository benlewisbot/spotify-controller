/**
 * @file SpotifyClient.cpp
 * @brief Spotify API Client Implementation
 */

#include "SpotifyClient.hpp"

SpotifyClient::SpotifyClient(AuthManager* auth)
    : authManager(auth)
    , tokenExpiryTime(0)
    , initialized(false) {
}

SpotifyClient::~SpotifyClient() {
    http.end();
}

void SpotifyClient::init() {
    if (initialized) {
        return;
    }

    Serial.println("🎵 Initializing SpotifyClient...");

    // Configure HTTPS client
    client.setInsecure();  // For Spotify API (TODO: add certificate validation)

    initialized = true;
    Serial.println("✅ SpotifyClient initialized");
}

void SpotifyClient::setTokens(const String& access, const String& refresh) {
    accessToken = access;
    refreshToken = refresh;

    // Parse expiry time from JWT (simplified - in production, parse actual expiry)
    // For now, assume 1 hour expiry
    tokenExpiryTime = millis() + 3600000;

    Serial.println("🎫 Spotify tokens set");
}

bool SpotifyClient::updateNowPlaying() {
    if (!ensureValidToken()) {
        return false;
    }

    StaticJsonDocument<4096> doc;

    if (!httpGet("/me/player/currently-playing", doc, 200)) {
        // 204 means nothing is playing
        if (http.lastResponseCode() == 204) {
            currentTrack.isPlaying = false;
            return true;
        }
        return false;
    }

    // Parse response
    if (doc.containsKey("item") && doc["item"] != nullptr) {
        JsonObject item = doc["item"];
        currentTrack = parseTrack(item);
        currentTrack.isPlaying = doc["is_playing"] | false;
        currentTrack.progressMs = doc["progress_ms"] | 0;

        // Get device info
        if (doc.containsKey("device")) {
            JsonObject device = doc["device"];
            currentDevice.id = device["id"] | "";
            currentDevice.name = device["name"] | "";
            currentDevice.volumePercent = device["volume_percent"] | 50;
        }
    }

    return true;
}

bool SpotifyClient::play() {
    if (!ensureValidToken()) {
        return false;
    }

    return httpPut("/me/player/play");
}

bool SpotifyClient::pause() {
    if (!ensureValidToken()) {
        return false;
    }

    return httpPut("/me/player/pause");
}

bool SpotifyClient::togglePlay() {
    if (currentTrack.isPlaying) {
        return pause();
    } else {
        return play();
    }
}

bool SpotifyClient::nextTrack() {
    if (!ensureValidToken()) {
        return false;
    }

    return httpPost("/me/player/next", "", 204);
}

bool SpotifyClient::previousTrack() {
    if (!ensureValidToken()) {
        return false;
    }

    return httpPost("/me/player/previous", "", 204);
}

bool SpotifyClient::seek(int positionMs) {
    if (!ensureValidToken()) {
        return false;
    }

    String endpoint = "/me/player/seek?position_ms=" + String(positionMs);
    return httpPut(endpoint);
}

bool SpotifyClient::setVolume(int volumePercent) {
    if (!ensureValidToken()) {
        return false;
    }

    volumePercent = constrain(volumePercent, 0, 100);
    String endpoint = "/me/player/volume?volume_percent=" + String(volumePercent);

    bool success = httpPut(endpoint);
    if (success) {
        currentDevice.volumePercent = volumePercent;
    }

    return success;
}

bool SpotifyClient::adjustVolume(int delta) {
    int newVolume = currentDevice.volumePercent + delta;
    return setVolume(newVolume);
}

int SpotifyClient::getVolume() {
    // Try to get current volume from device
    if (!ensureValidToken()) {
        return currentDevice.volumePercent;
    }

    StaticJsonDocument<2048> doc;
    if (httpGet("/me/player", doc, 200)) {
        if (doc.containsKey("device")) {
            JsonObject device = doc["device"];
            currentDevice.volumePercent = device["volume_percent"] | 50;
        }
    }

    return currentDevice.volumePercent;
}

bool SpotifyClient::saveTrack(const String& trackId) {
    if (!ensureValidToken()) {
        return false;
    }

    String endpoint = "/me/tracks?ids=" + trackId;
    return httpPut(endpoint);
}

bool SpotifyClient::removeTrack(const String& trackId) {
    if (!ensureValidToken()) {
        return false;
    }

    String endpoint = "/me/tracks?ids=" + trackId;
    return httpDelete(endpoint);
}

bool SpotifyClient::isTrackSaved(const String& trackId) {
    if (!ensureValidToken()) {
        return false;
    }

    String endpoint = "/me/tracks/contains?ids=" + trackId;
    StaticJsonDocument<512> doc;

    if (httpGet(endpoint, doc, 200)) {
        JsonArray arr = doc.as<JsonArray>();
        if (arr.size() > 0) {
            return arr[0].as<bool>();
        }
    }

    return false;
}

std::vector<SpotifyClient::DeviceInfo> SpotifyClient::getDevices() {
    std::vector<DeviceInfo> devices;

    if (!ensureValidToken()) {
        return devices;
    }

    StaticJsonDocument<4096> doc;
    if (httpGet("/me/player/devices", doc, 200)) {
        JsonArray arr = doc["devices"];
        for (JsonObject deviceJson : arr) {
            DeviceInfo info;
            info.id = deviceJson["id"] | "";
            info.name = deviceJson["name"] | "";
            info.type = deviceJson["type"] | "";
            info.isActive = deviceJson["is_active"] | false;
            info.volumePercent = deviceJson["volume_percent"] | 50;
            devices.push_back(info);
        }
    }

    return devices;
}

bool SpotifyClient::setDevice(const String& deviceId) {
    if (!ensureValidToken()) {
        return false;
    }

    String body = "{\"device_ids\":[\"" + deviceId + "\"]}";
    return httpPut("/me/player", body);
}

SpotifyClient::DeviceInfo SpotifyClient::getCurrentDevice() {
    return currentDevice;
}

std::vector<SpotifyClient::PlaylistInfo> SpotifyClient::getPlaylists() {
    std::vector<PlaylistInfo> playlists;

    if (!ensureValidToken()) {
        return playlists;
    }

    StaticJsonDocument<8192> doc;
    if (httpGet("/me/playlists?limit=50", doc, 200)) {
        JsonArray arr = doc["items"];
        for (JsonObject playlistJson : arr) {
            playlists.push_back(parsePlaylist(playlistJson));
        }
    }

    return playlists;
}

SpotifyClient::PlaylistInfo SpotifyClient::getPlaylist(const String& playlistId) {
    PlaylistInfo info;

    if (!ensureValidToken()) {
        return info;
    }

    String endpoint = "/playlists/" + playlistId;
    StaticJsonDocument<8192> doc;

    if (httpGet(endpoint, doc, 200)) {
        info = parsePlaylist(doc);
    }

    return info;
}

bool SpotifyClient::playPlaylist(const String& playlistId, const String& deviceId) {
    if (!ensureValidToken()) {
        return false;
    }

    String body = "{\"context_uri\":\"spotify:playlist:" + playlistId + "\"}";

    if (!deviceId.isEmpty()) {
        body = "{\"device_ids\":[\"" + deviceId + "\"],\"context_uri\":\"spotify:playlist:" + playlistId + "\"}";
    }

    return httpPut("/me/player/play", body);
}

bool SpotifyClient::playTrack(const String& trackUri, const String& deviceId) {
    if (!ensureValidToken()) {
        return false;
    }

    String body = "{\"uris\":[\"" + trackUri + "\"]}";

    if (!deviceId.isEmpty()) {
        body = "{\"device_ids\":[\"" + deviceId + "\"],\"uris\":[\"" + trackUri + "\"]}";
    }

    return httpPut("/me/player/play", body);
}

SpotifyClient::SearchResult SpotifyClient::search(const String& query, int limit) {
    SearchResult result;

    if (!ensureValidToken()) {
        return result;
    }

    String encodedQuery = query;
    encodedQuery.replace(" ", "%20");

    String endpoint = "/search?q=" + encodedQuery + "&type=track,playlist&limit=" + String(limit);
    StaticJsonDocument<16384> doc;

    if (httpGet(endpoint, doc, 200)) {
        // Parse tracks
        if (doc.containsKey("tracks")) {
            JsonObject tracks = doc["tracks"];
            if (tracks.containsKey("items")) {
                JsonArray items = tracks["items"];
                for (JsonObject item : items) {
                    result.tracks.push_back(parseTrack(item["track"]));
                }
            }
        }

        // Parse playlists
        if (doc.containsKey("playlists")) {
            JsonObject playlists = doc["playlists"];
            if (playlists.containsKey("items")) {
                JsonArray items = playlists["items"];
                for (JsonObject item : items) {
                    result.playlists.push_back(parsePlaylist(item));
                }
            }
        }
    }

    return result;
}

bool SpotifyClient::downloadImage(const String& url, const String& path) {
    // TODO: Implement image download
    // This requires HTTP client and file system integration
    return false;
}

// Private methods

bool SpotifyClient::httpGet(const String& endpoint, JsonDocument& doc, int expectedCode) {
    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();

    if (httpCode == expectedCode || httpCode == 204) {
        String payload = http.getString();
        if (!payload.isEmpty() && payload.length() < 65536) {
            DeserializationError error = deserializeJson(doc, payload);
            if (error) {
                Serial.printf("⚠️  JSON parse error: %s\n", error.c_str());
                http.end();
                return false;
            }
        }
        http.end();
        return true;
    }

    Serial.printf("⚠️  HTTP %d: %s\n", httpCode, http.getString().c_str());
    http.end();

    // Token might be expired
    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    return false;
}

bool SpotifyClient::httpPut(const String& endpoint, const String& body, int expectedCode) {
    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(body);

    http.end();

    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    return httpCode == expectedCode;
}

bool SpotifyClient::httpPost(const String& endpoint, const String& body, int expectedCode) {
    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(body);

    http.end();

    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    return httpCode == expectedCode;
}

bool SpotifyClient::httpDelete(const String& endpoint, int expectedCode) {
    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.sendRequest("DELETE");

    http.end();

    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    return httpCode == expectedCode;
}

bool SpotifyClient::ensureValidToken() {
    if (accessToken.isEmpty()) {
        return false;
    }

    // Check if token is expired
    if (millis() >= tokenExpiryTime) {
        Serial.println("🔄 Token expired, refreshing...");
        return refreshTokenIfNeeded();
    }

    return true;
}

bool SpotifyClient::refreshTokenIfNeeded() {
    if (refreshToken.isEmpty()) {
        Serial.println("❌ No refresh token available");
        return false;
    }

    // Use AuthManager to refresh token
    if (authManager) {
        String newAccess = authManager->refreshAccessToken(refreshToken);
        if (!newAccess.isEmpty()) {
            accessToken = newAccess;
            tokenExpiryTime = millis() + 3600000;
            Serial.println("✅ Token refreshed");
            return true;
        }
    }

    return false;
}

SpotifyClient::TrackInfo SpotifyClient::parseTrack(JsonObject trackJson) {
    TrackInfo track;

    track.id = trackJson["id"] | "";
    track.uri = trackJson["uri"] | "";
    track.title = trackJson["name"] | "";
    track.durationMs = trackJson["duration_ms"] | 0;
    track.explicitContent = trackJson["explicit"] | false;

    // Artist
    if (trackJson.containsKey("artists") && trackJson["artists"].size() > 0) {
        track.artist = trackJson["artists"][0]["name"] | "";
    }

    // Album
    if (trackJson.containsKey("album")) {
        JsonObject album = trackJson["album"];
        track.album = album["name"] | "";
        track.albumId = album["id"] | "";

        // Images
        if (album.containsKey("images") && album["images"].size() > 0) {
            // Get largest image
            int maxSize = 0;
            for (JsonObject img : album["images"]) {
                int size = img["width"] | 0;
                if (size > maxSize) {
                    maxSize = size;
                    track.coverUrl = img["url"] | "";
                    track.coverUrlLarge = img["url"] | "";
                }
            }

            // Get smallest image (for thumbnails)
            int minSize = 999999;
            for (JsonObject img : album["images"]) {
                int size = img["width"] | 0;
                if (size > 0 && size < minSize) {
                    minSize = size;
                    track.coverUrlSmall = img["url"] | "";
                }
            }
        }
    }

    return track;
}

SpotifyClient::PlaylistInfo SpotifyClient::parsePlaylist(JsonObject playlistJson) {
    PlaylistInfo playlist;

    playlist.id = playlistJson["id"] | "";
    playlist.uri = playlistJson["uri"] | "";
    playlist.name = playlistJson["name"] | "";
    playlist.trackCount = playlistJson["tracks"]["total"] | 0;
    playlist.isCollaborative = playlistJson["collaborative"] | false;

    // Owner
    if (playlistJson.containsKey("owner")) {
        playlist.owner = playlistJson["owner"]["id"] | "";
    }

    // Cover image
    if (playlistJson.containsKey("images") && playlistJson["images"].size() > 0) {
        playlist.coverUrl = playlistJson["images"][0]["url"] | "";
    }

    return playlist;
}
