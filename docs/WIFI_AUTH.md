# WiFi & Authentication - Implementation Guide

## Overview
This document describes the WiFi and Spotify OAuth2 authentication implementation for the Spotify Controller ESP32.

## WiFi Connection

### Architecture
- **Class:** `WiFiManager` (`src/network/WiFiManager.{h,cpp}`)
- **States:** `DISCONNECTED`, `CONNECTING`, `CONNECTED`, `AP_MODE`, `ERROR`
- **Features:**
  - Auto-reconnect with configurable retry attempts
  - Connection timeout handling
  - AP mode for initial setup
  - Event-based state management

### Configuration
WiFi credentials are stored in `/config.json`:

```json
{
  "wifi": {
    "ssid": "YourWiFiSSID",
    "password": "YourPassword"
  }
}
```

### Usage
```cpp
#include "network/WiFiManager.hpp"

WiFiManager* wifi = new WiFiManager();

// Connect to WiFi
wifi->connect(ssid, password);

// Check connection
if (wifi->isConnected()) {
    Serial.println("WiFi connected!");
    Serial.printf("IP: %s\n", wifi->getIP().c_str());
}

// Enable/disable auto-reconnect
wifi->setAutoReconnect(true);

// Call update() periodically
void loop() {
    wifi->update();
}
```

### Event Handling
WiFiManager emits events through the EventBus:
- `WIFI_CONNECTED`: Successfully connected
- `WIFI_DISCONNECTED`: Connection lost

### AP Mode (Captive Portal)
For initial setup when no WiFi credentials are configured:

```cpp
wifi->startAPMode("Spotify-Controller");
// IP: 192.168.4.1
```

## Spotify OAuth2 Authentication

### Architecture
- **Class:** `AuthManager` (`src/spotify/AuthManager.{h,cpp}`)
- **States:** `NONE`, `WAITING_FOR_AUTH`, `AUTHENTICATED`, `ERROR`
- **Flow:** Authorization Code with PKCE (Proof Key for Code Exchange)

### OAuth2 Flow

```
1. Device generates code_verifier and code_challenge
2. Device starts web server on port 8080
3. User navigates to Spotify authorization URL
4. User logs in and grants permissions
5. Spotify redirects to http://DEVICE_IP:8080/callback
6. Device exchanges authorization code for access+refresh tokens
7. Tokens are stored in config.json
```

### Scopes
The following Spotify OAuth scopes are requested:
- `user-read-playback-state` - Get playback state
- `user-modify-playback-state` - Control playback
- `user-read-currently-playing` - Get current track
- `user-library-read` - Read saved tracks
- `playlist-read-private` - Read private playlists
- `playlist-read-collaborative` - Read collaborative playlists

### Configuration
Spotify credentials are stored in `/config.json`:

```json
{
  "spotify": {
    "client_id": "your_client_id",
    "client_secret": "your_client_secret",
    "access_token": "encrypted_token",
    "refresh_token": "encrypted_token"
  }
}
```

### Getting Spotify Credentials

1. Go to https://developer.spotify.com/dashboard
2. Click "Create App"
3. Fill in app details:
   - App name: "Spotify Controller ESP32"
   - App description: "Touch controller for Spotify"
   - Redirect URI: `http://YOUR_DEVICE_IP:8080/callback`
4. Save the Client ID and Client Secret

### Usage
```cpp
#include "spotify/AuthManager.hpp"

AuthManager* auth = new AuthManager();

// Initialize with credentials
auth->init(clientId, clientSecret);

// Start auth server (captive portal)
auth->startAuthServer();

// Call update() periodically to handle web requests
void loop() {
    auth->update();
}

// Get tokens after authentication
String accessToken = auth->getAccessToken();
String refreshToken = auth->getRefreshToken();
```

### Token Refresh

When the access token expires (typically 1 hour), refresh it:

```cpp
String newAccessToken = auth->refreshAccessToken(refreshToken);
```

### PKCE Implementation

The PKCE (Proof Key for Code Exchange) flow adds security:

1. **Code Verifier:** Random 43-128 character string
2. **Code Challenge:** SHA-256 hash of verifier, Base64 URL encoded

```cpp
// Generate code verifier
String verifier = auth->generateCodeVerifier();

// Generate code challenge
String challenge = auth->generateCodeChallenge(verifier);

// Include challenge in auth URL
String url = "https://accounts.spotify.com/authorize";
url += "?code_challenge=" + challenge;
url += "&code_challenge_method=S256";
```

