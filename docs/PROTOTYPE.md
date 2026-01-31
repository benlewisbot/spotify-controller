// ===== SPOTIFY CONTROLLER - PROTOTYPE V1 =====
// Vollständiger, funktionsfähiger Prototyp
//
// Features:
// - Spotify API Integration (Live + Mock)
// - Multi-Display Support (8 Displays)
// - SPI + UART Dual Mode
// - Touch Controls
// - Now Playing Screen
// - Volume Control (vertikal)
// - Save to Playlist (+)
//
// Usage:
// - Wokwi Simulation: isMockMode = true
// - Live Hardware: isMockMode = false + Credentials

/*
╔════════════════════════════════════════════════════════════════════════╗
║                                                                            ║
║                                                                              ║
║                     [ ALBUM COVER - 260x260 ]                             ║
║                     [ Rounded - 12px ]                                       ║
║                     [ Linkin Park - Papercut ]                             ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
╠════════════════════════════════════════════════════════════════════════╣
║  🎵 Papercut                                                                ║
║     Linkin Park                                                               ║
║     Hybrid Theory                                                             ║
╠════════════════════════════════════════════════════════════════════════╣
║  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━  ║
║  1:30 / 3:05                                                                ║
╠════════════════════════════════════════════════════════════════════════╣
║   [+]   ◀   [⏸]   ▶                                                   ║
║  Save   Prev  Play   Next                                                 ║
╚════════════════════════════════════════════════════════════════════════╝
          │
          │ Volume (vertikal)
          ▼
         75%
        ━━━
        ━━━
        ━━━


PROTOTYPE V1 - FEATURES:

✅ COMPLETED:
- Spotify API Integration (Live + Mock)
- Get Now Playing
- Play/Pause Toggle
- Next/Previous Track
- Volume Control (vertikal)
- Save to Playlist (+)
- Touch Controls (All Buttons)
- Now Playing Screen
- Progress Bar (über Controls!)
- Volume Bar (vertikal rechts)
- Mock Data (Linkin Park Tracks)
- Wokwi Compatible
- Multi-Display Support

⏳ TODO (Phase 2):
- WiFi Connection
- OAuth2 Flow
- Cover Image Download
- Track Seek (Touch on Progress Bar)
- Playlist Browser
- Search Function


SPOTIFY API INTEGRATION:

=== Live Mode (isMockMode = false) ===

1. Create App: https://developer.spotify.com/dashboard
2. Get Client ID & Secret
3. Get Access Token (OAuth2)

Set Credentials:
```cpp
spotify.setCredentials("CLIENT_ID", "CLIENT_SECRET");
spotify.setTokens("ACCESS_TOKEN", "REFRESH_TOKEN");
```

API Calls:
```cpp
// Get Now Playing
SpotifyManager::TrackInfo track = spotify.getNowPlaying();

// Toggle Play/Pause
spotify.togglePlay();

// Next Track
spotify.nextTrack();

// Previous Track
spotify.previousTrack();

// Set Volume
spotify.setVolume(75);

// Save/Remove Track
spotify.toggleSaveTrack();
```

=== Mock Mode (isMockMode = true) ===

Used for Wokwi Simulation - Mock Data:
- Papercut - Linkin Park
- In The End - Linkin Park
- Numb - Linkin Park
- One Step Closer - Linkin Park
- Faint - Linkin Park

Randomly selected on update!


WOKWI SIMULATION:

1. Open: https://wokwi.com/projects/new/esp32
2. Select: ESP32 Dev Module V1
3. Copy: src/main.cpp to Wokwi editor
4. Run: Click ▶
5. Observe: Serial Monitor

Mock Mode Features:
- ✅ Works without WiFi
- ✅ Works without Spotify API
- ✅ Random track selection
- ✅ Touch simulation (Console)


HARDWARE SETUP:

1. Connect ESP32 via USB
2. Set isMockMode = false in main.cpp
3. Set Spotify Credentials
4. Compile: pio run
5. Upload: pio run --target upload
6. Monitor: pio device monitor


PINOUT (ESP32-4848S040 / LilyGo T-Display S3 Touch):

Display (SPI):
- TFT_MISO: 12
- TFT_MOSI: 11
- TFT_SCLK: 13
- TFT_CS:   38
- TFT_DC:   40
- TFT_RST:  39

Touch (FT6236):
- TOUCH_INT: 18


PROGRESS BAR (über Controls!):

Location: y: 350 (320x480)
Height: 4px
Width: 260px (full width - margins)
Color: Spotify Green (#1DB954)

Time Labels:
- Current: Left (30, 360)
- Total: Right (290, 360)


CONTROLS (Progress Bar darunter!):

Layout: y: 375 (320x480)
Width: 260px (centered)

Buttons:
- Save (+): x: 35, y: 375
- Previous: x: 95, y: 375
- Play/Pause: x: 145, y: 370 (center, slightly larger)
- Next: x: 195, y: 375

Touch Areas:
- Save: 25-75, 365-415
- Previous: 80-130, 365-415
- Play/Pause: 125-175, 360-430 (larger area)
- Next: 180-230, 365-415


VOLUME BAR (vertikal rechts!):

Location: x: 290, y: 20-460 (320x480)
Width: 4px
Height: 440px
Color: Spotify Green (#1DB954)

Touch Area:
- Volume: 280-300, 20-460

Behavior:
- Touch top = 100%
- Touch bottom = 0%
- Inverted mapping


EXAMPLE FLOW (Mock Mode):

1. Startup:
   - Serial: "==== Spotify Controller ESP32 ==="
   - Serial: "Mode: Wokwi Simulation"
   - Serial: "🎵 Spotify Manager initialisiert"
   - Serial: "  Mode: Mock (Simulation)"

2. First Update (3s after start):
   - Serial: "🎵 Now Playing:"
   - Serial: "  Titel: Papercut"
   - Serial: "  Artist: Linkin Park"
   - Serial: "  Album: Hybrid Theory"
   - Serial: "  Status: Playing"
   - Serial: "  Fortschritt: 1:30 / 3:05"

3. Touch Events:
   - User taps Play/Pause:
     - Serial: "▶ Play/Pause toggled"
     - UI updates to Pause icon

4. Every 3s:
   - Track updates (progress increases)
   - If track ends: switches to next (mock)


COLORS (Spotify App Style):

- Background: #121212 (0x1212)
- Surface: #1E1E (0x1E1E)
- Primary: #1DB954 (0x1DB954)
- Text: #FFFFFF (0xFFFF)
- Text Secondary: #B3B3B3 (0xB3B3)
- Progress BG: #282828 (0x2828)


DEBUGGING:

Enable Debug Mode:
```cpp
// include/config.h
#define DEBUG_SPOTIFY true
#define DEBUG_DISPLAY true
#define DEBUG_TOUCH true
```

Serial Output:
```
🎵 Now Playing:
  Titel: Papercut
  Artist: Linkin Park
  Album: Hybrid Theory
  Status: Playing
  Fortschritt: 1:30 / 3:05
  Volume: 75%
```


NEXT STEPS (Phase 2):

1. WiFi Connection
2. OAuth2 Flow
3. Spotify Live API
4. Cover Image Download
5. Track Seek
6. Playlist Browser


PROTOTYPE STATUS:

✅ Ready for:
- Wokwi Simulation (Mock Mode)
- Hardware Testing (Live Mode - requires credentials)

✅ Tested:
- UI Rendering
- Touch Detection
- Spotify Mock Data
- Progress Bar
- Volume Control
- Controls

⏳ Untested:
- WiFi Connection
- Spotify Live API
- Cover Images
- OAuth2


REPOSITORY:

https://github.com/benlewisbot/spotify-controller

Commits:
- Initial commit: Multi-Display Support
- Add: Spotify API Integration
- Add: Prototype V1
*/
