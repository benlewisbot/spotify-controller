# API Documentation - Spotify Controller ESP32

**Version:** 1.0.0
**Date:** 2026-02-02

---

## Table of Contents

1. [SpotifyClient API](#spotifyclient-api)
2. [AuthManager API](#authmanager-api)
3. [WiFiManager API](#wifimanager-api)
4. [DisplayManager API](#displaymanager-api)
5. [UI Components API](#ui-components-api)
6. [App API](#app-api)

---

## SpotifyClient API

### Overview

`SpotifyClient` handles all interactions with the Spotify Web API, including playback control, track information retrieval, and playlist management.

### Public Methods

#### `init()`

```cpp
void init()
```

**Description:** Initializes the SpotifyClient and sets up secure HTTPS connection.

**Returns:** None

**Example:**
```cpp
SpotifyClient* spotify = new SpotifyClient(authManager);
spotify->init();
```

---

#### `setTokens()`

```cpp
void setTokens(const String& access, const String& refresh)
```

**Description:** Sets the access and refresh tokens for authentication.

**Parameters:**
- `access` - OAuth2 access token
- `refresh` - OAuth2 refresh token

**Returns:** None

**Example:**
```cpp
spotify->setTokens("access_token_abc123", "refresh_token_xyz789");
```

---

#### `updateNowPlaying()`

```cpp
bool updateNowPlaying()
```

**Description:** Fetches the currently playing track from Spotify API.

**Returns:**
- `true` - Successfully updated
- `false` - Failed (check network, token)

**Example:**
```cpp
if (spotify->updateNowPlaying()) {
    TrackInfo track = spotify->getCurrentTrack();
    Serial.printf("Now playing: %s\n", track.title.c_str());
}
```

**Side Effects:** Updates `currentTrack` member variable.

---

#### `getCurrentTrack()`

```cpp
TrackInfo getCurrentTrack() const
```

**Description:** Returns the current track information.

**Returns:** `TrackInfo` structure

**Example:**
```cpp
TrackInfo track = spotify->getCurrentTrack();
Serial.printf("Title: %s\nArtist: %s\n",
             track.title.c_str(),
             track.artist.c_str());
```

---

### Playback Controls

#### `play()`

```cpp
bool play()
```

**Description:** Starts or resumes playback.

**Returns:**
- `true` - Playback started
- `false` - Failed (no active device, etc.)

**Example:**
```cpp
if (spotify->play()) {
    Serial.println("Playback started");
}
```

---

#### `pause()`

```cpp
bool pause()
```

**Description:** Pauses playback.

**Returns:**
- `true` - Playback paused
- `false` - Failed

**Example:**
```cpp
if (spotify->pause()) {
    Serial.println("Playback paused");
}
```

---

#### `togglePlay()`

```cpp
bool togglePlay()
```

**Description:** Toggles between play and pause based on current state.

**Returns:**
- `true` - State changed
- `false` - Failed

**Example:**
```cpp
spotify->togglePlay();  // Pauses if playing, plays if paused
```

---

#### `nextTrack()`

```cpp
bool nextTrack()
```

**Description:** Skips to the next track in the queue.

**Returns:**
- `true` - Skipped to next track
- `false` - Failed

**Example:**
```cpp
if (spotify->nextTrack()) {
    Serial.println("Skipped to next track");
}
```

---

#### `previousTrack()`

```cpp
bool previousTrack()
```

**Description:** Skips to the previous track.

**Behavior:**
- If more than 3 seconds into track: Restarts current track
- If less than 3 seconds into track: Goes to previous track

**Returns:**
- `true` - Previous track playing
- `false` - Failed

**Example:**
```cpp
if (spotify->previousTrack()) {
    Serial.println("Went to previous track");
}
```

---

#### `seek()`

```cpp
bool seek(int positionMs)
```

**Description:** Seeks to a specific position in the current track.

**Parameters:**
- `positionMs` - Position in milliseconds (0 to track duration)

**Returns:**
- `true` - Seeked successfully
- `false` - Failed

**Example:**
```cpp
// Seek to 1:30 (90000ms)
if (spotify->seek(90000)) {
    Serial.println("Seeked to 1:30");
}
```

---

### Volume Control

#### `setVolume()`

```cpp
bool setVolume(int volumePercent)
```

**Description:** Sets the playback volume.

**Parameters:**
- `volumePercent` - Volume level (0-100)

**Returns:**
- `true` - Volume set
- `false` - Failed

**Example:**
```cpp
spotify->setVolume(50);  // Set to 50%
```

---

#### `adjustVolume()`

```cpp
bool adjustVolume(int delta)
```

**Description:** Adjusts volume by a delta amount.

**Parameters:**
- `delta` - Amount to adjust (-100 to +100)

**Returns:**
- `true` - Volume adjusted
- `false` - Failed

**Example:**
```cpp
spotify->adjustVolume(10);   // Increase by 10%
spotify->adjustVolume(-5);  // Decrease by 5%
```

---

#### `getVolume()`

```cpp
int getVolume()
```

**Description:** Gets the current volume level.

**Returns:** Volume level (0-100)

**Example:**
```cpp
int volume = spotify->getVolume();
Serial.printf("Current volume: %d%%\n", volume);
```

---

### Track Management

#### `saveTrack()`

```cpp
bool saveTrack(const String& trackId)
```

**Description:** Saves a track to the user's library (Liked Songs).

**Parameters:**
- `trackId` - Spotify track ID

**Returns:**
- `true` - Track saved
- `false` - Failed

**Example:**
```cpp
if (spotify->saveTrack("4iV5W9uYEdYUVa79Axb7Rh")) {
    Serial.println("Track saved to library");
}
```

---

#### `removeTrack()`

```cpp
bool removeTrack(const String& trackId)
```

**Description:** Removes a track from the user's library.

**Parameters:**
- `trackId` - Spotify track ID

**Returns:**
- `true` - Track removed
- `false` - Failed

**Example:**
```cpp
if (spotify->removeTrack("4iV5W9uYEdYUVa79Axb7Rh")) {
    Serial.println("Track removed from library");
}
```

---

#### `isTrackSaved()`

```cpp
bool isTrackSaved(const String& trackId)
```

**Description:** Checks if a track is in the user's library.

**Parameters:**
- `trackId` - Spotify track ID

**Returns:**
- `true` - Track is saved
- `false` - Track not saved or error

**Example:**
```cpp
if (spotify->isTrackSaved("4iV5W9uYEdYUVa79Axb7Rh")) {
    Serial.println("Track is in your library");
} else {
    Serial.println("Track is not in your library");
}
```

---

### Device Management

#### `getDevices()`

```cpp
std::vector<DeviceInfo> getDevices()
```

**Description:** Gets all available Spotify devices.

**Returns:** Vector of `DeviceInfo` structures

**Example:**
```cpp
std::vector<SpotifyClient::DeviceInfo> devices = spotify->getDevices();
for (const auto& device : devices) {
    Serial.printf("Device: %s (%s)\n",
                 device.name.c_str(),
                 device.isActive ? "Active" : "Inactive");
}
```

---

#### `setDevice()`

```cpp
bool setDevice(const String& deviceId)
```

**Description:** Sets the active Spotify device for playback.

**Parameters:**
- `deviceId` - Spotify device ID

**Returns:**
- `true` - Device set as active
- `false` - Failed

**Example:**
```cpp
if (spotify->setDevice("abc123def456")) {
    Serial.println("Active device set");
}
```

---

#### `getCurrentDevice()`

```cpp
DeviceInfo getCurrentDevice()
```

**Description:** Gets information about the currently active device.

**Returns:** `DeviceInfo` structure

**Example:**
```cpp
SpotifyClient::DeviceInfo device = spotify->getCurrentDevice();
Serial.printf("Active device: %s (vol: %d%%)\n",
             device.name.c_str(),
             device.volumePercent);
```

---

### Playlist Management

#### `getPlaylists()`

```cpp
std::vector<PlaylistInfo> getPlaylists()
```

**Description:** Gets the user's playlists (max 50).

**Returns:** Vector of `PlaylistInfo` structures

**Example:**
```cpp
std::vector<SpotifyClient::PlaylistInfo> playlists = spotify->getPlaylists();
for (const auto& playlist : playlists) {
    Serial.printf("Playlist: %s (%d tracks)\n",
                 playlist.name.c_str(),
                 playlist.trackCount);
}
```

---

#### `getPlaylist()`

```cpp
PlaylistInfo getPlaylist(const String& playlistId)
```

**Description:** Gets detailed information about a specific playlist.

**Parameters:**
- `playlistId` - Spotify playlist ID

**Returns:** `PlaylistInfo` structure

**Example:**
```cpp
SpotifyClient::PlaylistInfo playlist = spotify->getPlaylist("37i9dQZF1DXcBWIGoYBM5M");
Serial.printf("Playlist: %s\nOwner: %s\n",
             playlist.name.c_str(),
             playlist.owner.c_str());
```

---

#### `playPlaylist()`

```cpp
bool playPlaylist(const String& playlistId, const String& deviceId = "")
```

**Description:** Starts playing a playlist.

**Parameters:**
- `playlistId` - Spotify playlist ID
- `deviceId` - Optional: Specific device ID

**Returns:**
- `true` - Playback started
- `false` - Failed

**Example:**
```cpp
// Play on active device
if (spotify->playPlaylist("37i9dQZF1DXcBWIGoYBM5M")) {
    Serial.println("Playlist started");
}

// Play on specific device
if (spotify->playPlaylist("37i9dQZF1DXcBWIGoYBM5M", "abc123")) {
    Serial.println("Playlist started on device");
}
```

---

#### `playTrack()`

```cpp
bool playTrack(const String& trackUri, const String& deviceId = "")
```

**Description:** Plays a specific track.

**Parameters:**
- `trackUri` - Spotify track URI (e.g., "spotify:track:4iV5W9uYEdYUVa79Axb7Rh")
- `deviceId` - Optional: Specific device ID

**Returns:**
- `true` - Playback started
- `false` - Failed

**Example:**
```cpp
if (spotify->playTrack("spotify:track:4iV5W9uYEdYUVa79Axb7Rh")) {
    Serial.println("Track started");
}
```

---

### Search

#### `search()`

```cpp
SearchResult search(const String& query, int limit = 20)
```

**Description:** Searches for tracks and playlists.

**Parameters:**
- `query` - Search query string
- `limit` - Maximum results per type (default 20)

**Returns:** `SearchResult` structure

**Example:**
```cpp
SpotifyClient::SearchResult result = spotify->search("Linkin Park", 10);
Serial.printf("Found %d tracks, %d playlists\n",
             result.tracks.size(),
             result.playlists.size());

for (const auto& track : result.tracks) {
    Serial.printf("- %s by %s\n", track.title.c_str(), track.artist.c_str());
}
```

---

### Image Download

#### `downloadImage()`

```cpp
bool downloadImage(const String& url, const String& path)
```

**Description:** Downloads an image from URL and saves to LittleFS.

**Parameters:**
- `url` - Image URL
- `path` - LittleFS path to save image

**Returns:**
- `true` - Image downloaded and saved
- `false` - Failed

**Example:**
```cpp
if (spotify->downloadImage("https://example.com/image.jpg", "/covers/album.jpg")) {
    Serial.println("Image downloaded");
}
```

**Note:** Requires LittleFS support and at least 500KB free space.

---

### Data Structures

#### `TrackInfo`

```cpp
struct TrackInfo {
    String id;              // Spotify track ID
    String uri;             // Spotify track URI
    String title;           // Track title
    String artist;          // Artist name
    String album;           // Album name
    String albumId;         // Spotify album ID
    String coverUrl;        // Album art URL
    String coverUrlSmall;    // Small cover URL (thumbnail)
    String coverUrlLarge;    // Large cover URL

    bool isPlaying;         // Currently playing?
    int progressMs;         // Current position (ms)
    int durationMs;         // Track duration (ms)

    int volumePercent;       // Volume (0-100)

    bool saved;             // Saved to library?
    bool explicitContent;    // Explicit content?
};
```

---

#### `PlaylistInfo`

```cpp
struct PlaylistInfo {
    String id;              // Spotify playlist ID
    String uri;             // Spotify playlist URI
    String name;            // Playlist name
    String owner;           // Owner username
    String coverUrl;        // Cover image URL
    int trackCount;         // Number of tracks
    bool isCollaborative;   // Is collaborative?
};
```

---

#### `DeviceInfo`

```cpp
struct DeviceInfo {
    String id;              // Spotify device ID
    String name;            // Device name
    String type;            // Device type (e.g., "Smartphone")
    bool isActive;          // Currently active?
    int volumePercent;      // Volume (0-100)
};
```

---

#### `SearchResult`

```cpp
struct SearchResult {
    std::vector<TrackInfo> tracks;      // Search results for tracks
    std::vector<PlaylistInfo> playlists; // Search results for playlists
};
```

---

## AuthManager API

### Overview

`AuthManager` handles OAuth2 authentication flow with PKCE for secure Spotify authentication.

### Public Methods

#### `init()`

```cpp
void init(const String& id, const String& secret)
```

**Description:** Initializes the authentication manager with Spotify credentials.

**Parameters:**
- `id` - Spotify Client ID
- `secret` - Spotify Client Secret

**Returns:** None

**Example:**
```cpp
AuthManager* auth = new AuthManager();
auth->init("your_client_id", "your_client_secret");
```

---

#### `startAuthServer()`

```cpp
void startAuthServer()
```

**Description:** Starts the web server for OAuth2 callback handling.

**Behavior:**
- Creates web server on port 8080
- Registers `/` and `/callback` endpoints
- Generates PKCE code verifier and challenge

**Returns:** None

**Example:**
```cpp
auth->startAuthServer();
Serial.printf("Auth URL: %s\n", auth->getAuthUrl().c_str());
```

---

#### `getAuthUrl()`

```cpp
String getAuthUrl()
```

**Description:** Generates the Spotify OAuth2 authorization URL.

**Returns:** Authorization URL string

**Example:**
```cpp
String authUrl = auth->getAuthUrl();
Serial.println("Open this URL in browser:");
Serial.println(authUrl);
```

---

#### `refreshAccessToken()`

```cpp
String refreshAccessToken(const String& refreshToken)
```

**Description:** Refreshes an expired access token using the refresh token.

**Parameters:**
- `refreshToken` - OAuth2 refresh token

**Returns:** New access token (empty string on failure)

**Example:**
```cpp
String newToken = auth->refreshAccessToken(refreshToken);
if (!newToken.isEmpty()) {
    Serial.println("Token refreshed successfully");
}
```

---

## WiFiManager API

### Overview

`WiFiManager` handles WiFi connection, auto-reconnect, and connection monitoring.

### Public Methods

#### `connect()`

```cpp
bool connect(const String& ssidName, const String& pass)
```

**Description:** Connects to a WiFi network.

**Parameters:**
- `ssidName` - Network SSID
- `pass` - Network password

**Returns:**
- `true` - Connection initiated
- `false` - Failed to initiate

**Example:**
```cpp
WiFiManager* wifi = new WiFiManager();
if (wifi->connect("MyWiFi", "password123")) {
    Serial.println("Connecting to WiFi...");
}
```

---

#### `disconnect()`

```cpp
void disconnect()
```

**Description:** Disconnects from WiFi network.

**Returns:** None

**Example:**
```cpp
wifi->disconnect();
```

---

#### `startAPMode()`

```cpp
void startAPMode(const String& apName)
```

**Description:** Starts access point mode for initial setup.

**Parameters:**
- `apName` - AP SSID name

**Behavior:**
- Creates WiFi access point
- IP: 192.168.4.1
- Useful for captive portal

**Example:**
```cpp
wifi->startAPMode("SpotifyController-Setup");
```

---

#### `update()`

```cpp
void update()
```

**Description:** Updates WiFi manager state (call in main loop).

**Behavior:**
- Monitors connection status
- Handles auto-reconnect
- Manages timeouts

**Returns:** None

**Example:**
```cpp
void loop() {
    wifi->update();
    // ... other code
}
```

---

## DisplayManager API

### Overview

`DisplayManager` abstracts display hardware, supporting multiple display types.

### Public Methods

#### `getInstance()`

```cpp
static DisplayManager* getInstance()
```

**Description:** Gets the singleton instance of DisplayManager.

**Returns:** Pointer to DisplayManager instance

**Example:**
```cpp
DisplayManager* display = DisplayManager::getInstance();
```

---

#### `init()`

```cpp
bool init()
```

**Description:** Initializes the display based on configured type.

**Returns:**
- `true` - Display initialized
- `false` - Failed

**Example:**
```cpp
if (display->init()) {
    Serial.println("Display initialized");
}
```

---

## UI Components API

### NowPlayingScreen

#### Constructor

```cpp
NowPlayingScreen(lv_obj_t* parent)
```

**Description:** Creates the Now Playing screen.

**Parameters:**
- `parent` - LVGL parent object (usually screen)

**Returns:** NowPlayingScreen object

**Example:**
```cpp
lv_obj_t* screen = lv_obj_create(NULL);
NowPlayingScreen* nowPlaying = new NowPlayingScreen(screen);
```

---

#### `updateTrackInfo()`

```cpp
void updateTrackInfo(const SpotifyClient::TrackInfo& track)
```

**Description:** Updates the UI with new track information.

**Parameters:**
- `track` - Track information structure

**Returns:** None

**Example:**
```cpp
TrackInfo track = spotify->getCurrentTrack();
nowPlaying->updateTrackInfo(track);
```

---

#### `updateProgress()`

```cpp
void updateProgress(int progressMs, int durationMs)
```

**Description:** Updates the progress bar and time labels.

**Parameters:**
- `progressMs` - Current position (ms)
- `durationMs` - Track duration (ms)

**Returns:** None

**Example:**
```cpp
nowPlaying->updateProgress(90000, 180000);  // 1:30 / 3:00
```

---

#### `updatePlaybackState()`

```cpp
void updatePlaybackState(bool isPlaying)
```

**Description:** Updates the play/pause button icon.

**Parameters:**
- `isPlaying` - True if playing, false if paused

**Returns:** None

**Example:**
```cpp
nowPlaying->updatePlaybackState(true);  // Show pause icon
```

---

#### `updateVolume()`

```cpp
void updateVolume(int volumePercent)
```

**Description:** Updates the volume slider position.

**Parameters:**
- `volumePercent` - Volume level (0-100)

**Returns:** None

**Example:**
```cpp
nowPlaying->updateVolume(75);
```

---

## App API

### Overview

`App` is the main application controller that coordinates all subsystems.

### Public Methods

#### `getInstance()`

```cpp
static App& getInstance()
```

**Description:** Gets the singleton instance of App.

**Returns:** Reference to App instance

**Example:**
```cpp
App& app = App::getInstance();
```

---

#### `init()`

```cpp
bool init()
```

**Description:** Initializes all subsystems.

**Initialization Order:**
1. Config
2. Logger
3. WiFi
4. Display
5. Spotify
6. UI

**Returns:**
- `true` - All subsystems initialized
- `false` - Initialization failed

**Example:**
```cpp
if (!App::getInstance().init()) {
    Serial.println("Initialization failed");
    return;
}
```

---

#### `loop()`

```cpp
void loop()
```

**Description:** Main application loop (call in Arduino `loop()`).

**Behavior:**
- Updates WiFi manager
- Updates LVGL UI
- Polls Spotify for updates
- Executes scheduled tasks

**Returns:** None

**Example:**
```cpp
void loop() {
    App::getInstance().loop();
}
```

---

#### `getSpotifyClient()`

```cpp
SpotifyClient* getSpotifyClient()
```

**Description:** Gets the SpotifyClient instance.

**Returns:** Pointer to SpotifyClient

**Example:**
```cpp
SpotifyClient* spotify = App::getInstance().getSpotifyClient();
spotify->play();
```

---

## Error Codes

### HTTP Errors

| Code | Meaning | Action |
|-------|---------|--------|
| 200 | OK | Success |
| 204 | No Content | Success (no data to return) |
| 400 | Bad Request | Check API call parameters |
| 401 | Unauthorized | Refresh or re-authenticate |
| 403 | Forbidden | Check permissions/scopes |
| 404 | Not Found | Resource doesn't exist |
| 429 | Too Many Requests | Rate limited, wait and retry |
| 500 | Server Error | Spotify API error, retry later |

### WiFi Errors

| Code | Meaning | Action |
|-------|---------|--------|
| WL_DISCONNECTED | Not connected | Check WiFi credentials |
| WL_IDLE_STATUS | Idle | Starting up |
| WL_SCAN_COMPLETED | Scan done | Connecting |
| WL_NO_SSID_AVAIL | Network not found | Check SSID |
| WL_CONNECT_FAILED | Connection failed | Check password, router |
| WL_CONNECTION_LOST | Connection lost | Auto-reconnect will retry |
| WL_CONNECTED | Connected | Normal |

---

**Last Updated:** 2026-02-02
**Maintainer:** Spotify GUI Agent
