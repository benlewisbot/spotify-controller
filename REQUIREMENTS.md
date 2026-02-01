# Spotify Controller ESP32 - Requirements & Specification

> **Goal:** Build a modern, modular Spotify Controller for desktop use with ESP32
> **Target:** Coding Agent (Claude Code) - Use this as complete input to generate code

---

## Project Overview

A sleek Spotify remote controller for the desk using ESP32 with touch display. The device connects to Spotify API to control playback, browse playlists, and display album artwork.

### Hardware Constraints
- **Platform:** ESP32-WROVER (DIY, ~25-35€)
- **Power:** USB-powered (desktop, no battery)
- **Audio:** No local speakers - remote control only
- **Multi-device:** Designed for multiple controllers on the same network

---

## Technical Stack

| Component | Technology |
|-----------|------------|
| Firmware | C++ / PlatformIO (ESP-IDF framework) |
| Display Driver | LVGL (Light and Versatile Graphics Library) |
| WiFi | ESP32 built-in WiFi |
| Spotify API | OAuth 2.0, Web API |
| Networking | HTTPS client (via WiFi) |
| Storage | SPIFFS/LittleFS for config |
| Build System | PlatformIO |

---

## Hardware Requirements

### Core Components
```
ESP32-WROVER Board
- Dual-core 240MHz
- 4MB+ PSRAM (for display buffer)
- WiFi/Bluetooth
- USB-C power input

Touch Display (Flexible Interface)
- Resolution: Support 240x320, 320x240, 480x320, etc.
- Touch: Capacitive or Resistive (auto-detect)
- Interface: SPI (preferred) or I2C
- Orientation: Portrait + Landscape support
```

### Display Compatibility Layer
**CRITICAL:** Code must be modular for easy display swapping.

```cpp
// Abstract display interface
class DisplayInterface {
public:
    virtual bool init() = 0;
    virtual void setOrientation(bool portrait) = 0;
    virtual void touchRead(int16_t &x, int16_t &y) = 0;
    virtual int16_t getWidth() = 0;
    virtual int16_t getHeight() = 0;
};

// Implementations for:
// - ILI9341 (SPI, 240x320 or 320x240)
// - ST7789 (SPI, 240x240 or 135x240)
// - ILI9488 (SPI, 480x320)
// - Generic SPI displays
```

---

## Software Architecture

```
src/
├── main.cpp                 # Entry point
├── app/
│   ├── App.hpp/cpp         # Main application controller
│   ├── State.hpp/cpp       # App state management
│   └── EventBus.hpp/cpp    # Event handling
├── display/
│   ├── Display.hpp/cpp     # Display interface
│   ├── DisplayManager.hpp/cpp  # Display lifecycle
│   └── themes/
│       └── SpotifyTheme.hpp/cpp  # UI theme
├── ui/
│   ├── screens/
│   │   ├── NowPlaying.hpp/cpp
│   │   ├── PlaylistBrowser.hpp/cpp
│   │   └── Settings.hpp/cpp
│   ├── components/
│   │   ├── AlbumCover.hpp/cpp
│   │   ├── ProgressBar.hpp/cpp
│   │   ├── ControlBar.hpp/cpp
│   │   └── TouchButton.hpp/cpp
│   └── WindowManager.hpp/cpp
├── spotify/
│   ├── SpotifyClient.hpp/cpp    # Spotify API client
│   ├── AuthManager.hpp/cpp      # OAuth 2.0
│   ├── PlaybackController.hpp/cpp
│   └── ImageLoader.hpp/cpp       # Cover art download
├── network/
│   ├── WiFiManager.hpp/cpp      # WiFi connection
│   └── HTTPClient.hpp/cpp       # HTTPS requests
├── config/
│   └── Config.hpp/cpp           # Persistent storage
└── utils/
    ├── Logger.hpp/cpp
    └── Timer.hpp/cpp
```

---

## UI/UX Requirements

### Design Language
- **Primary inspiration:** Current Spotify App (dark, sleek, album-focused)
- **Secondary influence:** Apple Liquid Glass (translucency, blur effects, smooth animations)
- **Theme:** Dark mode default (matching Spotify)

### Key Design Elements

