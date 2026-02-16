/**
 * @file SpotifyClient.cpp
 * @brief Spotify API Client Implementation
 */

#include "SpotifyClient.hpp"
#include "SpotifySecure.hpp"
#include "result.h"

SpotifyClient::SpotifyClient(AuthManager* auth)
    : authManager(auth)
    , tokenAcquiredAt(0)
    , tokenValidForMs(0)
    , lastRequestTime(0)
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

    // Configure HTTPS client with security
    SpotifySecure::initSecureClient(client);

    initialized = true;
    Serial.println("✅ SpotifyClient initialized");
}

void SpotifyClient::setTokens(const String& access, const String& refresh) {
    accessToken = access;
    refreshToken = refresh;

    // Set overflow-safe token expiry tracking
    tokenAcquiredAt = millis();
    tokenValidForMs = 3600000UL;  // 1 hour default (in production, parse JWT expiry)

    Serial.println("🎫 Spotify tokens set");
}

bool SpotifyClient::updateNowPlaying() {
    auto status = updateNowPlayingEx();
    return status.ok();
}

Status SpotifyClient::updateNowPlayingEx() {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    HttpResult httpResult = httpGetEx("/me/player/currently-playing");

    // 204 means nothing is playing
    if (httpResult.statusCode == 204) {
        currentTrack.isPlaying = false;
        return Status::success();
    }

    if (!httpResult.ok()) {
        return Status::failure(httpResult.error, httpResult.message);
    }

    // Parse response
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, httpResult.body);
    if (error) {
        return Status::failure(SpotifyError::PARSE_ERROR, error.c_str());
    }

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

    return Status::success();
}

bool SpotifyClient::play() {
    auto status = playEx();
    return status.ok();
}

