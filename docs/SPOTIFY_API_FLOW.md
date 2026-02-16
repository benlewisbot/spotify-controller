# Spotify API Integration Flow

This document describes the complete Spotify API integration flow for the ESP32 Spotify Controller.

## Overview

The Spotify Controller uses the **Spotify Web API** with **OAuth 2.0 PKCE** authentication. This allows secure, token-based API access without storing client secrets on the device (except during initial setup).

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        ESP32 Device                             │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────┐  ┌──────────────────┐  ┌────────────────┐ │
│  │   AuthManager    │  │ SpotifyClient   │  │  CoverManager  │ │
│  │  (OAuth2 PKCE)   │  │  (API Client)   │  │   (Images)     │ │
│  └────────┬─────────┘  └────────┬─────────┘  └────────┬───────┘ │
│           │                     │                     │          │
│           └─────────┬───────────┴─────────────────────┘          │
│                     │                                             │
│              ┌──────▼──────┐                                      │
│              │PlaybackCtrl │                                      │
│              │ (Wrapper)   │                                      │
│              └──────┬──────┘                                      │
│                     │                                             │
│              ┌──────▼──────┐                                      │
│              │     UI      │ (Now Playing, etc.)                  │
│              │   (LVGL)    │                                      │
│              └─────────────┘                                      │
└─────────────────────────────────────────────────────────────────┘
                           │ HTTPS
                           │
┌──────────────────────────▼──────────────────────────────────────┐
│                   Spotify API Server                             │
│  - Authorization Server (accounts.spotify.com)                     │
│  - Web API (api.spotify.com)                                     │
└──────────────────────────────────────────────────────────────────┘
```

## Authentication Flow (OAuth 2.0 PKCE)

### Step 1: Authorization Code Request

```
User ──► ESP32 ──► Spotify Auth Server
              │
              ├─ Generate code_verifier (random string)
              ├─ Generate code_challenge = SHA256(code_verifier)
              ├─ Generate state (CSRF protection)
              │
              └─ GET https://accounts.spotify.com/authorize?
                 client_id=CLIENT_ID
                 &response_type=code
                 &redirect_uri=http://192.168.4.1:8080/callback
                 &scope=SCOPES
                 &code_challenge=CODE_CHALLENGE
                 &code_challenge_method=S256
                 &state=STATE
```

**Parameters:**
- `client_id`: Your Spotify application's client ID
- `response_type`: Always `code` for authorization code flow
- `redirect_uri`: Must match your Spotify app settings
- `scope`: Space-separated list of permissions (see below)
- `code_challenge`: Base64 URL-encoded SHA256 hash of `code_verifier`
- `code_challenge_method`: Always `S256` (SHA256)
- `state`: Random string for CSRF protection

### Step 2: User Authorization

User opens the authorization URL in browser and logs into Spotify.

User grants permissions → Redirected to `redirect_uri` with authorization code:

```
http://192.168.4.1:8080/callback?code=AUTH_CODE&state=STATE
```

### Step 3: Exchange Code for Tokens

ESP32 receives callback and exchanges authorization code for tokens:

```
ESP32 ──► Spotify Token Server
       POST https://accounts.spotify.com/api/token
       Content-Type: application/x-www-form-urlencoded
       
       grant_type=authorization_code
       &code=AUTH_CODE
       &redirect_uri=http://192.168.4.1:8080/callback
       &client_id=CLIENT_ID
       &code_verifier=CODE_VERIFIER
```

**Response:**

```json
{
  "access_token": "NgCXRKcM...",
  "token_type": "Bearer",
  "expires_in": 3600,
  "refresh_token": "NgAagA...",
  "scope": "user-read-playback-state user-modify-playback-state ..."
}
```

### Step 4: Use Access Token

Include `access_token` in all API requests:

```
GET https://api.spotify.com/v1/me/player/currently-playing
Authorization: Bearer NgCXRKcM...
```

### Step 5: Refresh Access Token

Access tokens expire after 1 hour. Use `refresh_token` to get a new one:

```
POST https://accounts.spotify.com/api/token
Content-Type: application/x-www-form-urlencoded

grant_type=refresh_token
&refresh_token=NgAagA...
&client_id=CLIENT_ID
```

**Response:**

```json
{
  "access_token": "NgAagA...",
  "token_type": "Bearer",
  "expires_in": 3600
}
```

## Required Scopes

The following OAuth scopes are required for the Spotify Controller:

```cpp
"user-read-playback-state"      // Get current playback state
"user-modify-playback-state"    // Control playback (play/pause/skip)
"user-read-currently-playing"    // Get currently playing track
"user-read-playback-position"    // Get track position for seek
"user-library-read"              // Check if track is saved
"user-library-modify"            // Save/remove tracks
"playlist-read-private"          // Get user's playlists
"playlist-read-collaborative"    // Read collaborative playlists
```

## API Endpoints Used

### Currently Playing

```http
GET /v1/me/player/currently-playing
Authorization: Bearer {access_token}