#### Color Palette
```cpp
// Spotify-inspired colors
#define COLOR_BG            LV_COLOR_MAKE(18, 18, 18)      // Spotify dark bg
#define COLOR_SURFACE       LV_COLOR_MAKE(40, 40, 40)      // Elevated surface
#define COLOR_SPOTIFY_GREEN LV_COLOR_MAKE(30, 215, 96)     // Brand green
#define COLOR_TEXT_PRIMARY  LV_COLOR_MAKE(255, 255, 255)
#define COLOR_TEXT_SECONDARY LV_COLOR_MAKE(179, 179, 179)
#define COLOR_ACCENT        LV_COLOR_MAKE(29, 185, 84)
```

#### Typography
- **Header:** Bold, large (24-28px)
- **Title:** Medium, (18-22px)
- **Body:** Regular, (14-16px)
- **Caption:** Light, (12px)

#### Liquid Glass Effects
- **Blur:** Background blur behind modals and overlays
- **Translucency:** Semi-transparent overlays (alpha ~0.7-0.9)
- **Borders:** Subtle, thin borders with low opacity
- **Shadows:** Soft, diffuse shadows

### Screen Layouts

#### 1. Now Playing Screen (Main)
```
┌────────────────────────────┐
│                            │
│     [ALBUM ARTWORK]        │
│       (cover image)        │
│                            │
│      Song Title            │
│      Artist Name           │
│                            │
│   ━━━━━━━━━━━━━━━━━━━     │  Progress bar
│   1:23              3:45   │  Time labels
│                            │
│   ◀◀  [▶/⏸]  ▶▶  🔊      │  Controls
│                            │
│            ⋮              │  Menu
└────────────────────────────┘
```

**Features:**
- Large album artwork with smooth transitions
- Song title + artist name (ellipsis if too long)
- Progress bar with scrubbing
- Playback controls (previous, play/pause, next, volume)
- Volume slider (tapped from volume icon)
- Menu access (three dots)

#### 2. Playlist Browser Screen
```
┌────────────────────────────┐
│  ← Playlists            ⚙️  │
├────────────────────────────┤
│  ┌──────────────────────┐   │
│  │ [Cover] Title        │   │  Playlist items
│  │         Description  │   │  (scrollable)
│  └──────────────────────┘   │
│  ┌──────────────────────┐   │
│  │ [Cover] Title        │   │
│  │         Description  │   │
│  └──────────────────────┘   │
│  ...                        │
└────────────────────────────┘
```

**Features:**
- Scrollable playlist list
- Playlist thumbnail + title
- Tap to load and play
- Search bar (future feature)
- Pull to refresh

#### 3. Settings Screen
```
┌────────────────────────────┐
│  ← Settings                │
├────────────────────────────┤
│  Display Orientation        │
│  ○ Portrait ● Landscape     │
│                            │
│  Screensaver                │
│  [ON/OFF]  5 minutes       │
│                            │
│  Brightness                 │
│  ━━━━━━━━ 75%              │
│                            │
│  WiFi Status                │
│  Connected: HomeNet        │
│                            │
│  Spotify Account            │
│  ben@email.com             │
│                            │
│  [Reconnect Spotify]        │
│                            │
│  [Display Info]             │
│  [Version: 1.0.0]           │
└────────────────────────────┘
```

### Interaction Design

#### Touch Gestures
- **Tap:** Select button, toggle play/pause, open menu
- **Swipe:** Navigate between screens, scroll lists
- **Hold:** Show additional options (context menu)
- **Long press:** Seek in progress bar

#### Animation Guidelines
- **Duration:** 200-300ms for most transitions
- **Easing:** Ease-out for natural feel
- **Transitions:** Fade + slide for screen changes
- **Button press:** Scale down (0.95x) feedback

---

## Functional Requirements

### Priority 1 (MVP - Core Features)

#### 1.1 WiFi Connection
- Auto-connect to configured network on boot
- Fallback to AP mode if no WiFi (for initial setup)
- Persistent credentials stored in flash
- Auto-reconnect on connection loss

#### 1.2 Spotify Authentication
- OAuth 2.0 flow (Authorization Code with PKCE recommended)
- Initial auth via captive portal or QR code
- Refresh token management
- Token auto-refresh before expiry