Status SpotifyClient::playEx() {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    HttpResult result = httpPutEx("/me/player/play");
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

bool SpotifyClient::pause() {
    auto status = pauseEx();
    return status.ok();
}

Status SpotifyClient::pauseEx() {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    HttpResult result = httpPutEx("/me/player/pause");
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

bool SpotifyClient::togglePlay() {
    auto status = togglePlayEx();
    return status.ok();
}

Status SpotifyClient::togglePlayEx() {
    if (currentTrack.isPlaying) {
        return pauseEx();
    } else {
        return playEx();
    }
}

bool SpotifyClient::nextTrack() {
    auto status = nextTrackEx();
    return status.ok();
}

Status SpotifyClient::nextTrackEx() {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    HttpResult result = httpPostEx("/me/player/next");
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

bool SpotifyClient::previousTrack() {
    auto status = previousTrackEx();
    return status.ok();
}

Status SpotifyClient::previousTrackEx() {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    HttpResult result = httpPostEx("/me/player/previous");
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

bool SpotifyClient::seek(int positionMs) {
    auto status = seekEx(positionMs);
    return status.ok();
}

Status SpotifyClient::seekEx(int positionMs) {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String endpoint;
    endpoint.reserve(64);
    endpoint = "/me/player/seek?position_ms=";
    endpoint += String(positionMs);

    HttpResult result = httpPutEx(endpoint);
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

bool SpotifyClient::setVolume(int volumePercent) {
    auto status = setVolumeEx(volumePercent);
    return status.ok();
}

Status SpotifyClient::setVolumeEx(int volumePercent) {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    volumePercent = constrain(volumePercent, 0, 100);
    String endpoint;
    endpoint.reserve(64);
    endpoint = "/me/player/volume?volume_percent=";
    endpoint += String(volumePercent);

    HttpResult result = httpPutEx(endpoint);
    if (result.ok()) {
        currentDevice.volumePercent = volumePercent;
        return Status::success();
    }
    return Status::failure(result.error, result.message);
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
    auto status = saveTrackEx(trackId);
    return status.ok();
}

Status SpotifyClient::saveTrackEx(const String& trackId) {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String endpoint;
    endpoint.reserve(64);
    endpoint = "/me/tracks?ids=";
    endpoint += trackId;

    HttpResult result = httpPutEx(endpoint);
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

bool SpotifyClient::removeTrack(const String& trackId) {
    auto status = removeTrackEx(trackId);
    return status.ok();
}

Status SpotifyClient::removeTrackEx(const String& trackId) {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String endpoint;
    endpoint.reserve(64);
    endpoint = "/me/tracks?ids=";
    endpoint += trackId;

    HttpResult result = httpDeleteEx(endpoint);
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

bool SpotifyClient::isTrackSaved(const String& trackId) {
    auto result = isTrackSavedEx(trackId);
    return result.ok() ? result.value : false;
}

Result<bool> SpotifyClient::isTrackSavedEx(const String& trackId) {
    if (!ensureValidToken()) {
        return Result<bool>::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String endpoint;
    endpoint.reserve(64);
    endpoint = "/me/tracks/contains?ids=";
    endpoint += trackId;

    HttpResult httpResult = httpGetEx(endpoint);
    if (!httpResult.ok()) {
        return Result<bool>::failure(httpResult.error, httpResult.message);
    }

    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, httpResult.body);
    if (error) {
        return Result<bool>::failure(SpotifyError::PARSE_ERROR, error.c_str());
    }

    JsonArray arr = doc.as<JsonArray>();
    if (arr.size() > 0) {
        return Result<bool>::success(arr[0].as<bool>());
    }

    return Result<bool>::success(false);
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

    String body;
    body.reserve(64);  // Bug #17 fix
    body = "{\"device_ids\":[\"";
    body += deviceId;
    body += "\"]}";
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

    String endpoint;
    endpoint.reserve(64);  // Bug #17 fix
    endpoint = "/playlists/";
    endpoint += playlistId;
    StaticJsonDocument<8192> doc;

    if (httpGet(endpoint, doc, 200)) {
        info = parsePlaylist(doc.as<JsonObject>());
    }

    return info;
}

bool SpotifyClient::playPlaylist(const String& playlistId, const String& deviceId) {
    if (!ensureValidToken()) {
        return false;
    }

    String body;
    body.reserve(128);  // Bug #17 fix
    body = "{\"context_uri\":\"spotify:playlist:";
    body += playlistId;
    body += "\"}";

    if (!deviceId.isEmpty()) {
        body = "{\"device_ids\":[\"";
        body += deviceId;
        body += "\"],\"context_uri\":\"spotify:playlist:";
        body += playlistId;
        body += "\"}";
    }

    return httpPut("/me/player/play", body);
}

bool SpotifyClient::playTrack(const String& trackUri, const String& deviceId) {
    if (!ensureValidToken()) {
        return false;
    }

    String body;
    body.reserve(128);  // Bug #17 fix
    body = "{\"uris\":[\"";
    body += trackUri;
    body += "\"]}";

    if (!deviceId.isEmpty()) {
        body = "{\"device_ids\":[\"";
        body += deviceId;
        body += "\"],\"uris\":[\"";
        body += trackUri;
        body += "\"]}";
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

    String endpoint;
    endpoint.reserve(256);  // Bug #17 fix
    endpoint = "/search?q=";
    endpoint += encodedQuery;
    endpoint += "&type=track,playlist&limit=";
    endpoint += String(limit);
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

// ===== Ex Methods with Error Propagation (Bug #16) =====

Result<std::vector<SpotifyClient::DeviceInfo>> SpotifyClient::getDevicesEx() {
    if (!ensureValidToken()) {
        return Result<std::vector<DeviceInfo>>::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    HttpResult httpResult = httpGetEx("/me/player/devices");
    if (!httpResult.ok()) {
        return Result<std::vector<DeviceInfo>>::failure(httpResult.error, httpResult.message);
    }

    std::vector<DeviceInfo> devices;
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, httpResult.body);

    if (error) {
        return Result<std::vector<DeviceInfo>>::failure(SpotifyError::PARSE_ERROR, error.c_str());
    }

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

    return Result<std::vector<DeviceInfo>>::success(devices);
}

Status SpotifyClient::setDeviceEx(const String& deviceId) {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String body;
    body.reserve(64);
    body = "{\"device_ids\":[\"";
    body += deviceId;
    body += "\"]}";

    HttpResult result = httpPutEx("/me/player", body);
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

Result<std::vector<SpotifyClient::PlaylistInfo>> SpotifyClient::getPlaylistsEx() {
    if (!ensureValidToken()) {
        return Result<std::vector<PlaylistInfo>>::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    HttpResult httpResult = httpGetEx("/me/playlists?limit=50");
    if (!httpResult.ok()) {
        return Result<std::vector<PlaylistInfo>>::failure(httpResult.error, httpResult.message);
    }

    std::vector<PlaylistInfo> playlists;
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, httpResult.body);

    if (error) {
        return Result<std::vector<PlaylistInfo>>::failure(SpotifyError::PARSE_ERROR, error.c_str());
    }

    JsonArray arr = doc["items"];
    for (JsonObject playlistJson : arr) {
        playlists.push_back(parsePlaylist(playlistJson));
    }

    return Result<std::vector<PlaylistInfo>>::success(playlists);
}

Result<SpotifyClient::PlaylistInfo> SpotifyClient::getPlaylistEx(const String& playlistId) {
    if (!ensureValidToken()) {
        return Result<PlaylistInfo>::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String endpoint;
    endpoint.reserve(64);
    endpoint = "/playlists/";
    endpoint += playlistId;

    HttpResult httpResult = httpGetEx(endpoint);
    if (!httpResult.ok()) {
        return Result<PlaylistInfo>::failure(httpResult.error, httpResult.message);
    }

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, httpResult.body);

    if (error) {
        return Result<PlaylistInfo>::failure(SpotifyError::PARSE_ERROR, error.c_str());
    }

    PlaylistInfo info = parsePlaylist(doc.as<JsonObject>());
    return Result<PlaylistInfo>::success(info);
}

Status SpotifyClient::playPlaylistEx(const String& playlistId, const String& deviceId) {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String body;
    body.reserve(128);

    if (!deviceId.isEmpty()) {
        body = "{\"device_ids\":[\"";
        body += deviceId;
        body += "\"],\"context_uri\":\"spotify:playlist:";
        body += playlistId;
        body += "\"}";
    } else {
        body = "{\"context_uri\":\"spotify:playlist:";
        body += playlistId;
        body += "\"}";
    }

    HttpResult result = httpPutEx("/me/player/play", body);
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

Status SpotifyClient::playTrackEx(const String& trackUri, const String& deviceId) {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String body;
    body.reserve(128);

    if (!deviceId.isEmpty()) {
        body = "{\"device_ids\":[\"";
        body += deviceId;
        body += "\"],\"uris\":[\"";
        body += trackUri;
        body += "\"]}";
    } else {
        body = "{\"uris\":[\"";
        body += trackUri;
        body += "\"]}";
    }

    HttpResult result = httpPutEx("/me/player/play", body);
    if (!result.ok()) {
        return Status::failure(result.error, result.message);
    }
    return Status::success();
}

Result<SpotifyClient::SearchResult> SpotifyClient::searchEx(const String& query, int limit) {
    if (!ensureValidToken()) {
        return Result<SearchResult>::failure(SpotifyError::AUTH_EXPIRED, "Token validation failed");
    }

    String encodedQuery = query;
    encodedQuery.replace(" ", "%20");

    String endpoint;
    endpoint.reserve(256);
    endpoint = "/search?q=";
    endpoint += encodedQuery;
    endpoint += "&type=track,playlist&limit=";
    endpoint += String(limit);

    HttpResult httpResult = httpGetEx(endpoint);
    if (!httpResult.ok()) {
        return Result<SearchResult>::failure(httpResult.error, httpResult.message);
    }

    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, httpResult.body);

    if (error) {
        return Result<SearchResult>::failure(SpotifyError::PARSE_ERROR, error.c_str());
    }

    SearchResult result;

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

    return Result<SearchResult>::success(result);
}

bool SpotifyClient::downloadImage(const String& url, const String& path) {
    Serial.printf("🖼️  Downloading image: %s\n", url.c_str());
    Serial.printf("   Saving to: %s\n", path.c_str());

    WiFiClientSecure client;
    SpotifySecure::initInsecureClient(client); // Images from various sources

    HTTPClient http;
    http.begin(client, url);
    http.addHeader("User-Agent", "SpotifyController/1.0");

    int httpCode = http.GET();

    if (httpCode != 200) {
        Serial.printf("⚠️  Image download failed: %d\n", httpCode);
        http.end();
        return false;
    }

    // Get content length
    int contentLength = http.getSize();
    Serial.printf("   Image size: %d bytes\n", contentLength);

    if (contentLength == 0 || contentLength > 500000) {
        Serial.println("⚠️  Invalid image size");
        http.end();
        return false;
    }

    // Read image data
    WiFiClient* stream = http.getStreamPtr();

    // Open LittleFS file for writing
    #ifdef LITTLEFS_PRESENT
    if (!LittleFS.begin()) {
        Serial.println("⚠️  LittleFS not available");
        http.end();
        return false;
    }

    File file = LittleFS.open(path, "w");
    if (!file) {
        Serial.println("⚠️  Failed to open file for writing");
        http.end();
        LittleFS.end();
        return false;
    }

    // Download and write data
    uint8_t buffer[1024];
    int bytesRead = 0;
    int totalRead = 0;

    while (http.connected() && (bytesRead = stream->readBytes(buffer, sizeof(buffer))) > 0) {
        file.write(buffer, bytesRead);
        totalRead += bytesRead;

        // Show progress every 10KB
        if (totalRead % 10000 == 0) {
            Serial.printf("   Progress: %d/%d bytes (%d%%)\n",
                         totalRead, contentLength, (totalRead * 100) / contentLength);
        }
    }

    file.close();
    http.end();
    LittleFS.end();

    Serial.printf("✅ Image downloaded: %d bytes saved\n", totalRead);
    return true;
    #else
    http.end();
    Serial.println("⚠️  LittleFS not available, image download skipped");
    return false;
    #endif
}

// Private methods

void SpotifyClient::enforceRateLimit() {
    // Enforce minimum time between requests
    unsigned long now = millis();
    unsigned long elapsed = now - lastRequestTime;

    // Handle millis() overflow (every ~49 days)
    // If elapsed > 1 day, assume overflow happened
    const unsigned long ONE_DAY_MS = 24UL * 60UL * 60UL * 1000UL;
    if (elapsed > ONE_DAY_MS) {
        lastRequestTime = now;
        return;
    }

    // Wait if needed
    if (elapsed < MIN_REQUEST_INTERVAL_MS) {
        delay(MIN_REQUEST_INTERVAL_MS - elapsed);
    }

    lastRequestTime = millis();
}

bool SpotifyClient::httpGet(const String& endpoint, JsonDocument& doc, int expectedCode) {
    enforceRateLimit();

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
    enforceRateLimit();

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
    enforceRateLimit();

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
    enforceRateLimit();

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

HttpResult SpotifyClient::httpGetEx(const String& endpoint) {
    enforceRateLimit();

    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();
    String payload = http.getString();

    http.end();

    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    if (httpCode >= 200 && httpCode < 300) {
        return HttpResult::success(httpCode, payload);
    } else if (httpCode < 0) {
        return HttpResult::networkFailure(httpCode, "Network error");
    } else {
        return HttpResult::failure(httpCode, SpotifyError::API_ERROR, payload);
    }
}

HttpResult SpotifyClient::httpPutEx(const String& endpoint, const String& body) {
    enforceRateLimit();

    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(body);
    String payload = http.getString();

    http.end();

    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    if (httpCode >= 200 && httpCode < 300) {
        return HttpResult::success(httpCode, payload);
    } else if (httpCode < 0) {
        return HttpResult::networkFailure(httpCode, "Network error");
    } else {
        return HttpResult::failure(httpCode, SpotifyError::API_ERROR, payload);
    }
}

HttpResult SpotifyClient::httpPostEx(const String& endpoint, const String& body) {
    enforceRateLimit();

    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(body);
    String payload = http.getString();

    http.end();

    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    if (httpCode >= 200 && httpCode < 300) {
        return HttpResult::success(httpCode, payload);
    } else if (httpCode < 0) {
        return HttpResult::networkFailure(httpCode, "Network error");
    } else {
        return HttpResult::failure(httpCode, SpotifyError::API_ERROR, payload);
    }
}

HttpResult SpotifyClient::httpDeleteEx(const String& endpoint) {
    enforceRateLimit();

    String url = String(SPOTIFY_API_BASE) + endpoint;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.sendRequest("DELETE");
    String payload = http.getString();

    http.end();

    if (httpCode == 401) {
        refreshTokenIfNeeded();
    }

    if (httpCode >= 200 && httpCode < 300) {
        return HttpResult::success(httpCode, payload);
    } else if (httpCode < 0) {
        return HttpResult::networkFailure(httpCode, "Network error");
    } else {
        return HttpResult::failure(httpCode, SpotifyError::API_ERROR, payload);
    }
}

bool SpotifyClient::ensureValidToken() {
    if (accessToken.isEmpty()) {
        return false;
    }

    // Overflow-safe token expiry check
    // Uses elapsed time (millis() - acquiredAt) which handles overflow correctly
    unsigned long now = millis();
    unsigned long elapsed = now - tokenAcquiredAt;

    if (elapsed >= tokenValidForMs) {
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
            // Use overflow-safe token tracking
            tokenAcquiredAt = millis();
            tokenValidForMs = 3600000UL;  // 1 hour
            // For backwards compatibility (deprecated field)
            tokenExpiryTime = tokenAcquiredAt + tokenValidForMs;
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
            JsonArray images = album["images"];
            for (size_t i = 0; i < images.size(); i++) {
                JsonObject img = images[i];
                int size = img["width"] | 0;
                if (size > maxSize) {
                    maxSize = size;
                    track.coverUrl = img["url"] | "";
                    track.coverUrlLarge = img["url"] | "";
                }
            }

            // Get smallest image (for thumbnails)
            int minSize = 999999;
            for (size_t i = 0; i < images.size(); i++) {
                JsonObject img = images[i];
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
