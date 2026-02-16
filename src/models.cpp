/**
 * @file models.cpp
 * @brief Spotify Controller Data Models Implementation
 */

#include "models.h"

namespace spotify {

TrackInfo TrackInfo::fromJson(JsonObject trackJson) {
    TrackInfo track;
    
    // Basic track info
    track.id = trackJson["id"] | "";
    track.uri = trackJson["uri"] | "";
    track.title = trackJson["name"] | "";
    track.durationMs = trackJson["duration_ms"] | 0;
    track.isExplicit = trackJson["explicit"] | false;
    
    // Artist (primary)
    if (trackJson.containsKey("artists") && trackJson["artists"].size() > 0) {
        JsonArray artists = trackJson["artists"];
        track.artist = artists[0]["name"] | "";
    }
    
    // Album info
    if (trackJson.containsKey("album")) {
        JsonObject album = trackJson["album"];
        track.album = album["name"] | "";
        track.albumId = album["id"] | "";
        
        // Cover images
        if (album.containsKey("images") && album["images"].size() > 0) {
            JsonArray images = album["images"];
            
            // Get largest image (for coverUrlLarge)
            int maxSize = 0;
            for (JsonObject img : images) {
                int width = img["width"] | 0;
                int height = img["height"] | 0;
                int size = (width > height) ? width : height;
                
                if (size > maxSize) {
                    maxSize = size;
                    track.coverUrlLarge = img["url"] | "";
                    track.coverUrl = img["url"] | "";
                }
            }
            
            // Get smallest image (for thumbnails)
            int minSize = 999999;
            for (JsonObject img : images) {
                int width = img["width"] | 0;
                int height = img["height"] | 0;
                int size = (width > height) ? width : height;
                
                if (size > 0 && size < minSize) {
                    minSize = size;
                    track.coverUrlSmall = img["url"] | "";
                }
            }
            
            // If only one image, use it for all
            if (images.size() == 1) {
                track.coverUrl = track.coverUrlLarge;
                track.coverUrlSmall = track.coverUrlLarge;
            }
        }
    }
    
    return track;
}

DeviceInfo DeviceInfo::fromJson(JsonObject deviceJson) {
    DeviceInfo device;
    
    device.id = deviceJson["id"] | "";
    device.name = deviceJson["name"] | "";
    device.type = deviceJson["type"] | "";
    device.isActive = deviceJson["is_active"] | false;
    device.volumePercent = deviceJson["volume_percent"] | 50;
    
    return device;
}

PlaylistInfo PlaylistInfo::fromJson(JsonObject playlistJson) {
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
        JsonArray images = playlistJson["images"];
        playlist.coverUrl = images[0]["url"] | "";
    }
    
    return playlist;
}

} // namespace spotify
