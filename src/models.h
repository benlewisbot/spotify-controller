/**
 * @file models.h
 * @brief Spotify Controller Data Models
 *
 * Consolidated data structures for Spotify integration.
 * Compatible with ArduinoJson v7.x for parsing API responses.
 */

#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

namespace spotify {

/**
 * @brief Track Information Structure
 *
 * Represents a Spotify track with all relevant metadata.
 */
struct TrackInfo {
    String id;              // Spotify Track ID
    String uri;             // Spotify URI (spotify:track:...)
    String title;           // Track name
    String artist;          // Primary artist name
    String album;           // Album name
    String albumId;         // Album ID
    String coverUrl;        // Full-size cover image URL (640x640)
    String coverUrlSmall;   // Thumbnail cover URL (64x64)
    String coverUrlLarge;   // Extra-large cover URL (3000x3000)
    
    bool isPlaying;         // Playback state
    int progressMs;         // Current position (ms)
    int durationMs;         // Total duration (ms)
    int volumePercent;      // Current volume (0-100)
    
    bool isSaved;           // Track saved in library (liked)
    bool isExplicit;        // Explicit content flag
    
    // Constructor
    TrackInfo()
        : isPlaying(false)
        , progressMs(0)
        , durationMs(0)
        , volumePercent(50)
        , isSaved(false)
        , isExplicit(false) {
    }
    
    /**
     * @brief Parse track from JSON object
     * @param trackJson JSON object containing track data
     * @return TrackInfo struct with parsed data
     */
    static TrackInfo fromJson(JsonObject trackJson);
    
    /**
     * @brief Check if track data is valid
     */
    bool isValid() const {
        return !id.isEmpty() && !title.isEmpty();
    }
    
    /**
     * @brief Get cover URL for display (selects best size)
     */
    String getBestCoverUrl() const {
        // Prefer large size for high-DPI displays
        if (!coverUrlLarge.isEmpty()) return coverUrlLarge;
        if (!coverUrl.isEmpty()) return coverUrl;
        if (!coverUrlSmall.isEmpty()) return coverUrlSmall;
        return "";
    }
    
    /**
     * @brief Get duration in formatted string (mm:ss)
     */
    String getDurationFormatted() const {
        int minutes = durationMs / 60000;
        int seconds = (durationMs % 60000) / 1000;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, seconds);
        return String(buffer);
    }
    
    /**
     * @brief Get progress in formatted string (mm:ss)
     */
    String getProgressFormatted() const {
        int minutes = progressMs / 60000;
        int seconds = (progressMs % 60000) / 1000;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, seconds);
        return String(buffer);
    }
};

/**
 * @brief Device Information Structure
 *
 * Represents a Spotify Connect device.
 */
struct DeviceInfo {
    String id;              // Device ID
    String name;            // Device name
    String type;            // Device type (Computer, Smartphone, etc.)
    bool isActive;          // Currently active device
    int volumePercent;      // Current volume (0-100)
    
    // Constructor
    DeviceInfo()
        : isActive(false)
        , volumePercent(50) {
    }
    
    /**
     * @brief Parse device from JSON object
     */
    static DeviceInfo fromJson(JsonObject deviceJson);
    
    /**
     * @brief Check if device data is valid
     */
    bool isValid() const {
        return !id.isEmpty() && !name.isEmpty();
    }
};

/**
 * @brief Playlist Information Structure
 *
 * Represents a Spotify playlist.
 */
struct PlaylistInfo {
    String id;              // Playlist ID
    String uri;             // Spotify URI
    String name;            // Playlist name
    String owner;           // Owner ID
    String coverUrl;        // Cover image URL
    int trackCount;         // Number of tracks
    bool isCollaborative;   // Collaborative playlist
    
    // Constructor
    PlaylistInfo()
        : trackCount(0)
        , isCollaborative(false) {
    }
    
    /**
     * @brief Parse playlist from JSON object
     */
    static PlaylistInfo fromJson(JsonObject playlistJson);
    
    /**
     * @brief Check if playlist data is valid
     */
    bool isValid() const {
        return !id.isEmpty() && !name.isEmpty();
    }
};

/**
 * @brief Player State Structure
 *
 * Complete playback state including track and device info.
 */
struct PlayerState {
    TrackInfo currentTrack;
    DeviceInfo currentDevice;
    bool isPlaying;
    int shuffleState;       // 0 = off, 1 = on
    int repeatState;        // 0 = off, 1 = context, 2 = track
    int timestampMs;        // When state was fetched
    
    // Constructor
    PlayerState()
        : isPlaying(false)
        , shuffleState(0)
        , repeatState(0)
        , timestampMs(0) {
    }
    
    /**
     * @brief Check if player is active
     */
    bool isActive() const {
        return currentTrack.isValid() && currentDevice.isValid();
    }
    
    /**
     * @brief Calculate progress percentage
     */
    int getProgressPercent() const {
        if (currentTrack.durationMs <= 0) return 0;
        return (currentTrack.progressMs * 100) / currentTrack.durationMs;
    }
};

/**
 * @brief Cover Cache Info Structure
 *
 * Metadata for cached cover images.
 */
struct CoverCacheInfo {
    String trackId;         // Associated track ID
    String filePath;        // File path in LittleFS
    String url;             // Original URL
    unsigned long timestamp; // When cached
    size_t fileSize;        // File size in bytes
    
    // Constructor
    CoverCacheInfo()
        : timestamp(0)
        , fileSize(0) {
    }
    
    /**
     * @brief Check if cache entry is valid
     */
    bool isValid() const {
        return !trackId.isEmpty() && !filePath.isEmpty();
    }
    
    /**
     * @brief Check if cache entry is expired
     * @param maxAgeMs Maximum age in milliseconds
     */
    bool isExpired(unsigned long maxAgeMs) const {
        if (timestamp == 0) return true;
        unsigned long now = millis();
        return (now - timestamp) > maxAgeMs;
    }
};

/**
 * @brief Search Result Structure
 *
 * Combined search results for tracks and playlists.
 */
struct SearchResult {
    std::vector<TrackInfo> tracks;
    std::vector<PlaylistInfo> playlists;
    
    /**
     * @brief Check if search returned results
     */
    bool hasResults() const {
        return !tracks.empty() || !playlists.empty();
    }
};

} // namespace spotify

#endif // MODELS_H