#### 1.3 Playback Control
- Play/Pause toggle
- Next/Previous track
- Volume control (via Spotify API)
- Get current track info
- Get playback state

#### 1.4 Now Playing Display
- Album artwork (from Spotify API)
- Track title + artist name
- Progress bar with current position
- Real-time updates (poll every 1-2s or use WebSockets if available)

#### 1.5 Touch Controls
- Basic touch input handling
- Button press feedback
- Reliable touch calibration

### Priority 2 (Enhanced Features)

#### 2.1 Playlist Browser
- List user playlists
- Display playlist cover, name, owner
- Load playlist and play first track
- Show track count

#### 2.2 Progress Bar Scrubbing
- Touch and drag to seek
- Show scrub position preview
- Smooth seeking animation

#### 2.3 Settings Screen
- Display orientation toggle
- Screensaver configuration
- WiFi status
- Spotify account info
- Reconnect option

#### 2.4 Search (Future - Not MVP)
- Full search bar with on-screen keyboard
- Search tracks, artists, albums
- Tap to play search result

### Priority 3 (Nice-to-Have)

#### 3.1 Screensaver
- Dim display after X minutes
- Show simple info (time, current song)
- Wake on touch

#### 3.2 Device Selection
- List available Spotify devices
- Switch playback to another device

#### 3.3 Playlist Editing
- Add/remove tracks (future version)

---

## Spotify API Integration

### API Endpoints to Use

| Endpoint | Purpose |
|----------|---------|
| `GET /me/player` | Get current playback |
| `GET /me/player/currently-playing` | Get currently playing track |
| `PUT /me/player/play` | Start playback |
| `PUT /me/player/pause` | Pause playback |
| `POST /me/player/next` | Next track |
| `POST /me/player/previous` | Previous track |
| `PUT /me/player/volume` | Set volume |
| `GET /me/playlists` | Get user playlists |
| `GET /users/{user_id}/playlists` | Get playlists by owner |
| `GET /playlist/{id}/tracks` | Get playlist tracks |
| `GET /track/{id}` | Get track details |
| `GET /album/{id}/images` | Get album images |

### Authentication Flow

```
1. User navigates to Spotify auth URL (via captive portal or QR)
2. User logs in and grants permissions
3. Spotify redirects to callback with auth code
4. Device exchanges code for access + refresh tokens
5. Store tokens securely (flash)
6. Use access token for API calls
7. Refresh when expires (use refresh token)
```

### Required Scopes
- `user-read-playback-state`
- `user-modify-playback-state`
- `user-read-currently-playing`
- `user-library-read`
- `playlist-read-private`
- `playlist-read-collaborative`

---

## Configuration & Storage

### Config Structure (JSON)
```json
{
  "wifi": {
    "ssid": "NetworkName",
    "password": "WiFiPassword",
    "static_ip": false,
    "ip": "",
    "gateway": "",
    "subnet": ""
  },
  "spotify": {
    "client_id": "YOUR_CLIENT_ID",
    "client_secret": "YOUR_CLIENT_SECRET",
    "access_token": "encrypted",
    "refresh_token": "encrypted",
    "device_id": "unique-device-id"
  },
  "display": {
    "orientation": "landscape",
    "brightness": 75,
    "screensaver": {
      "enabled": true,
      "timeout_minutes": 5
    }
  },
  "ui": {
    "theme": "spotify-dark"
  }
}
```

### Storage Strategy
- Use LittleFS for persistent storage
- Config file: `/config.json`
- Cache album art: `/cache/` (with LRU eviction)
- Max cache size: ~10MB

---

## Error Handling

### Connection Errors
- **WiFi down:** Show error screen, retry automatically
- **Spotify API error:** Show user-friendly message, retry
- **Auth expired:** Auto-refresh token, prompt re-auth if fails

### Display Errors
- **Touch calibration off:** Re-calibrate option in settings
- **Display driver fail:** Retry init, show error if persists

### Recovery
- Watchdog timer for crash recovery
- State persistence on critical error
- Safe mode (minimal functionality)

---

## Development Guidelines

