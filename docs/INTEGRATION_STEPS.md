# Phase 4 Integration Steps

This guide provides step-by-step instructions to integrate Phase 4 (Spotify API) into your project.

## Overview

Phase 4 adds complete Spotify API integration including:
- OAuth2 authentication with PKCE
- Playback control (play/pause/skip/seek/volume)
- Track information and library management
- Cover image download and caching
- Full search and playlist access

## Prerequisites

1. **Phase 1-3 completed**: WiFi, display, and touch handling must be working
2. **LittleFS**: Filesystem support for configuration and cache
3. **ArduinoJson v7.x**: Required for JSON parsing
4. **Spotify Developer Account**: Need a registered Spotify application

## Step 1: Create Spotify Application

1. Go to [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
2. Click "Create App"
3. Fill in:
   - App name: "ESP32 Spotify Controller"
   - Description: "WiFi-connected Spotify controller"
   - Redirect URIs: `http://localhost:8080/callback`
4. Copy **Client ID** and **Client Secret**
5. Edit `src/config/Config.hpp` or use captive portal to configure

## Step 2: Add New Source Files

Add the following files to your project:

```
src/
├── models.h              (NEW - Data structures)
├── models.cpp            (NEW - Model implementations)
├── cover_manager.h       (NEW - Image download/cache)
├── cover_manager.cpp     (NEW - Cover manager implementation)
├── playback_controller.h (NEW - Playback control wrapper)
├── playback_controller.cpp (NEW - Playback controller implementation)
├── spotify/
│   ├── SpotifyClient.hpp (ALREADY EXISTS)
│   ├── SpotifyClient.cpp (ALREADY EXISTS)
│   ├── AuthManager.hpp   (ALREADY EXISTS)
│   └── AuthManager.cpp   (ALREADY EXISTS)
└── ui/
    └── screens/
        └── NowPlaying.cpp (UPDATE - Cover manager integration)
```

## Step 3: Update platformio.ini

Ensure these dependencies are included:

```ini
lib_deps =
    ; Existing dependencies
    lvgl/lvgl@^8.3.11
    bblanchon/ArduinoJson@^6.21.5
    base64/Base64@^1.1.0
    
    ; Add if missing
    esphome/ESPAsyncWebServer-esphome@^3.1.0  ; For OAuth captive portal
```

## Step 4: Initialize CoverManager in App

Edit `src/app/App.cpp`:

```cpp
#include "../cover_manager.h"

class App {
private:
    CoverManager* coverManager;  // Add member

public:
    bool initSpotify() {
        // ... existing Spotify initialization ...
        
        // Initialize cover manager
        coverManager = new CoverManager();
        coverManager->init();
        
        return true;
    }
    
    // Add getter
    CoverManager* getCoverManager() {
        return coverManager;
    }
};
```

## Step 5: Initialize PlaybackController in App

Edit `src/app/App.cpp`:

```cpp
#include "../playback_controller.h"

class App {
private:
    PlaybackController* playbackController;  // Add member

public:
    bool initSpotify() {
        // ... existing Spotify initialization ...
        
        // Initialize playback controller
        playbackController = new PlaybackController(spotifyClient);
        playbackController->init();
        
        return true;
    }
    
    // Add getter
    PlaybackController* getPlaybackController() {
        return playbackController;
    }
};
```

## Step 6: Update NowPlaying Screen

The `NowPlaying.cpp` file has already been updated to use CoverManager. Ensure it includes:

```cpp
#include "../../cover_manager.h"  // Add include
```

## Step 7: Update Event Handlers

In `src/app/App.cpp`, register playback event handlers:

```cpp
void App::registerEventHandlers() {
    // Playback control events
    eventBus.subscribe(EventType::PLAYBACK_CHANGED,
        [this](const Event& e) { this->onPlaybackChanged(); });
    
    eventBus.subscribe(EventType::TRACK_CHANGED,
        [this](const Event& e) { this->onTrackChanged(); });
}
```

## Step 8: Update Touch Handlers

Edit your touch handler to call PlaybackController methods:

```cpp
#include "../playback_controller.h"

// In your touch handler:
void TouchHandler::handleButtonPress(int x, int y) {
    auto* app = App::getInstance();
    auto* playback = app->getPlaybackController();
    
    if (isInPlayPauseButton(x, y)) {
        playback->togglePlay();
    } else if (isInNextButton(x, y)) {
        playback->next();
    } else if (isInPreviousButton(x, y)) {
        playback->previous();
    } else if (isInSaveButton(x, y)) {
        auto track = playback->getCurrentTrack();
        playback->toggleSaveTrack(track.id);
    }
}

void TouchHandler::handleVolumeTouch(int x, int y) {
    auto* app = App::getInstance();
    auto* playback = app->getPlaybackController();
    
    int volume = calculateVolumeFromPosition(x, y);
    playback->setVolume(volume);
}

void TouchHandler::handleSeekTouch(int x, int y) {
    auto* app = App::getInstance();
    auto* playback = app->getPlaybackController();
    
    int percentage = calculatePercentageFromPosition(x, y);
    playback->seekPercentage(percentage);
}
```

## Step 9: Update Main Loop Polling

Edit `src/app/App.cpp`:

```cpp
void App::loop() {
    // ... existing code ...
    
    // Poll Spotify status periodically
    if (playbackController && state == AppState::NOW_PLAYING) {
        static unsigned long lastPoll = 0;
        if (millis() - lastPoll >= PLAYER_POLL_INTERVAL) { // 1000ms or 2000ms
            playbackController->updateNowPlaying();
            lastPoll = millis();
        }
    }
}
```

## Step 10: Add PlaybackController Header to Headers

Ensure `include/playback_controller.h` exists or add:

```cpp
// In src/playback_controller.cpp, add include:
#include "../spotify/SpotifyClient.hpp"
#include "models.h"
```

## Step 11: Test Authentication Flow

1. **Flash the device**: Upload the firmware
2. **Connect to WiFi**: Device should connect to your WiFi
3. **Open captive portal**: Go to `http://<device-ip>:8080`
4. **Click "Connect Spotify"**: This opens Spotify's authorization page
5. **Log in and authorize**: Grant the requested permissions
6. **Redirect back**: Device receives authorization code
7. **Tokens exchanged**: Access and refresh tokens are saved
8. **Now playing**: Device should show current Spotify track

## Step 12: Test Playback Controls

Verify each control works:

- [ ] **Play/Pause**: Toggle playback
- [ ] **Next/Previous**: Skip tracks
- [ ] **Seek**: Tap progress bar to seek
- [ ] **Volume**: Drag slider to change volume
- [ ] **Save Track**: Heart icon toggles save state

## Step 13: Test Cover Images

1. Play a track with album art
2. Check Serial Monitor for:
   ```
   🖼️  Loading album art: https://i.scdn.co/image/...
   📥 Cache miss: 4iV5W9uYEdYUVa79Axb7Rh, downloading...
   ✅ Cover downloaded: 45230 bytes
   ```
3. Re-play same track: Should see "Cache hit"
4. LittleFS should contain `/covers/` directory with images

## Step 14: Test Token Refresh

1. Wait 1 hour (or modify token expiry for testing)
2. Play a track
3. Check Serial Monitor for:
   ```
   🔄 Token expired, refreshing...
   ✅ Token refreshed
   ```

## Step 15: Test Error Handling

1. **No WiFi**: Disconnect WiFi → Should show error
2. **Expired Token**: Delete token file → Should request re-auth
3. **Rate Limit**: Make many requests → Should back off
4. **Invalid Device**: Try to control offline device → Should handle gracefully

## Step 16: Verify Integration

Complete this checklist:

- [ ] Authentication flow works end-to-end
- [ ] Tokens are saved and loaded correctly
- [ ] Now playing updates every 1-2 seconds
- [ ] Play/Pause/Next/Previous work
- [ ] Volume control works
- [ ] Seek works (progress bar)
- [ ] Save/Remove track works
- [ ] Cover images download and cache
- [ ] Token refresh works automatically
- [ ] Errors are handled gracefully
- [ ] UI updates correctly on state changes

## Step 17: Optimize Performance

1. **Reduce polling**: If needed, increase `PLAYER_POLL_INTERVAL` to 3000ms
2. **Cache covers**: Ensure cover cache is working (check `/covers/`)
3. **Lazy load**: Images only load when needed
4. **Memory check**: Monitor free memory in Serial Monitor

## Step 18: Add User Configuration

Users need to configure:

1. **WiFi SSID and Password**
2. **Spotify Client ID** (from Spotify Developer Dashboard)
3. **Spotify Client Secret** (from Spotify Developer Dashboard)

Configure via:
- Captive portal (recommended)
- Serial config
- LittleFS file (`/config.json`)

## Step 19: Deploy and Test on Hardware

1. **Upload to device**: Use `pio run -t upload`
2. **Monitor Serial**: `pio device monitor`
3. **Test all features**: Go through full integration checklist
4. **Check memory**: Ensure no memory leaks or crashes

## Step 20: Documentation

Update your project documentation:

1. **README.md**: Add Spotify integration section
2. **User Guide**: Explain authentication and controls
3. **Troubleshooting**: Add common issues and solutions

## Common Issues and Solutions

### Issue: "LittleFS not available"
**Solution**: Ensure `LITTLEFS_PRESENT` is defined or format filesystem with `LittleFS.begin(true)`

### Issue: "Token refresh failed"
**Solution**: Check client credentials, verify refresh token is valid

### Issue: "Cover download failed"
**Solution**: Check WiFi connection, verify URL is valid, ensure LittleFS has space

### Issue: "Rate limit exceeded"
**Solution**: Increase polling interval, reduce API calls

### Issue: "No tracks found"
**Solution**: Ensure Spotify is playing on some device, check active device

### Issue: "401 Unauthorized"
**Solution**: Token expired, refresh or re-authenticate

## Performance Benchmarks

Expected performance on ESP32-WROVER with PSRAM:

- Authentication: 2-3 seconds
- Now playing update: 50-200ms
- Cover download: 1-5 seconds (varies by size)
- UI refresh: 10-50ms
- Free memory: ~150KB (with PSRAM)

## Next Steps

After Phase 4 is complete:

1. **Add more features**: Search, playlists, shuffle/repeat
2. **Improve UI**: Animations, gestures, more screens
3. **Optimize**: Reduce memory usage, faster updates
4. **Add settings**: Volume limit, display brightness, themes
5. **Power management**: Deep sleep, battery monitoring

## Support

For issues or questions:
1. Check `docs/SPOTIFY_API_FLOW.md` for API details
2. Review Serial Monitor output for errors
3. Test each component individually
4. Check Spotify Developer Dashboard for rate limits

## Success Criteria

Phase 4 is complete when:

✅ Spotify authentication works automatically
✅ Playback controls are responsive (<100ms latency)
✅ Now playing updates accurately
✅ Cover images load and cache correctly
✅ Token refresh works transparently
✅ Errors are handled without crashes
✅ Memory usage is stable (no leaks)
✅ UI updates smoothly

Good luck with your integration! 🎵
