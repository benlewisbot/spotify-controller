#ifndef SPOTIFY_MANAGER_H
#define SPOTIFY_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Spotify API Endpoints
#define SPOTIFY_API_URL "https://api.spotify.com/v1"
#define SPOTIFY_TOKEN_URL "https://accounts.spotify.com/api/token"

// OAuth2 Scopes
#define SPOTIFY_SCOPES "user-read-playback-state user-modify-playback-state user-read-currently-playing user-read-playback-position user-library-read user-library-modify"

class SpotifyManager {
private:
  WiFiClientSecure client;
  HTTPClient http;
  String accessToken;
  String refreshToken;
  String clientId;
  String clientSecret;
  bool initialized;
  bool isMockMode; // Für Wokwi Simulation

public:
  struct TrackInfo {
    String title = "";
    String artist = "";
    String album = "";
    String coverUrl = "";
    bool isPlaying = false;
    int progressMs = 0;
    int durationMs = 0;
    int volumePercent = 50;
    bool saved = false;
  };

  SpotifyManager() : initialized(false), isMockMode(false) {
  }

  void init(bool mockMode = false) {
    isMockMode = mockMode;

    if (!isMockMode) {
      // WiFiClientSecure für HTTPS
      client.setInsecure(); // Für Spotify API (Zertifikate checken TODO)
    }

    Serial.println("\n🎵 Spotify Manager initialisiert");
    Serial.printf("  Mode: %s\n", isMockMode ? "Mock (Simulation)" : "Live API");

    initialized = true;
  }

  void setCredentials(String id, String secret) {
    clientId = id;
    clientSecret = secret;
    Serial.println("🔐 Spotify Credentials gesetzt");
  }

  void setTokens(String access, String refresh) {
    accessToken = access;
    refreshToken = refresh;
    Serial.println("🎫 Spotify Tokens gesetzt");
  }

  // Get Now Playing (Currently Playing)
  TrackInfo getNowPlaying() {
    TrackInfo track;

    if (!initialized) {
      Serial.println("⚠️ Spotify Manager nicht initialisiert");
      return track;
    }

    if (isMockMode) {
      // Mock Daten für Wokwi Simulation
      track = getMockTrack();
      #if DEBUG_SPOTIFY
        Serial.println("🎭 Mock Track Daten geladen");
      #endif
      return track;
    }

    // Ech Spotify API Call
    String url = String(SPOTIFY_API_URL) + "/me/player/currently-playing";

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      if (doc.containsKey("item")) {
        JsonObject item = doc["item"];
        track.title = item["name"].as<String>();
        track.artist = item["artists"][0]["name"].as<String>();
        track.album = item["album"]["name"].as<String>();

        if (item["images"].size() > 0) {
          track.coverUrl = item["images"][0]["url"].as<String>();
        }

        track.durationMs = item["duration_ms"].as<int>();
        track.progressMs = doc["progress_ms"].as<int>();
        track.isPlaying = doc["is_playing"].as<bool>();

        // Volume (aus devices endpoint)
        track.volumePercent = getVolume();
      }
    } else if (httpCode == 204) {
      // Kein Track wird abgespielt
      track.isPlaying = false;
    } else {
      Serial.printf("⚠️ Spotify API Error: %d\n", httpCode);
    }