### Code Style
- **Language:** C++17
- **Naming:** `CamelCase` for classes, `snake_case` for functions/variables
- **File layout:** Header + implementation (`.hpp/.cpp`)
- **Comments:** Doxygen-style for public APIs

### Memory Management
- Use smart pointers where possible
- Avoid dynamic allocation in critical paths
- Monitor PSRAM usage (display buffers)
- Use object pools for frequent allocations

### Performance
- Target 60 FPS for UI updates
- Limit API polling (1-2 seconds)
- Use DMA for display SPI transfers
- Optimize image decoding

### Testing
- Unit tests for Spotify client logic
- Integration tests for API calls
- Hardware tests for display/touch

---

## Build & Deployment

### PlatformIO Configuration
```ini
[env:esp32-wrover]
platform = espressif32
board = esp32-wrover
framework = espidf
board_build.partitions = default.csv
board_build.arduino.memory_type = qio_opi
lib_deps =
    lvgl/lvgl@^8.3
    bblanchon/ArduinoJson@^6
    esphome/ESPAsyncWebServer-esphome@^2
monitor_speed = 115200
```

### Build Commands
```bash
# Build
platformio run

# Upload
platformio run --target upload

# Monitor serial
platformio device monitor

# Clean
platformio run --target clean
```

---

## GitHub Repository Structure

```
spotify-controller-esp32/
├── .github/
│   └── workflows/
│       └── ci.yml              # CI/CD pipeline
├── docs/
│   ├── README.md               # Project overview
│   ├── ARCHITECTURE.md         # System architecture
│   ├── API.md                  # Spotify API notes
│   ├── HARDWARE.md             # Hardware guide
│   ├── SETUP.md                # Setup instructions
│   └── CONTRIBUTING.md         # Contribution guidelines
├── src/                        # Source code (see architecture above)
├── test/                       # Unit tests
├── platformio.ini              # PlatformIO config
├── .gitignore
├── LICENSE
└── README.md                   # Main project README
```

### README.md Structure
```markdown
# Spotify Controller ESP32

A modern, touch-enabled Spotify controller built with ESP32 and LVGL.

## Features
- [x] WiFi connectivity with auto-reconnect
- [x] Spotify OAuth 2.0 authentication
- [x] Playback control (play/pause, next, prev, volume)
- [x] Beautiful UI inspired by Spotify + Apple Liquid Glass
- [x] Album artwork display
- [ ] Playlist browser (in progress)
- [ ] Progress bar scrubbing (planned)

## Hardware
- ESP32-WROVER
- SPI touch display (multiple resolutions supported)

## Quick Start

### Prerequisites
- PlatformIO
- Spotify Premium account
- WiFi network

### Installation

1. Clone repository
2. Configure `src/config/Config.hpp` with your Spotify credentials
3. Build and flash: `platformio run --target upload`

### Initial Setup

1. Power on device
2. Connect to "Spotify-Controller" WiFi AP
3. Open browser, navigate to Spotify auth page
4. Grant permissions
5. Device auto-connects to your WiFi

## Usage

See [docs/SETUP.md](docs/SETUP.md) for detailed instructions.

## Development

See [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) for development guidelines.

## License

MIT License - See LICENSE file
```

---

## Milestones

| Phase | Features | Target |
|-------|----------|--------|
| **Phase 1** (MVP) | WiFi, Auth, Now Playing, Basic Controls | Week 1 |
| **Phase 2** | Playlist Browser, Settings | Week 2 |
| **Phase 3** | Screensaver, Device Selection | Week 3 |
| **Phase 4** | Search, Playlist Editing | Future |

---

## Notes for Coding Agent

1. **Modularity is key:** Display drivers must be easily swappable
2. **UI quality matters:** Invest time in smooth animations and transitions
3. **Memory constraints:** ESP32 has limited RAM, use PSRAM efficiently
4. **Error recovery:** Don't crash on network errors, handle gracefully
5. **Code documentation:** Document public APIs with Doxygen comments
6. **GitHub:** Create repo, commit frequently with descriptive messages
7. **Testing:** At minimum, test core Spotify API functions

---

**Generated:** 2026-02-01
**Version:** 1.0
**Status:** Ready for Coding