Response:
{
  "item": {
    "id": "4iV5W9uYEdYUVa79Axb7Rh",
    "name": "You Know You're Right",
    "duration_ms": 240000,
    "album": {
      "name": "Nirvana",
      "images": [
        {"url": "https://i.scdn.co/image/...", "width": 640, "height": 640}
      ]
    },
    "artists": [
      {"name": "Nirvana"}
    ]
  },
  "is_playing": true,
  "progress_ms": 120000,
  "device": {
    "id": "device_id",
    "name": "Living Room",
    "volume_percent": 75
  }
}
```

### Play/Pause

```http
PUT /v1/me/player/play
Authorization: Bearer {access_token}

PUT /v1/me/player/pause
Authorization: Bearer {access_token}
```

### Skip Tracks

```http
POST /v1/me/player/next
Authorization: Bearer {access_token}

POST /v1/me/player/previous
Authorization: Bearer {access_token}
```

### Seek

```http
PUT /v1/me/player/seek?position_ms={ms}
Authorization: Bearer {access_token}
```

### Volume

```http
PUT /v1/me/player/volume?volume_percent={0-100}
Authorization: Bearer {access_token}
```

### Save/Remove Track

```http
PUT /v1/me/tracks?ids={track_id}
Authorization: Bearer {access_token}

DELETE /v1/me/tracks?ids={track_id}
Authorization: Bearer {access_token}
```

### Check if Track Saved

```http
GET /v1/me/tracks/contains?ids={track_id}
Authorization: Bearer {access_token}

Response: [true]  // Array of booleans
```

## Cover Image Handling

### Image URLs

Spotify provides multiple image sizes for album art:

```json
{
  "images": [
    {"url": "https://i.scdn.co/image/small.jpg", "width": 64, "height": 64},
    {"url": "https://i.scdn.co/image/medium.jpg", "width": 300, "height": 300},
    {"url": "https://i.scdn.co/image/large.jpg", "width": 640, "height": 640}
  ]
}
```

### Cache Strategy

The `CoverManager` implements the following caching strategy:

1. **Cache Key**: Track ID (e.g., `/covers/4iV5W9uYEdYUVa79Axb7Rh.jpg`)
2. **Cache Index**: JSON file tracking all cached covers
3. **Size Limit**: 5MB total cache size
4. **File Limit**: 50 files maximum
5. **TTL**: 7 days before expiration
6. **LRU**: Least recently used eviction when cache is full

### Download Flow

```
Request Cover (trackId)
    │
    ├─ Check Cache Index
    │   └─ Hit? → Return cached file path
    │
    ├─ Download from Spotify URL
    │   ├─ Check available space
    │   ├─ Download to temp file
    │   └─ Verify file integrity
    │
    ├─ Update Cache Index
    │   └─ Save metadata (timestamp, size, access count)
    │
    └─ Return file path
```

## Error Handling

### HTTP Error Codes

| Code | Meaning | Handling |
|------|---------|----------|
| 200  | Success | Process response |
| 204  | No Content | Nothing playing (for /currently-playing) |
| 400  | Bad Request | Invalid parameters |
| 401  | Unauthorized | Token expired → Refresh token |
| 403  | Forbidden | Insufficient permissions |
| 404  | Not Found | Resource not found |
| 429  | Rate Limited | Back off and retry |
| 500+ | Server Error | Retry with exponential backoff |

### Token Refresh Flow

```cpp
if (httpCode == 401) {
    // Access token expired
    String newToken = authManager->refreshAccessToken(refreshToken);
    if (!newToken.isEmpty()) {
        accessToken = newToken;
        // Retry request
        return retryRequest();
    }
}
```

## Rate Limiting

Spotify API has rate limits. The implementation enforces:

- **Minimum request interval**: 100ms between requests
- **Backoff on 429**: Exponential backoff (1s, 2s, 4s, 8s, ...)

## Security Considerations

1. **HTTPS Only**: All API requests use HTTPS
2. **PKCE**: Authorization flow uses Proof Key for Code Exchange
3. **State Parameter**: CSRF protection
4. **Token Storage**: Tokens stored in LittleFS (consider encryption)
5. **No Secrets in Code**: Client credentials not hardcoded

## Performance Optimization

1. **Polling Interval**: Update now playing every 2 seconds
2. **Conditional Updates**: Only update UI when track changes
3. **Image Caching**: Avoid re-downloading album art
4. **Lazy Loading**: Download images only when needed
5. **Memory Efficiency**: Stream downloads in chunks (1KB buffers)

## Configuration

### WiFi

```json
{
  "wifi": {
    "ssid": "YourWiFi",
    "password": "YourPassword"
  }
}
```

### Spotify

```json
{
  "spotify": {
    "clientId": "your-client-id",
    "clientSecret": "your-client-secret",
    "accessToken": "NgCXRKcM...",
    "refreshToken": "NgAagA..."
  }
}
```

## Troubleshooting

### Token Refresh Fails

1. Check if refresh_token is valid
2. Verify client credentials
3. Check WiFi connection
4. Re-authenticate if refresh_token expired

### Cover Images Not Loading

1. Check LittleFS is mounted
2. Verify cache directory exists
3. Check available storage
4. Clear cache if corrupted

### Playback Control Not Working

1. Check if device is active
2. Verify access token is valid
3. Check scopes include `user-modify-playback-state`
4. Ensure device is selected

## API Reference

See [Spotify Web API Reference](https://developer.spotify.com/documentation/web-api) for complete documentation.

## License

This implementation follows Spotify's Developer Terms of Service.
