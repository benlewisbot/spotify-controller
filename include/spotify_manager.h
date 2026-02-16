/**
 * spotify_manager.h - Spotify API Manager
 * Behandelt alle Spotify API Interaktionen
 */

#ifndef SPOTIFY_MANAGER_H
#define SPOTIFY_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Track Information
struct TrackInfo {
    String title;
    String artist;
    String album;
    String coverUrl;
    bool isPlaying;
    int durationMs;
    int progressMs;
    int volume;
    bool isSaved;
};

class SpotifyManager {
public:
    SpotifyManager();
    
    // Authentifizierung
    bool begin();
    String getAuthorizationUrl();
    bool exchangeCodeForToken(const String& code);
    bool refreshAccessToken();
    bool hasValidToken();
    void saveToken();
    bool loadToken();
    void clearToken();
    
    // Player API
    bool getCurrentTrack(TrackInfo& track);
    bool play();
    bool pause();
    bool next();
    bool previous();
    bool seek(int positionMs);
    bool setVolume(int volume);
    bool toggleSaveTrack();
    
    // Token Management
    String getAccessToken() const { return accessToken; }
    int getTokenExpiresIn() const { return expiresIn; }
    
    // Check if track info changed
    bool trackChanged(const TrackInfo& newTrack);
    
private:
    String accessToken;
    String refreshToken;
    String clientId;
    String clientSecret;
    int expiresIn;
    unsigned long tokenAcquiredAt;
    
    TrackInfo lastTrack;
    
    // HTTP Request Helper
    bool apiRequest(const String& endpoint, const String& method = "GET", 
                   const String& body = "", JsonDocument* response = nullptr);
    bool makeRequest(WiFiClientSecure& client, const String& url,
                    const String& method, const String& body,
                    JsonDocument& response);
    String extractCoverUrl(const JsonDocument& trackJson);
    bool checkIfSaved(const String& trackId);
};

#endif // SPOTIFY_MANAGER_H