    http.end();
    return track;
  }

  // Play/Pause Toggle
  bool togglePlay() {
    if (isMockMode) {
      Serial.println("🎭 Mock: Play/Pause toggled");
      return true;
    }

    TrackInfo current = getNowPlaying();

    String url = String(SPOTIFY_API_URL) + "/me/player/" + (current.isPlaying ? "pause" : "play");

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT("");

    http.end();
    return (httpCode == 204);
  }

  // Next Track
  bool nextTrack() {
    if (isMockMode) {
      Serial.println("🎭 Mock: Next Track");
      return true;
    }

    String url = String(SPOTIFY_API_URL) + "/me/player/next";

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST("");

    http.end();
    return (httpCode == 204);
  }

  // Previous Track
  bool previousTrack() {
    if (isMockMode) {
      Serial.println("🎭 Mock: Previous Track");
      return true;
    }

    String url = String(SPOTIFY_API_URL) + "/me/player/previous";

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST("");

    http.end();
    return (httpCode == 204);
  }

  // Volume Control
  bool setVolume(int volumePercent) {
    if (volumePercent < 0) volumePercent = 0;
    if (volumePercent > 100) volumePercent = 100;

    if (isMockMode) {
      Serial.printf("🎭 Mock: Volume auf %d%%\n", volumePercent);
      return true;
    }

    String url = String(SPOTIFY_API_URL) + "/me/player/volume?volume_percent=" + String(volumePercent);

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT("");

    http.end();
    return (httpCode == 204);
  }

  // Get Current Volume
  int getVolume() {
    if (isMockMode) {
      return 75; // Mock volume
    }

    String url = String(SPOTIFY_API_URL) + "/me/player";

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      if (doc.containsKey("device")) {
        return doc["device"]["volume_percent"].as<int>();
      }
    }

    http.end();
    return 50; // Default
  }

  // Add/Remove from Saved Tracks
  bool toggleSaveTrack() {
    if (isMockMode) {
      Serial.println("🎭 Mock: Track saved toggled");
      return true;
    }

    // Erst prüfen ob track saved ist
    TrackInfo current = getNowPlaying();

    String trackId = ""; // TODO: aus URL extrahieren

    String url;
    if (current.saved) {
      // Remove
      url = String(SPOTIFY_API_URL) + "/me/tracks?ids=" + trackId;
      http.begin(client, url);
      http.addHeader("Authorization", "Bearer " + accessToken);
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.DELETE("");
      http.end();
      return (httpCode == 200);
    } else {
      // Add
      url = String(SPOTIFY_API_URL) + "/me/tracks?ids=" + trackId;
      http.begin(client, url);
      http.addHeader("Authorization", "Bearer " + accessToken);
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.PUT("");
      http.end();
      return (httpCode == 200);
    }
  }

  // Check if Track is Saved
  bool isTrackSaved() {
    if (isMockMode) {
      return false;
    }

    String trackId = ""; // TODO: aus URL extrahieren

    String url = String(SPOTIFY_API_URL) + "/me/tracks/contains?ids=" + trackId;

    http.begin(client, url);
    http.addHeader("Authorization", "Bearer " + accessToken);

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      return payload[0] == 't';
    }

    http.end();
    return false;
  }

  // Refresh Access Token
  bool refreshTokenIfNeeded() {
    if (isMockMode) {
      return true;
    }

    // TODO: Token Refresh Logic
    Serial.println("🔄 Token Refresh nicht implementiert");
    return false;
  }

private:
  // Mock Track Daten für Wokwi Simulation (Linkin Park - Papercut)
  TrackInfo getMockTrack() {
    TrackInfo track;

    // Random track aus Liste
    int trackIndex = random(0, 5); // 0-4

    switch (trackIndex) {
      case 0:
        // Linkin Park - Papercut
        track.title = "Papercut";
        track.artist = "Linkin Park";
        track.album = "Hybrid Theory";
        track.coverUrl = "https://i.scdn.co/image/ab67616d0000b273bb0039458172c461a2930c30";
        track.durationMs = 185000; // 3:05
        track.progressMs = 90000; // 1:30
        track.isPlaying = true;
        track.volumePercent = 75;
        track.saved = false;
        break;

      case 1:
        // Linkin Park - In The End
        track.title = "In The End";
        track.artist = "Linkin Park";
        track.album = "Hybrid Theory";
        track.coverUrl = "https://i.scdn.co/image/ab67616d0000b273bb0039458172c461a2930c30";
        track.durationMs = 216000; // 3:36
        track.progressMs = 100000; // 1:40
        track.isPlaying = true;
        track.volumePercent = 60;
        track.saved = true;
        break;

      case 2:
        // Linkin Park - Numb
        track.title = "Numb";
        track.artist = "Linkin Park";
        track.album = "Meteora";
        track.coverUrl = "https://i.scdn.co/image/ab67616d0000b273bb0039458172c461a2930c30";
        track.durationMs = 183000; // 3:03
        track.progressMs = 150000; // 2:30
        track.isPlaying = true;
        track.volumePercent = 80;
        track.saved = false;
        break;

      case 3:
        // Linkin Park - One Step Closer
        track.title = "One Step Closer";
        track.artist = "Linkin Park";
        track.album = "Hybrid Theory";
        track.coverUrl = "https://i.scdn.co/image/ab67616d0000b273bb0039458172c461a2930c30";
        track.durationMs = 156000; // 2:36
        track.progressMs = 50000; // 0:50
        track.isPlaying = true;
        track.volumePercent = 70;
        track.saved = true;
        break;

      case 4:
        // Linkin Park - Faint
        track.title = "Faint";
        track.artist = "Linkin Park";
        track.album = "Meteora";
        track.coverUrl = "https://i.scdn.co/image/ab67616d0000b273bb0039458172c461a2930c30";
        track.durationMs = 162000; // 2:42
        track.progressMs = 80000; // 1:20
        track.isPlaying = false; // Paused
        track.volumePercent = 65;
        track.saved = false;
        break;
    }

    return track;
  }
};

#endif // SPOTIFY_MANAGER_H
