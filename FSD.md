# Spotify Controller - Functional Specification Document (FSD)

**Version:** 2.0
**Last Updated:** 2026-02-22
**Status:** Authoritative - Single Source of Truth
**Target Hardware:** Guition ESP32-S3-4848S040 (480x480 square display)

> This document is the **definitive specification** for the Spotify Controller project.
> It supersedes all other planning documents (TODO.md, REQUIREMENTS.md, PHASE5_PLAN.md,
> PHASE6_PLAN.md, 100_PERCENT_ROADMAP.md). Those files are historical context only.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Hardware Specification](#2-hardware-specification)
3. [Software Architecture](#3-software-architecture)
4. [Spotify Developer Setup](#4-spotify-developer-setup)
5. [Functional Requirements](#5-functional-requirements)
6. [UI/UX Specification](#6-uiux-specification)
7. [Screen Specifications](#7-screen-specifications)
8. [Implementation Phases](#8-implementation-phases)
9. [Codebase Status & Technical Debt](#9-codebase-status--technical-debt)
10. [Build & Flash Instructions](#10-build--flash-instructions)
11. [Testing Strategy](#11-testing-strategy)
12. [Risk Register](#12-risk-register)

---

## 1. Project Overview

### 1.1 Purpose

A dedicated hardware Spotify controller for desk use. The device has a 480x480 touchscreen
display and connects to the Spotify Web API to control playback on any Spotify Connect
device. It does **not** play audio locally - it is a remote control only.

### 1.2 Core Value Proposition

- Dedicated always-on Spotify controller on your desk
- No need to unlock your phone to skip/pause/browse
- Beautiful album art display
- Touch controls for playback, volume, playlists, and search

### 1.3 Key Design Decisions (Locked)

| Decision | Choice | Rationale |
|----------|--------|-----------|
| UI Framework | **LVGL 9.4.0** | Modern, memory-efficient, hardware-accelerated. All legacy TFT_eSPI UI code is deprecated. |
| Display Driver | **LovyanGFX** | Required for RGB parallel interface on ST7701S |
| Auth Flow | **OAuth 2.0 PKCE** | No client secret needed on device, more secure |
| Storage | **LittleFS** | Reliable, wear-leveling, atomic writes |
| WiFi Setup | **Captive Portal** | User-friendly first-time setup, no config file editing |
| Layout | **Square-optimized** | 480x480 display: album art top, info middle, controls bottom |
| Album Art | **JPEG decoding** | Real album art display using TJpg_Decoder or LVGL built-in |
| Visual Style | **Clean & polished Spotify-dark** | Dark theme, proper spacing, rounded corners, smooth lightweight animations |

---

## 2. Hardware Specification

### 2.1 Target Board: Guition ESP32-S3-4848S040

| Spec | Value |
|------|-------|
| MCU | ESP32-S3 (Dual-core LX7, 240MHz) |
| Flash | 16MB |
| PSRAM | 8MB (Octal) |
| Display | 480x480 ST7701S, RGB parallel interface |
| Touch | GT911 Capacitive (I2C) |
| Connectivity | WiFi 802.11 b/g/n, Bluetooth 5.0 LE |
| Power | USB-C (5V, desk-powered, no battery) |
| Buttons | BOOT, RESET |

### 2.2 Pin Configuration

```
Display (ST7701S RGB Parallel - 16-bit):
  Data:    B0=GPIO4, B1=GPIO5, B2=GPIO6, B3=GPIO7, B4=GPIO15
           G0=GPIO8, G1=GPIO20, G2=GPIO3, G3=GPIO46, G4=GPIO9, G5=GPIO10
           R0=GPIO11, R1=GPIO12, R2=GPIO13, R3=GPIO14, R4=GPIO0
  Control: DE=GPIO18, VSYNC=GPIO17, HSYNC=GPIO16, PCLK=GPIO21
  Backlight: GPIO38 (PWM, channel 7, 44.1kHz)

Touch (GT911 I2C):
  SDA=GPIO19, SCL=GPIO45, Address=0x5D, I2C Port 1, 400kHz
```

### 2.3 Memory Budget

| Resource | Total | Reserved | Available |
|----------|-------|----------|-----------|
| Flash | 16MB | ~4MB firmware | ~12MB (LittleFS, OTA) |
| PSRAM | 8MB | ~2MB display buffers | ~6MB (album art, cache) |
| Internal RAM | 512KB | ~200KB LVGL + stack | ~300KB heap |

---

## 3. Software Architecture

### 3.1 Technology Stack

| Component | Technology | Version |
|-----------|------------|---------|
| Framework | Arduino (via PlatformIO) | ESP32 Arduino Core |
| UI Library | LVGL | 9.4.0 |
| Display Driver | LovyanGFX | ^1.1.9 |
| JSON Parser | ArduinoJson | ^6.21.5 |
| Web Server | ESPAsyncWebServer | ^3.1.0 |
| JPEG Decoder | TJpg_Decoder or LVGL built-in | TBD |
| Build System | PlatformIO | Latest |

### 3.2 Architecture Diagram

```
+---------------------------------------------------+
|                   main.cpp                         |
|                  App Singleton                     |
+---------------------------------------------------+
|                                                    |
|  +----------+  +----------+  +--------------+     |
|  | Display  |  |  WiFi    |  |   Spotify    |     |
|  | Manager  |  |  Manager |  |   Client     |     |
|  +----+-----+  +----+-----+  +------+-------+     |
|       |              |               |             |
|  +----+-----+  +----+-----+  +------+-------+     |
|  | LovyanGFX|  | Captive  |  |    Auth      |     |
|  | ST7701S  |  | Portal   |  |   Manager    |     |
|  | GT911    |  | (setup)  |  |  (OAuth PKCE)|     |
|  +----------+  +----------+  +--------------+     |
|                                                    |
|  +--------------------------------------------+   |
|  |              Window Manager                 |   |
|  |  +------+ +------+ +--------+ +-------+    |   |
|  |  |  Now | | Auth | |Settings| |Playlist|   |   |
|  |  |Playing| |Screen| | Stack  | |Browser |   |   |
|  |  +------+ +------+ +--------+ +-------+    |   |
|  +--------------------------------------------+   |
|                                                    |
|  +----------+  +----------+  +--------------+     |
|  |  Config  |  |  Event   |  |    Cover     |     |
|  |  Manager |  |   Bus    |  |   Manager    |     |
|  |(LittleFS)|  |(pub/sub) |  | (JPEG cache) |     |
|  +----------+  +----------+  +--------------+     |
|                                                    |
+---------------------------------------------------+
|  LVGL 9.4.0 (display driver, input driver)        |
|  LovyanGFX (RGB parallel bus, GT911 touch)        |
|  FreeRTOS (tasks, mutexes, timers)                |
+---------------------------------------------------+
```

### 3.3 Source File Map

```
src/
  main.cpp                           # Entry point, App singleton
  app/
    App.hpp/cpp                      # Main application controller (singleton)
    State.hpp                        # AppState enum
    EventBus.hpp                     # Pub/sub event system
  config/
    Config.hpp/cpp                   # LittleFS config read/write (mutex-protected)
  display/
    Display.hpp                      # Abstract DisplayInterface / TouchInterface
    DisplayManager.hpp/cpp           # LVGL init, flush callbacks, touch callbacks
    drivers/
      ST7701SDisplay.hpp/cpp         # PRIMARY: LovyanGFX RGB parallel + GT911 touch
      ILI9341Display.hpp/cpp         # Legacy SPI driver (not used for target board)
      ILI9488Display.hpp/cpp         # Legacy SPI driver
      ST7789Display.hpp/cpp          # Legacy SPI driver
      ST7796UDisplay.hpp/cpp         # Legacy SPI driver
      FT6236Touch.hpp/cpp            # Legacy touch driver
      XPT2046Touch.hpp/cpp           # Legacy touch driver
    themes/
      SpotifyTheme.hpp/cpp           # LVGL theme colors/styles
  network/
    WiFiManager.hpp/cpp              # WiFi connect, reconnect, AP mode
  spotify/
    AuthManager.hpp/cpp              # OAuth 2.0 PKCE, captive portal, token exchange
    SpotifyClient.hpp/cpp            # API calls (play, pause, now playing, playlists, search)
    SpotifySecure.hpp/cpp            # TLS cert pinning (DigiCert Root CA)
  ui/
    WindowManager.hpp/cpp            # Screen lifecycle, transitions
    screens/
      NowPlaying.hpp/cpp             # Main player screen
      Auth.hpp/cpp                   # Auth/login screen
      Settings.hpp/cpp               # Settings menu
      MenuManager.hpp/cpp            # Navigation + overlay management
      WiFiSettingsScreen.hpp/cpp     # WiFi configuration
      SpotifyTokenScreen.hpp/cpp     # Token status / re-auth
      DisplaySettingsScreen.hpp/cpp  # Brightness, rotation
      AboutScreen.hpp/cpp            # Device info
  models.h/cpp                       # Data structures (TrackInfo, PlaylistInfo, etc.)
  cover_manager.h/cpp                # Album art download + LittleFS cache
  RuntimeConfig.cpp                  # Runtime settings persistence
  utils/
    Logger.hpp/cpp                   # Structured logging
    Timer.hpp/cpp                    # Non-blocking timers

include/
  config.h                           # Legacy multi-display config + UI constants
  errors.h                           # SpotifyError enum
  result.h                           # Result<T> / Status templates
  RuntimeConfig.hpp                  # RuntimeConfig struct + manager
```

### 3.4 Application Flow

```
Power On
  |
  v
[Init Config]    Load WiFi creds, Spotify tokens from LittleFS
  |
  v
[Init Display]   LovyanGFX ST7701S + GT911, LVGL 9 init
  |
  v
[Init WiFi]      Has stored creds?
  |                  |
  | YES              | NO
  |                  v
  |          [Start AP Mode]
  |          [Captive Portal]
  |          [User enters WiFi creds + Spotify auth]
  |                  |
  v                  v
[WiFi Connected]
  |
  v
[Check Spotify Tokens]   Has valid tokens?
  |                          |
  | YES                      | NO
  |                          v
  |                   [Show Auth Screen]
  |                   [Start OAuth PKCE]
  |                   [User opens URL, grants access]
  |                   [Callback -> tokens stored]
  |                          |
  v                          v
[Show Now Playing]
  |
  v
[Main Loop]      LVGL timer_handler()
                 Poll Spotify every 2s
                 Handle touch events
                 Update UI
```

---

## 4. Spotify Developer Setup

### 4.1 Create a Spotify Developer App

1. Go to https://developer.spotify.com/dashboard
2. Log in with your Spotify account (Premium required for playback control)
3. Click **"Create App"**
4. Fill in:
   - **App Name:** `ESP32 Spotify Controller`
   - **App Description:** `Hardware Spotify remote controller`
   - **Redirect URI:** `http://192.168.4.1:8080/callback`
     (This is the ESP32's AP-mode IP. Also add your expected station IP, e.g., `http://192.168.1.100:8080/callback`)
   - **APIs used:** Check "Web API"
5. Click **"Save"**
6. Go to **Settings** and copy your **Client ID**
7. You do **NOT** need the Client Secret (we use PKCE)

### 4.2 Required OAuth Scopes

```
user-read-playback-state
user-modify-playback-state
user-read-currently-playing
user-read-playback-position
user-library-read
user-library-modify
playlist-read-private
playlist-read-collaborative
```

### 4.3 Configure the Device

Edit `data/config.json` before first flash:

```json
{
  "wifi": {
    "ssid": "",
    "password": ""
  },
  "spotify": {
    "client_id": "YOUR_CLIENT_ID_HERE",
    "client_secret": ""
  },
  "display": {
    "brightness": 75,
    "orientation": 0,
    "screensaver_enabled": false,
    "screensaver_timeout": 5
  }
}
```

If WiFi credentials are left empty, the device starts in AP mode for captive portal setup.

---

## 5. Functional Requirements

### 5.1 Priority 1 - MVP (Must Have)

These features constitute the minimum viable product. The device must boot, connect,
authenticate, and control playback.

| ID | Requirement | Description | Status |
|----|-------------|-------------|--------|
| **FR-1.1** | Boot & Init | Device boots, initializes display (ST7701S via LovyanGFX), shows splash screen | Code exists, needs testing |
| **FR-1.2** | WiFi Connection | Auto-connect to stored WiFi. If no creds, start AP mode with captive portal for WiFi setup | Code exists, captive portal needs work |
| **FR-1.3** | Spotify Auth (PKCE) | OAuth 2.0 PKCE flow via captive portal web page. User opens URL, grants access, callback stores tokens | Code exists, untested |
| **FR-1.4** | Token Management | Store access + refresh tokens in LittleFS. Auto-refresh before expiry. Handle token expiry gracefully | Code exists |
| **FR-1.5** | Now Playing Display | Show current track: album art (JPEG), song title, artist name, progress bar with time labels | Code exists (placeholder art), needs square layout + JPEG |
| **FR-1.6** | Playback Controls | Play/Pause, Next Track, Previous Track via touch buttons | Code exists |
| **FR-1.7** | Volume Control | Volume slider (horizontal on square layout), adjust via Spotify API | Code exists |
| **FR-1.8** | Progress Bar | Show playback progress, update in real-time (poll every 2s) | Code exists |
| **FR-1.9** | Touch Input | Capacitive touch via GT911, reliable button hit detection | Code exists, untested on hardware |
| **FR-1.10** | Album Art (JPEG) | Download album art from Spotify CDN, decode JPEG, display on screen (240x240) | **Not implemented** - needs JPEG decoder |
| **FR-1.11** | Error Recovery | Handle WiFi drops (auto-reconnect), API errors (retry with backoff), auth failures (prompt re-auth) | Partial |

### 5.2 Priority 2 - Enhanced Features

| ID | Requirement | Description | Status |
|----|-------------|-------------|--------|
| **FR-2.1** | Settings Menu | Navigate from Now Playing to Settings via menu button. Sub-screens for WiFi, Spotify, Display, About | Screens exist, navigation needs testing |
| **FR-2.2** | WiFi Settings | View connection status, scan networks, connect to new network, static IP config | Screen exists |
| **FR-2.3** | Display Settings | Brightness slider (PWM backlight), screensaver toggle + timeout | Screen exists |
| **FR-2.4** | Spotify Token Screen | View token status (valid/expired), trigger re-auth, clear credentials | Screen exists |
| **FR-2.5** | About Screen | Show device info: version, board, CPU, RAM, WiFi IP, MAC address | Screen exists |
| **FR-2.6** | Progress Bar Seek | Touch/drag progress bar to seek to position in track | **Not implemented** |
| **FR-2.7** | Save/Heart Track | Toggle "liked" status for current track via Spotify API | **Visual only**, API not connected |
| **FR-2.8** | Shuffle / Repeat Toggle | Toggle shuffle and repeat modes | **Not implemented** |

### 5.3 Priority 3 - Full Features

| ID | Requirement | Description | Status |
|----|-------------|-------------|--------|
| **FR-3.1** | Playlist Browser | List user playlists with name, owner, track count. Scrollable list. Tap to open. | **Not implemented** (API exists in SpotifyClient) |
| **FR-3.2** | Track List | View tracks in a playlist. Tap track to play. Play All / Shuffle buttons. | **Not implemented** |
| **FR-3.3** | Search | Search bar with on-screen QWERTY keyboard. Search tracks. Tap result to play. | **Not implemented** |
| **FR-3.4** | Device Selector | List Spotify Connect devices. Switch playback to another device. | **Not implemented** (API exists) |
| **FR-3.5** | Swipe Gestures | Swipe left/right to navigate between screens. Swipe up/down to scroll lists. | **Not implemented** |

### 5.4 Priority 4 - Nice to Have (Post-MVP)

| ID | Requirement | Description | Status |
|----|-------------|-------------|--------|
| **FR-4.1** | Screensaver | Dim display after configurable timeout. Show clock + current track info. Wake on touch. | **Not implemented** |
| **FR-4.2** | Album Art Cache | Cache album art in LittleFS with LRU eviction (max ~2MB). | Cache structure exists, JPEG decode missing |
| **FR-4.3** | OTA Updates | Over-the-air firmware updates via WiFi. | **Not implemented** |
| **FR-4.4** | Multiple Themes | Light mode, AMOLED mode, custom color themes | **Not implemented** |
| **FR-4.5** | Screen Transitions | Smooth slide/fade transitions between screens (LVGL animations) | **Not implemented** |

---

## 6. UI/UX Specification

### 6.1 Design Principles

1. **Spotify-inspired dark theme** - Match the Spotify app's visual language
2. **Clean & polished** - Proper spacing, rounded corners, readable typography
3. **Touch-friendly** - Minimum touch target 44x44px, clear visual feedback
4. **Performance-conscious** - Lightweight animations only, no heavy blur/gradient effects
5. **Square-optimized** - All layouts designed for 480x480 aspect ratio

### 6.2 Color Palette

```
Background:        #121212  (Spotify dark)
Surface:           #282828  (Elevated cards/buttons)
Surface Hover:     #333333  (Touch feedback)
Spotify Green:     #1DB954  (Primary accent, play button, active states)
Spotify Green Dim: #1A7A3E  (Pressed state)
Text Primary:      #FFFFFF  (Titles, main content)
Text Secondary:    #B3B3B3  (Subtitles, captions, timestamps)
Text Disabled:     #535353  (Disabled items)
Error:             #E74C3C  (Error states)
Warning:           #F39C12  (Warnings)
Success:           #1DB954  (Success states, reuse green)
Progress BG:       #535353  (Progress bar background)
```

### 6.3 Typography

LVGL built-in Montserrat fonts:

| Use | Font | Size | Color |
|-----|------|------|-------|
| Screen title | montserrat_24 bold | 24px | #FFFFFF |
| Track title | montserrat_20 bold | 20px | #FFFFFF |
| Artist / subtitle | montserrat_16 | 16px | #B3B3B3 |
| Body text | montserrat_14 | 14px | #FFFFFF |
| Timestamps / captions | montserrat_12 | 12px | #B3B3B3 |
| Button labels | montserrat_18 | 18px | #FFFFFF |

### 6.4 Component Styles

| Component | Background | Border | Radius | Padding |
|-----------|------------|--------|--------|---------|
| Screen | #121212 | none | 0 | 0 |
| Card | #282828 | none | 12px | 12px |
| Button (primary) | #1DB954 | none | 32px (pill) | 16px |
| Button (secondary) | #282828 | none | 24px | 12px |
| Button (ghost) | transparent | none | 24px | 8px |
| Progress bar bg | #535353 | none | 4px | 0 |
| Progress bar fill | #1DB954 | none | 4px | 0 |
| Slider knob | #FFFFFF | none | circle | 0 |
| List item | transparent | bottom 1px #282828 | 0 | 12px v, 16px h |
| Text input | #282828 | 1px #535353 | 8px | 12px |

### 6.5 Spacing System

Base unit: 8px. Use multiples: 4, 8, 12, 16, 24, 32, 48.

| Spacing | Use |
|---------|-----|
| 4px | Tight spacing (between icon and label) |
| 8px | Small gap (between related elements) |
| 12px | Default internal padding |
| 16px | Standard margin / padding |
| 24px | Section spacing |
| 32px | Large spacing between major sections |
| 48px | Screen edge margins (top/bottom) |

### 6.6 Touch Targets

- Minimum touch target: 44x44px
- Recommended: 48x48px for primary actions
- Large: 64x64px for play/pause button
- Touch feedback: briefly change background to Surface Hover (#333333) on press

### 6.7 Animations (Lightweight)

- **Screen transitions:** 200ms slide left/right (or instant if performance is poor)
- **Button press:** Scale to 0.95x, 100ms ease-out
- **Progress bar:** Smooth value interpolation via `LV_ANIM_ON`
- **Volume slider:** Smooth value change
- **Loading states:** Spinning arc indicator (LVGL arc + animation)
- **No heavy effects:** No blur, no gradients, no glass morphism

---

## 7. Screen Specifications

### 7.1 Splash Screen (Boot)

Shown during initialization (1-3 seconds).

```
+------------------------------------+
|                                    |
|                                    |
|                                    |
|         # SPOTIFY                  |
|         CONTROLLER                 |
|                                    |
|         v1.0.0                     |
|                                    |
|      Connecting...                 |
|      (spinner)                     |
|                                    |
|                                    |
+------------------------------------+
480x480
```

**Behavior:**
- Show app name and version
- Show status text: "Initializing...", "Connecting to WiFi...", "Authenticating..."
- Transition to Now Playing or Auth screen when ready

### 7.2 Now Playing Screen (Main)

The primary screen. Optimized for 480x480 square display.

```
+------------------------------------+
|  =                          <3  @  |  Top bar (menu, save, settings)
|                                    |
|     +------------------------+     |
|     |                        |     |
|     |                        |     |
|     |    [ALBUM ARTWORK]     |     |  240x240px JPEG
|     |       (centered)       |     |
|     |                        |     |
|     |                        |     |
|     +------------------------+     |
|                                    |
|          Song Title                |  montserrat_20, white, center
|          Artist Name               |  montserrat_16, #B3B3B3, center
|                                    |
|    1:23 ========================== |  Progress bar + time labels
|                               3:45 |
|                                    |
|       |<<    [ > / || ]    >>|     |  Transport controls (centered)
|                                    |
|      -  ======================== + |  Volume slider (horizontal)
|                                    |
+------------------------------------+
480x480
```

**Layout Details:**
- **Top bar** (y: 0-48): Menu hamburger (left), Heart/Save (right), Settings gear (right)
- **Album art** (y: 56-296): 240x240px centered, 12px border radius, placeholder color if no art
- **Track info** (y: 304-360): Title (max 2 lines, ellipsis), Artist (1 line, ellipsis), centered
- **Progress** (y: 368-400): Thin bar (full width minus margins), time labels left and right
- **Controls** (y: 408-456): Previous (48x48), Play/Pause (64x64), Next (48x48), centered with 24px gaps
- **Volume** (y: 456-480): Horizontal slider spanning width minus margins

**Functional Notes:**
- Album art: Decode JPEG from Spotify CDN URL, cache in PSRAM, display via `lv_image`
- Progress bar: Poll Spotify API every 2s, interpolate between polls for smooth movement
- Play/pause icon toggles based on `isPlaying` state
- Volume slider: on value change, call `setVolume()` API (debounced, max 1 call/500ms)
- Heart button: filled = saved, outline = not saved. Tap toggles via API.
- Menu button: navigate to Settings
- Settings gear: shortcut to Settings

### 7.3 Auth Screen

Shown when Spotify authentication is required.

```
+------------------------------------+
|                                    |
|         # SPOTIFY                  |
|         CONTROLLER                 |
|                                    |
|    --------------------------------|
|                                    |
|    To connect your Spotify         |
|    account, open this URL:         |
|                                    |
|    +----------------------------+  |
|    | http://192.168.1.x:8080   |  |  URL display
|    +----------------------------+  |
|                                    |
|    Or connect to WiFi:             |
|    "SpotifyController"             |
|    and open any browser            |
|                                    |
|    Status: Waiting for auth...     |
|    (spinner)                       |
|                                    |
+------------------------------------+
```

**Behavior:**
- If WiFi connected: show device IP + port for auth URL
- If WiFi not connected: show AP name to connect to
- ESPAsyncWebServer serves auth page on port 8080
- Auth page has "Login with Spotify" button that redirects to Spotify auth URL
- On successful callback: store tokens, transition to Now Playing
- Show spinner while waiting

### 7.4 Settings Screen

```
+------------------------------------+
|  <-  Settings                      |  Header with back button
|                                    |
|  +------------------------------+  |
|  |  WiFi Settings             > |  |  Navigation item
|  +------------------------------+  |
|  +------------------------------+  |
|  |  Spotify Account           > |  |  Navigation item
|  +------------------------------+  |
|  +------------------------------+  |
|  |  Display Settings          > |  |  Navigation item
|  +------------------------------+  |
|                                    |
|  --------------------------------  |  Separator
|                                    |
|  +------------------------------+  |
|  |  About                     > |  |  Navigation item
|  +------------------------------+  |
|                                    |
|  +------------------------------+  |
|  |  Reboot                [OK] |  |  Action button
|  +------------------------------+  |
|                                    |
+------------------------------------+
```

**Sub-screens:** WiFi Settings, Spotify Token, Display Settings, About (see existing screen files).

**Navigation:**
- Back button (top-left) returns to Now Playing
- Tap navigation items to go to sub-screens
- Sub-screens have their own back buttons returning to Settings

### 7.5 Playlist Browser (FR-3.1)

```
+------------------------------------+
|  <-  Playlists                  Q  |  Header + search button
|                                    |
|  +------------------------------+  |
|  | [ART] Liked Songs            |  |  60px item height
|  |       234 songs              |  |
|  +------------------------------+  |
|  +------------------------------+  |
|  | [ART] Discover Weekly        |  |
|  |       Spotify - 30 songs     |  |
|  +------------------------------+  |
|  +------------------------------+  |
|  | [ART] Rock Classics          |  |
|  |       Ben - 156 songs        |  |
|  +------------------------------+  |
|  +------------------------------+  |
|  | [ART] Focus Music            |  |
|  |       89 songs               |  |
|  +------------------------------+  |
|  +------------------------------+  |
|  | [ART] 2026 Favorites         |  |
|  |       Ben - 45 songs         |  |
|  +------------------------------+  |
|                                    |
|           v scroll v               |
+------------------------------------+
```

**Behavior:**
- Load playlists from `GET /me/playlists` (paginated, 20 per page)
- Show loading spinner while fetching
- Scrollable list (LVGL `lv_list` or custom scroll container)
- Tap playlist: navigate to Track List for that playlist
- Search button: navigate to Search screen
- Back button: return to Now Playing
- Playlist art: 48x48 thumbnail (cached JPEG or placeholder)

### 7.6 Track List (FR-3.2)

```
+------------------------------------+
|  <-  Discover Weekly    > ALL  ~   |  Header + play all + shuffle
|                                    |
|  +------------------------------+  |
|  |  1.  Song Title              |  |  50px item height
|  |      Artist Name             |  |
|  +------------------------------+  |
|  +------------------------------+  |
|  |  2.  Another Song            |  |
|  |      Another Artist          |  |
|  +------------------------------+  |
|  ... (scrollable)                  |
+------------------------------------+
```

**Behavior:**
- Load tracks from `GET /playlists/{id}/tracks` (paginated)
- Tap track: play via API, navigate to Now Playing
- "Play All": play playlist from beginning
- Shuffle: play playlist in shuffle mode
- Scrollable list, load more on scroll to bottom

### 7.7 Search Screen (FR-3.3)

```
+------------------------------------+
|  <-  Search                     X  |
|                                    |
|  +------------------------------+  |
|  |  Search tracks...          Q |  |  Text input
|  +------------------------------+  |
|                                    |
|  +------------------------------+  |  Results
|  |  1. Track Title - Artist     |  |  (appear after search)
|  |  2. Track Title - Artist     |  |
|  |  3. Track Title - Artist     |  |
|  +------------------------------+  |
|                                    |
|  +------------------------------+  |  On-screen keyboard
|  | Q W E R T Y U I O P         |  |  (shows when input focused)
|  |  A S D F G H J K L          |  |
|  |   Z X C V B N M             |  |
|  | <x  [   SPACE   ]  ENTER    |  |
|  +------------------------------+  |
+------------------------------------+
```

**Behavior:**
- LVGL `lv_textarea` for search input
- LVGL `lv_keyboard` (built-in QWERTY keyboard) for text entry
- Debounce search: wait 500ms after last keystroke before API call
- Search via `GET /search?q=...&type=track&limit=20`
- Display results as scrollable list
- Tap result: play track, navigate to Now Playing

---

## 8. Implementation Phases

### Phase 0: Fix & Compile (Prerequisite)

**Goal:** Get a clean build that can be flashed to hardware.

| Task | Description | Effort |
|------|-------------|--------|
| P0.1 | Verify build compiles with current code | Low |
| P0.2 | Remove ARM Helium files from LVGL 9 (or add pre-build script) | Low |
| P0.3 | Fix any LVGL 9.4.0 API incompatibilities in screen files | Medium |
| P0.4 | Ensure all screens compile (they exist as .cpp, verify no .bak issues) | Medium |
| P0.5 | Clean up `#ifdef DISPLAY_ST7701S` paths to ensure ST7701S build works end-to-end | Low |
| P0.6 | Add JPEG library to `platformio.ini` | Low |
| P0.7 | Flash to hardware and verify display initializes (even if just a color screen) | Critical |

### Phase 1: Working Player (MVP)

**Goal:** Boot -> WiFi -> Auth -> Now Playing with real Spotify data and controls.

| Task | Description | Depends On | Effort |
|------|-------------|------------|--------|
| P1.1 | Redesign NowPlaying layout for 480x480 square display | P0.7 | Medium |
| P1.2 | Implement JPEG album art decoder + display | P0.6 | High |
| P1.3 | Connect NowPlaying UI to SpotifyClient (real polling + display) | P0.7 | Medium |
| P1.4 | Test WiFi connection on real hardware | P0.7 | Low |
| P1.5 | Test OAuth PKCE flow on real hardware | P1.4 | High |
| P1.6 | Test playback controls (play, pause, next, prev, volume) | P1.3, P1.5 | Medium |
| P1.7 | Test touch input + calibration on GT911 | P0.7 | Medium |
| P1.8 | Implement captive portal for first-time WiFi + Spotify setup | P1.4 | High |
| P1.9 | Add volume slider debouncing (max 1 API call per 500ms) | P1.6 | Low |
| P1.10 | Add splash screen during boot | P0.7 | Low |

### Phase 2: Settings & Polish

**Goal:** Settings navigation works, display settings apply, visual polish.

| Task | Description | Depends On | Effort |
|------|-------------|------------|--------|
| P2.1 | Wire up MenuManager navigation (NowPlaying <-> Settings <-> sub-screens) | P1.1 | Medium |
| P2.2 | Test all settings sub-screens on hardware | P2.1 | Medium |
| P2.3 | Implement progress bar seek (touch to seek position) | P1.3 | Medium |
| P2.4 | Connect Heart/Save button to Spotify API | P1.3 | Low |
| P2.5 | Add shuffle/repeat toggle buttons to NowPlaying | P1.3 | Low |
| P2.6 | Implement proper error screens (WiFi lost, API error, no device) | P1.3 | Medium |
| P2.7 | Visual polish pass: spacing, typography, animations | P2.1 | Medium |

### Phase 3: Playlists & Search

**Goal:** Browse playlists, view tracks, search, and play from lists.

| Task | Description | Depends On | Effort |
|------|-------------|------------|--------|
| P3.1 | Implement Playlist Browser screen (LVGL) | P2.1 | High |
| P3.2 | Implement Track List screen (LVGL) | P3.1 | Medium |
| P3.3 | Implement Search screen with LVGL keyboard | P2.1 | High |
| P3.4 | Connect playlist/track/search screens to SpotifyClient API | P3.1, P3.2, P3.3 | Medium |
| P3.5 | Add Device Selector screen | P2.1 | Medium |
| P3.6 | Implement swipe gestures for navigation | P3.1 | Medium |
| P3.7 | Add list scrolling with pagination (load more on scroll) | P3.1 | Medium |

### Phase 4: Stability & Optimization

**Goal:** Stable for 24+ hours, optimized performance, ready for daily use.

| Task | Description | Depends On | Effort |
|------|-------------|------------|--------|
| P4.1 | Implement album art LittleFS cache with LRU eviction | P1.2 | Medium |
| P4.2 | Memory leak testing (run for 24 hours, monitor heap) | P3 | High |
| P4.3 | Performance profiling: LVGL frame rate, touch latency | P3 | Medium |
| P4.4 | Implement screensaver (dim + clock after timeout) | P2 | Medium |
| P4.5 | Add watchdog timer for crash recovery | P3 | Low |
| P4.6 | Optimize PSRAM usage for display buffers | P4.3 | Medium |
| P4.7 | Stress test: rapid button presses, network drops, token expiry | P3 | High |

---

## 9. Codebase Status & Technical Debt

### 9.1 Current Build State

- **Last successful build:** 2026-02-16 (minimal build)
- **LVGL version:** Upgraded from 8.x to 9.4.0
- **Known build prerequisite:** Must remove LVGL 9 ARM Helium assembly files after `pio run`:
  ```bash
  rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/convert/helium
  rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/sw/blend/helium
  ```

### 9.2 Bug Fixes Applied

All 17 critical bugs identified in `OPUS_COMPLETE_AUDIT.md` have been fixed:
- Memory leaks (HTTP client in downloadImage, CoverManager, MenuManager overlay)
- WiFi event handler memory corruption
- millis() overflow in token expiry
- LVGL callback signatures updated to `lv_event_t*` (LVGL 9)
- String fragmentation (added `reserve()` calls)
- Mutex protection for LittleFS writes
- Null safety in JSON deserialization
- Missing error propagation in API methods

### 9.3 Known Technical Debt

| Issue | Severity | Description |
|-------|----------|-------------|
| LVGL 9 symbols | Medium | Some LVGL 8 symbols (custom) may not exist in LVGL 9. Need to verify all `LV_SYMBOL_*` usage. |
| NowPlaying layout | High | Currently landscape-oriented (side-by-side). Must be redesigned for 480x480 square. |
| No JPEG decoder | High | Album art shows colored placeholder. Need to add JPEG decode library. |
| Duplicate TrackInfo | Low | `SpotifyClient::TrackInfo` and `spotify::TrackInfo` both exist. Should consolidate. |
| Legacy SPI drivers | Low | ILI9341, ILI9488, ST7789, ST7796U drivers exist but are not needed for target board. Keep for compatibility but don't maintain. |
| GT911Touch coupling | Medium | `GT911Touch` class in ST7701SDisplay.hpp references `ST7701SDisplay*` but the touch read actually uses LovyanGFX panel touch. Coupling needs review. |
| WindowManager minimal | High | WindowManager only shows NowPlaying and Auth. Needs to integrate with MenuManager for full navigation. |
| Captive portal incomplete | Medium | AuthManager has captive portal skeleton but WiFi AP mode setup for first-time config needs work. |
| LVGL tick handling | Low | `lv_tick_inc()` is called in DisplayManager::update() which runs in main loop. Consider FreeRTOS task for consistent timing. |
| Unit tests | Medium | 13 tests exist in `test/` but have never been run. |

### 9.4 Files to Deprecate (Historical Only)

These files are legacy (pre-LVGL, TFT_eSPI-based) and should not be modified:

| File | Reason |
|------|--------|
| `include/config.h` | Legacy multi-display config. Constants may still be referenced. |
| `src/utils.cpp` | Legacy utilities. May contain referenced helpers. |
| `wokwi/*` | Wokwi simulation files. Not relevant to target hardware. |

### 9.5 Key Architectural Constraints

1. **Single-threaded UI:** LVGL must be called from one task only. All UI updates happen in `loop()`.
2. **PSRAM for buffers:** Display buffer and JPEG decode buffer must use PSRAM (`heap_caps_malloc(..., MALLOC_CAP_SPIRAM)`).
3. **No blocking calls in loop:** All HTTP requests should use non-blocking patterns or execute quickly. WiFi scanning should use async callbacks.
4. **Config write protection:** All LittleFS writes are mutex-protected. Use atomic write pattern (write to temp file, rename).
5. **Rate limiting:** Minimum 100ms between Spotify API calls. Volume changes debounced to 500ms.

---

## 10. Build & Flash Instructions

### 10.1 Prerequisites

```bash
# Install PlatformIO
pip install platformio

# Or use VS Code with PlatformIO extension
```

### 10.2 First-Time Build

```bash
# Clone / navigate to project
cd spotify-controller

# Build (this downloads all dependencies)
pio run -e esp32-s3-4848s040

# IMPORTANT: After first build, remove ARM assembly files
# (LVGL 9 includes ARM Helium code that fails on Xtensa/ESP32)
# On Linux/Mac:
rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/convert/helium
rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/sw/blend/helium
# On Windows (PowerShell):
Remove-Item -Recurse -Force .pio\libdeps\esp32-s3-4848s040\lvgl\src\draw\convert\helium
Remove-Item -Recurse -Force .pio\libdeps\esp32-s3-4848s040\lvgl\src\draw\sw\blend\helium

# Rebuild after removing helium files
pio run -e esp32-s3-4848s040
```

### 10.3 Upload Config Data (LittleFS)

```bash
# Upload data/ directory to LittleFS partition
pio run -e esp32-s3-4848s040 --target uploadfs
```

### 10.4 Flash Firmware

```bash
# Enter download mode:
# 1. Hold BOOT button
# 2. Press + release RESET button
# 3. Release BOOT button

# Upload firmware
pio run -e esp32-s3-4848s040 --target upload

# After flashing, press RESET to start
```

### 10.5 Serial Monitor

```bash
pio device monitor -b 115200
```

---

## 11. Testing Strategy

### 11.1 Hardware Validation (Phase 0)

| Test | Expected Result |
|------|-----------------|
| Flash firmware | Upload succeeds, serial output shows boot messages |
| Display init | Screen shows any content (even a colored rectangle) |
| Touch response | Serial prints touch coordinates when screen is tapped |
| WiFi connect | Device connects to configured WiFi, serial shows IP |
| LittleFS mount | Config loads without errors |

### 11.2 Integration Tests (Phase 1)

| Test | Expected Result |
|------|-----------------|
| Spotify auth flow | Auth URL shown, user can login, tokens stored |
| Now Playing poll | Current track info updates on screen every 2s |
| Play/Pause | Tapping play/pause button toggles playback on active device |
| Next/Previous | Tapping skip buttons changes track |
| Volume control | Moving slider changes volume on active device |
| Album art | JPEG album art displays correctly (not just placeholder) |

### 11.3 Stress Tests (Phase 4)

| Test | Expected Result |
|------|-----------------|
| 24-hour run | No crashes, no memory leak, heap stable |
| Rapid tapping | All button presses register, no double-fires |
| WiFi disconnect/reconnect | Auto-reconnects, UI shows status, playback resumes |
| Token expiry | Auto-refreshes without user intervention |
| No active device | Shows "No active device" message, does not crash |

---

## 12. Risk Register

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Display does not initialize | Medium | Blocker | Verify LovyanGFX pin config against Guition schematic. Test with simple color fill first. |
| Touch not working | Medium | High | GT911 I2C address may be 0x14 instead of 0x5D. Try both. Check I2C pull-ups. |
| LVGL 9 incompatibilities | Medium | Medium | Some API calls may differ from 9.0 to 9.4. Check LVGL 9 migration guide. |
| Insufficient RAM for JPEG | Low | High | Use PSRAM for JPEG decode buffer. Downscale to 240x240 before decode if needed. |
| Spotify rate limiting | Low | Medium | Enforce 100ms minimum between requests. Cache responses. Use exponential backoff on 429. |
| OAuth PKCE on ESP32 | Medium | High | mbedtls SHA256 is already implemented. Verify PKCE challenge/verifier work with Spotify. |
| WiFi instability | Medium | Medium | Auto-reconnect with exponential backoff. Show connection status on screen. |
| LVGL frame rate too low | Low | Medium | Reduce buffer size, use partial rendering, disable animations if needed. Target 30fps minimum. |
| LittleFS corruption | Low | High | Atomic writes (temp to backup to rename) already implemented. Mutex protection in place. |
| Board variant differences | Low | Medium | Some Guition boards have slightly different pin assignments. Document tested board revision. |

---

## Appendix A: Spotify API Quick Reference

### Playback Control
```
GET    /v1/me/player                    -> Player state
GET    /v1/me/player/currently-playing  -> Current track
PUT    /v1/me/player/play               -> Resume/play
PUT    /v1/me/player/pause              -> Pause
POST   /v1/me/player/next               -> Next track
POST   /v1/me/player/previous           -> Previous track
PUT    /v1/me/player/volume?volume_percent=50 -> Set volume
PUT    /v1/me/player/seek?position_ms=30000   -> Seek to position
PUT    /v1/me/player/shuffle?state=true       -> Toggle shuffle
PUT    /v1/me/player/repeat?state=track       -> Set repeat mode
```

### Library & Playlists
```
GET    /v1/me/playlists                 -> User playlists
GET    /v1/playlists/{id}/tracks        -> Playlist tracks
GET    /v1/me/tracks/contains?ids=...   -> Check saved tracks
PUT    /v1/me/tracks?ids=...            -> Save tracks
DELETE /v1/me/tracks?ids=...            -> Remove saved tracks
GET    /v1/me/player/devices            -> Available devices
PUT    /v1/me/player                    -> Transfer playback
```

### Search
```
GET    /v1/search?q=...&type=track&limit=20  -> Search tracks
```

### Image URLs
Album art URLs from Spotify API responses (in `album.images[]`):
- `640x640` - Full size (use for main display)
- `300x300` - Medium
- `64x64` - Thumbnail (use for playlist items)

---

## Appendix B: LVGL 9 API Notes

Key differences from LVGL 8.x used in migration:

```cpp
// Display creation (LVGL 9)
lv_display_t* disp = lv_display_create(480, 480);
lv_display_set_flush_cb(disp, flush_cb);
lv_display_set_buffers(disp, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

// Input device (LVGL 9)
lv_indev_t* indev = lv_indev_create();
lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
lv_indev_set_read_cb(indev, read_cb);

// Event callbacks (LVGL 9) - CRITICAL: signature is lv_event_t*
lv_obj_add_event_cb(obj, [](lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    void* user_data = lv_event_get_user_data(e);
}, LV_EVENT_CLICKED, user_data);

// Spinner replacement (LVGL 9 has no lv_spinner_create)
// Use lv_arc_create() with animation instead

// Style padding (LVGL 9 requires part selector)
lv_obj_set_style_pad_all(obj, 0, 0);  // Last param is selector (0 = LV_PART_MAIN)

// Button (LVGL 9)
lv_obj_t* btn = lv_button_create(parent);  // Not lv_btn_create

// Keyboard (LVGL 9 built-in)
lv_obj_t* kb = lv_keyboard_create(parent);
lv_keyboard_set_textarea(kb, textarea);
```

---

## Appendix C: File Dependencies for Phase 0 Build

Files that **must compile** for a minimal bootable build:

```
src/main.cpp
src/app/App.hpp, App.cpp, State.hpp, EventBus.hpp
src/config/Config.hpp, Config.cpp
src/display/Display.hpp, DisplayManager.hpp, DisplayManager.cpp
src/display/drivers/ST7701SDisplay.hpp, ST7701SDisplay.cpp
src/display/themes/SpotifyTheme.hpp, SpotifyTheme.cpp
src/network/WiFiManager.hpp, WiFiManager.cpp
src/spotify/SpotifyClient.hpp, SpotifyClient.cpp
src/spotify/AuthManager.hpp, AuthManager.cpp
src/spotify/SpotifySecure.hpp, SpotifySecure.cpp
src/ui/WindowManager.hpp, WindowManager.cpp
src/ui/screens/NowPlaying.hpp, NowPlaying.cpp
src/ui/screens/Auth.hpp, Auth.cpp
src/models.h, models.cpp
src/cover_manager.h, cover_manager.cpp
src/RuntimeConfig.cpp
src/utils/Logger.hpp, Logger.cpp
src/utils/Timer.hpp, Timer.cpp
include/errors.h, result.h, RuntimeConfig.hpp, config.h
lv_conf.h
```

Files that should compile but may need fixes for LVGL 9:
```
src/ui/screens/Settings.hpp, Settings.cpp
src/ui/screens/MenuManager.hpp, MenuManager.cpp
src/ui/screens/WiFiSettingsScreen.hpp, WiFiSettingsScreen.cpp
src/ui/screens/SpotifyTokenScreen.hpp, SpotifyTokenScreen.cpp
src/ui/screens/DisplaySettingsScreen.hpp, DisplaySettingsScreen.cpp
src/ui/screens/AboutScreen.hpp, AboutScreen.cpp
```

---

## Appendix D: Future — Cloud Proxy + QR Code Authentication (Phase 2)

> **Status:** NOT YET IMPLEMENTED. This section documents the recommended future
> approach for a polished, no-copy-paste OAuth flow. Implement once hardware
> testing of the current manual-paste flow is stable.

### Background & Motivation

The current OAuth flow (Phase 1) requires the user to:
1. Open a Spotify auth URL from the ESP32's web page
2. Authorize on Spotify, get redirected to `http://127.0.0.1:8888/callback` (which fails)
3. Manually copy the URL from the browser address bar and paste it back into the ESP32's web page

This works but is clunky. Spotify does **not** support Device Code flow (RFC 8628), and their
redirect URI policy (enforced Nov 2025) requires HTTPS for all non-loopback URIs. This means
the ESP32's local IP (`http://192.168.x.x`) cannot be used as a redirect URI directly.

### Recommended Architecture: QR Code + Cloudflare Worker

Deploy a tiny HTTPS cloud function (~50 lines) as the OAuth redirect target. The ESP32
displays a QR code on its touchscreen — the user scans it with their phone, completes
Spotify login on the phone browser, and the ESP32 automatically receives the tokens.

#### Flow

```
ESP32 Touchscreen          Phone Browser              Cloud Worker           Spotify
      |                         |                         |                    |
  [1] Generate unique           |                         |                    |
      device_state token        |                         |                    |
      |                         |                         |                    |
  [2] Display QR code on        |                         |                    |
      480x480 screen encoding:  |                         |                    |
      https://worker/auth?      |                         |                    |
      device_state=ABC123       |                         |                    |
      |                         |                         |                    |
      |   [3] User scans QR --> |                         |                    |
      |                         | -- [4] GET /auth ------> |                    |
      |                         |                         | -- [5] 302 to ----> |
      |                         |                         |    Spotify /authorize
      |                         | <-- [6] Spotify login page ----------------> |
      |                         | --- [7] User approves --> |                   |
      |                         |                          | <-- [8] Redirect   |
      |                         |                          |    ?code=X&state=Y |
      |                         |                          |                    |
      |                         |                   [9] Worker exchanges code   |
      |                         |                       for tokens using PKCE   |
      |                         |                       Stores tokens keyed     |
      |                         |                       by device_state         |
      |                         | <-- [10] "Success! You can close this" ---   |
      |                         |                          |                    |
  [11] ESP32 polls              |                          |                    |
       GET /poll?state=ABC123   |                          |                    |
       every 3 seconds          | -----------------------> |                    |
      |                         |                          |                    |
  [12] Receives tokens  <------ | --- JSON tokens -------- |                    |
      |                         |                          |                    |
  [13] Stores tokens in NVS,    |                          |                    |
       shows "Authenticated!"   |                          |                    |
       on touchscreen           |                          |                    |
```

#### Cloud Worker Implementation (Cloudflare Workers, ~0 cost)

Three routes:

| Route | Method | Purpose |
|---|---|---|
| `/auth?device_state=X` | GET | Generates PKCE, stores verifier keyed by state, redirects to Spotify `/authorize` |
| `/callback?code=X&state=X` | GET | Exchanges code for tokens using stored PKCE verifier, stores tokens keyed by device_state |
| `/poll?state=X` | GET | Returns `{"status":"pending"}` or `{"status":"ready","access_token":"...","refresh_token":"..."}` |

The worker uses KV storage (free tier: 100K reads/day) for ephemeral state. Tokens are
deleted after retrieval or after 10-minute TTL.

#### Security Considerations

- PKCE code_verifier is generated and stored server-side in the worker (never on ESP32)
- Tokens pass through the worker briefly but are encrypted in transit (HTTPS) and deleted after retrieval
- The `device_state` token is a 32-byte random value, making brute-force polling impractical
- No client secret is needed (PKCE flow)
- The worker does not store any long-term user data

#### ESP32 Implementation Requirements

- QR code rendering library (e.g., `qrcode` Arduino library, ~2KB flash)
- LVGL image widget to display the QR code on the 480x480 screen
- Background polling task: `GET https://worker/poll?state=X` every 3 seconds
- Fallback: manual paste flow remains available if cloud proxy is unreachable

#### Spotify Dashboard Configuration

Register redirect URI: `https://your-worker.your-subdomain.workers.dev/callback`

#### Why Not Other Approaches?

| Approach | Why Not |
|---|---|
| ESP32 local IP as redirect URI | Spotify requires HTTPS for non-loopback. Self-signed certs break browser UX. |
| mDNS `.local` domain | Not loopback, needs HTTPS. Android mDNS is unreliable in browsers. |
| Captive portal DNS hijack of 127.0.0.1 | 127.0.0.1 is a literal IP, not a DNS name — cannot be intercepted. |
| Auto-extract code from popup via JS | Cross-origin policy blocks reading popup.location.href. |
| Spotify Device Code flow (RFC 8628) | Spotify does not implement this flow. |
| Companion mobile app | Best UX but requires building/distributing an app — overkill for DIY. |

---

*Document created: 2026-02-22*
*Last updated: 2026-02-23 — Added Appendix D (Cloud Proxy + QR Code auth roadmap)*
*This is the authoritative specification. All implementation should follow this document.*