## Configuration Manager

### Architecture
- **Class:** `ConfigManager` (`src/config/Config.{h,cpp}`)
- **Storage:** LittleFS partition on ESP32 flash
- **Format:** JSON (`/config.json`)

### Configuration Structure

```json
{
  "wifi": {
    "ssid": "NetworkName",
    "password": "WiFiPassword"
  },
  "spotify": {
    "client_id": "ClientID",
    "client_secret": "ClientSecret",
    "access_token": "...",
    "refresh_token": "..."
  },
  "display": {
    "orientation": 1,
    "brightness": 75,
    "screensaver": {
      "enabled": true,
      "timeout_minutes": 10
    }
  },
  "volume": {
    "limit": 80
  },
  "device": {
    "device_id": "ESP1234567890"
  }
}
```

### Usage
```cpp
#include "config/Config.hpp"

ConfigManager& config = ConfigManager::getInstance();

// Initialize (mounts LittleFS, loads config)
config.init();

// WiFi configuration
config.setWiFiSSID("MyWiFi");
config.setWiFiPassword("MyPassword");
config.save();  // Persist to flash

// Spotify configuration
config.setSpotifyClientId("client_id");
config.setSpotifyClientSecret("client_secret");

// Save tokens after authentication
config.saveTokens(accessToken, refreshToken);

// Get configuration
String ssid = config.getWiFiSSID();
String clientId = config.getSpotifyClientId();
```

## LittleFS File System

### Partition Layout
The ESP32 uses a custom partition table with LittleFS partition:

```
nvs (WiFi data)
ota_0 (OTA application)
ota_1 (OTA application)
app0 (Application)
spiffs (LittleFS - ~1MB)
```

### File System Operations

```cpp
// Initialize LittleFS
LittleFS.begin(true);  // true = format if fails

// List files
File root = LittleFS.open("/");
File file = root.openNextFile();
while (file) {
    Serial.printf("%s (%d bytes)\n", file.name(), file.size());
    file = root.openNextFile();
}

// Read file
File f = LittleFS.open("/config.json", "r");
if (f) {
    String content = f.readString();
    f.close();
}

// Write file
File f = LittleFS.open("/config.json", "w");
if (f) {
    f.print(jsonString);
    f.close();
}

// Format file system
LittleFS.format();
```

### Uploading Files via PlatformIO

```bash
# Upload data directory to LittleFS
pio run --target uploadfs --environment esp32-wrover
```

### File System Size
- **Total:** ~1MB
- **Config:** ~1KB
- **Cover Cache:** Up to 10MB (with external SPIFFS)

## Security Considerations

### WiFi Credentials
- Stored in plain text in `/config.json`
- Consider encryption for production deployments

### Spotify Tokens
- Access tokens are temporary (~1 hour)
- Refresh tokens are long-lived
- Tokens are stored in plain text (consider encryption)

### HTTPS
- Spotify API uses HTTPS with `WiFiClientSecure`
- Certificates are validated (currently insecure for simplicity)
- **TODO:** Add proper certificate validation

### PKCE Benefits
- Prevents authorization code interception
- No client secret needed for mobile/IoT apps
- Recommended for public clients

## Troubleshooting

### WiFi Connection Fails
```
❌ Failed to mount LittleFS
```
- Check partition table in `platformio.ini`
- Try formatting: `LittleFS.format()`

### Authentication Timeout
```
⏰ Auth timeout
```
- Check callback URL in Spotify Developer Dashboard
- Ensure device is reachable on local network
- Verify redirect URI matches: `http://IP:8080/callback`

### Token Refresh Fails
```
⚠️ Token refresh failed: 401
```
- Refresh token may be expired
- User must re-authenticate
- Clear tokens and restart auth flow

### LittleFS Full
```
❌ Failed to write config
```
- Check used space: `LittleFS.usedBytes()`
- Delete cached cover images
- Consider larger partition table

## Next Steps

1. **AP Mode Setup:** Implement captive portal for initial WiFi configuration
2. **Token Auto-Refresh:** Automatically refresh access token before expiry
3. **Certificate Validation:** Add proper HTTPS certificate validation
4. **Error Recovery:** Graceful handling of network failures
5. **Configuration UI:** Add UI for changing WiFi and Spotify settings
