# Spotify Controller ESP32 - Coding Requirements

## Projekt-Übersicht

Ein dedizierter Spotify Controller auf ESP32 mit Touch-Display für den Schreibtisch. Wie eine "Hardware Spotify App" mit physischer Präsenz.

---

## Hardware-Anforderungen

### Empfohlene Komponenten

| Komponente | Empfehlung | Alternativen | Preisklasse |
|-----------|-----------|--------------|-------------|
| ESP32 Board | ESP32-WROVER / ESP32-S3 | LilyGo T-Display S3, M5Stack Core2 | 15-30€ |
| Touch Display | 2.4" ILI9341 (320x240) oder 3.5" (480x320) | ST7789, ST7735 (kleiner) | 10-20€ |
| Audio (optional) | I2S DAC oder PAM8403 | Kopfhörer-Jack an ESP32 | 5-10€ |
| Gehäuse | 3D gedruckt oder offenes Board | | 0-10€ |
| Strom | USB-C 5V/2A | Li-Ion Battery für mobil | 5€+ |

### Spezifikationen
- **Minimum:** ESP32 (4MB Flash), 240x320 Touch-Display, WiFi
- **Optimal:** ESP32-S3 (16MB Flash), 480x320 Capacitive Touch, I2S DAC
- **Premium:** M5Stack Core2 (alles integriert, Batterie)

### Board-Optionen

**1. DIY (Budget: 25-35€)**
- ESP32-WROVER (mit PSRAM)
- ILI9341 2.4" Touch-Display
- Jumper wires, Breadboard oder eigenes PCB

**2. Semi-Integrated (Budget: 35-50€)**
- LilyGo T-Display S3 (alles in einem, USB-C, Lipo-Support)
- M5Stack Cardputer (klein, aber Touch-Keyboard)

**3. All-in-One (Budget: 60-80€)**
- M5Stack Core2 (Display, Touch, Batterie, Audio, Gehäuse)
- ESP32-S3-Box (WiFi + Audio + Display)

---

## Software-Architektur

### Tech Stack
- **MCU:** ESP32 / ESP32-S3
- **Framework:** Arduino IDE / PlatformIO (C++)
- **Libraries:**
  - `lvgl` (Light and Versatile Graphics Library) - UI Framework
  - `TFT_eSPI` oder `LovyanGFX` - Display Treiber
  - `ESP32WiFi` / `WiFiClientSecure` - HTTPS
  - `ArduinoJson` - JSON Parsing
  - `SpotifyArduino` oder `ESP32Spotify` - Spotify API Wrapper
- **Spotify:** Web API (OAuth2)

### Architektur-Diagramm
```
┌─────────────────────────────────────────────────────────┐
│                    ESP32 + Touch Display                 │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────────────────────────────────────────┐  │
│  │            LVGL UI Layer                           │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐        │  │
│  │  │ Album    │  │ Controls │  │ Playlist │        │  │
│  │  │ Cover    │  │          │  │ Browser  │        │  │
│  │  └──────────┘  └──────────┘  └──────────┘        │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐        │  │
│  │  │ Search   │  │ Volume   │  │ Settings │        │  │
│  │  │ Bar      │  │ Slider   │  │ Menu     │        │  │
│  │  └──────────┘  └──────────┘  └──────────┘        │  │
│  └──────────────────────────────────────────────────┘  │
│                    ↑↓                                   │
│  ┌──────────────────────────────────────────────────┐  │
│  │           Spotify Client Layer                   │  │
│  │  - OAuth2 Authentication                         │  │
│  │  - Current Track State Polling                  │  │
│  │  - Play/Pause/Next/Previous Control             │  │
│  │  - Volume Control                               │  │
│  │  - Playlist/Track Search                        │  │
│  │  - Cover Image Download + JPEG decode           │  │
│  └──────────────────────────────────────────────────┘  │
│                    ↑↓                                   │
│  ┌──────────────────────────────────────────────────┐  │
│  │           WiFi + HTTPS Layer                      │  │
│  │  - WiFi Connection                               │  │
│  │  - SSL/TLS for Spotify API                       │  │
│  │  - Image Download (HTTPS)                        │  │
│  └──────────────────────────────────────────────────┘  │
│                                                          │
└─────────────────────────────────────────────────────────┘
                         ↑↓
                    ┌────────┐
                    │Spotify │
                    │  API   │
                    └────────┘
```

---

## Spotify API Integration

### OAuth2 Authentication Flow

**Initial Setup (einmalig via Web-Browser):**
1. ESP32 erstellt WiFi Hotspot
2. User verbindet mit Handy und öffnet `http://192.168.4.1`
3. Redirect zu Spotify OAuth2 URL
4. User authentifiziert und gibt Permission
5. Spotify redirect mit `code` Parameter
6. ESP32 tauscht `code` gegen `access_token` und `refresh_token`
7. Tokens werden in SPIFFS/LittleFS gespeichert

**Runtime:**
- `access_token` (1 Stunde gültig) für API Calls
- `refresh_token` für erneuern
- Automatisch refreshen wenn token abläuft

### API Endpoints (Verwendet)

| Endpoint | Methode | Zweck | Rate Limit |
|----------|---------|-------|------------|
| /v1/me/player/currently-playing | GET | Aktueller Track + Cover | 1 req/sec |
| /v1/me/player/play | PUT | Playback starten | 1 req/sec |
| /v1/me/player/pause | PUT | Pause | 1 req/sec |
| /v1/me/player/next | POST | Nächster Track | 1 req/sec |
| /v1/me/player/previous | POST | Vorheriger Track | 1 req/sec |
| /v1/me/player/seek | PUT | Im Track seeken | 1 req/sec |
| /v1/me/player/volume | PUT | Lautstärke | 1 req/sec |
| /v1/me/playlists/{id}/tracks | GET | Playlist Tracks | 1 req/sec |
| /v1/search | GET | Track/Playlist suchen | 1 req/sec |

### Spotify Developer Account Setup

**Benötigt:**
1. Spotify Account (Premium für Playback Control)
2. Dashboard: developer.spotify.com
3. Neue App anlegen
4. Redirect URI: `http://192.168.4.1/callback` (ESP32 Hotspot)
5. Client ID und Client Secret notieren

**Scopes (Permissions):**
- `user-read-playback-state`
- `user-modify-playback-state`
- `user-read-currently-playing`
- `user-library-read`
- `playlist-read-private`
- `playlist-read-collaborative`

---

## UI/UX Design

### Screen Layout (Portrait 320x480)

```
┌──────────────────────────────┐
│       Album Cover            │
│    (quadratisch, zentriert)  │
│         200x200px            │
│                              │
│   ┌────────────────────┐     │
│   │  Track Title      │     │
│   │  Artist Name      │     │
│   │  Album Name       │     │
│   └────────────────────┘     │
│                              │
│    [⏮]  [▶/⏸]  [⏭]         │
│                              │
│    Volume: [━━━○━] 50%       │
│                              │
│    [Search]  [Playlist] [⚙] │
│                              │
└──────────────────────────────┘
```

### Screens

**1. Main Screen (Now Playing)**
- Album Cover (zentriert, quadratisch)
- Track Info (Title, Artist, Album)
- Playback Controls (Previous, Play/Pause, Next)
- Volume Slider
- Progress Bar (seekable)
- Navigation Buttons (Search, Playlist, Settings)

**2. Search Screen**
- Touch Keyboard (optional) oder Web-App Integration
- Kategorie-Tabs (Tracks, Playlists, Artists)
- Search Results List (mit Cover Thumbnails)
- Tap to Play

**3. Playlist Browser**
- User Playlists Liste
- Folder/Struktur wenn möglich
- Tap to open playlist
- Playlist Tracks Liste
- Tap to play track

**4. Settings Screen**
- WiFi Settings (SSID, Password)
- Spotify Re-Auth
- Volume Limit
- Display Brightness
- Sleep Timer

### Interaction Design

- **Tap:** Primary Action (Play, Pause, Select)
- **Swipe:** Secondary Actions (Volume, Seek)
- **Long Press:** Context Menu (Add to Library, Queue)
- **Haptic Feedback (optional):** Vibration motor für tactile Bestätigung

### Touch Handling

- Capacitive Touch: Resistive kann tricky sein
- Multi-Touch: Nicht erforderlich
- Gestures: Swipe für Volume, Tap für Controls
- Debouncing: Min 100ms zwischen Touch Events

---

## Daten-Speicherung

### Filesystem (LittleFS/SPIFFS)

```
/config/
  ├── wifi.json           # WiFi Credentials
  ├── spotify.json        # Access/Refresh Tokens
  └── settings.json       # User Preferences

/cache/
  └── album_covers/       # Gecachte Cover Images
      ├── abc123.jpg      # Max 50MB Limit
      └── def456.jpg

/playlists/               # Offline Playlist Cache (optional)
  ├── playlist_1.json
  └── playlist_2.json
```

### JSON Strukturen

**wifi.json**
```json
{
  "ssid": "YourWiFi",
  "password": "YourPassword"
}
```

**spotify.json**
```json
{
  "access_token": "xxx",
  "refresh_token": "yyy",
  "expires_at": 1738260000,
  "client_id": "xxx",
  "client_secret": "yyy"
}
```

**settings.json**
```json
{
  "volume_limit": 80,
  "brightness": 70,
  "sleep_timer": 0,
  "auto_off": true
}
```

---

## Image Handling

### Cover Download

**Challenge:** Spotify API gibt 640x640 JPEG URLs zurück
**Problem:** JPEG decode auf ESP32 ist RAM-intensiv
**Solution:**

1. **Request Size:** 64x64 oder 80x80 reicht für 2.4" Display
2. **Progressive Download:** Chunk-by-Chunk in Buffer
3. **JPEG Decoder:** `JPEGDEC` oder `TJpgDec` (tailored for ESP32)
4. **Cache Strategy:**
   - Hash Image URL als Filename
   - Prüfe ob Cache vorhanden
   - If yes: Display from Cache
   - If no: Download + Decode + Cache
   - Max Cache Size: 50MB (alte Files löschen)

**Alternative:** Spotify Web Playback SDK (JavaScript) + ESP32 als Remote Control. Aber das requires ein anderes Setup.

---

## Performance & Constraints

### RAM (ESP32: 520KB SRAM)
- LVGL UI Buffer: ~100KB (320x240 @ 16bit = 153KB, double-buffered)
- WiFi Stack: ~40KB
- Spotify JSON Response: ~20KB
- JPEG Decoder Buffer: ~50KB
- **Critical:** PSRAM ist Pflicht für 320x480+ Display

### Flash (4MB min, 16MB optimal)
- Firmware: ~1MB
- Image Cache: Rest
- Configuration Files: <10KB

### API Rate Limits
- Spotify: ~180 req/min pro User
- Strategie: Polling nur alle 3-5 Sekunden
- Events: Only poll when playback state changes (detected via UI)

### Power
- Idle: ~100mA (WiFi an)
- Display on: ~200-300mA
- Touch Event: Burst ~500mA
- Battery Life: 3-4h mit 2000mAh LiPo

---

## Code Structure (File Overview)

```
spotify-controller/
├── src/
│   ├── main.cpp              # Entry point
│   ├── wifi_setup.cpp        # WiFi connection + Hotspot
│   ├── spotify_auth.cpp      # OAuth2 flow
│   ├── spotify_api.cpp       # API wrapper
│   ├── ui_manager.cpp         # LVGL screens + navigation
│   ├── image_downloader.cpp  # Cover download + decode
│   ├── config_manager.cpp    # JSON config save/load
│   ├── playlist_manager.cpp  # Playlist handling
│   └── touch_handler.cpp     # Input processing
├── include/
│   ├── spotify_api.h
│   ├── ui_manager.h
│   └── ... (headers)
├── lib/
│   ├── lvgl/                 # LVGL library
│   ├── TFT_eSPI/             # Display driver
│   ├── ESP32Spotify/         # Spotify wrapper
│   └── JPEGDEC/              # JPEG decoder
├── data/                     # Upload to SPIFFS
│   └── fonts/                # LVGL fonts
├── platformio.ini            # Build config
└── README.md                 # Setup instructions
```

---

## Implementation Phases

### Phase 1: MVP (Minimum Viable Product) - 1-2 Nächte
- WiFi Connection
- OAuth2 Authentication
- Now Playing Screen (Cover + Title + Controls)
- Play/Pause/Next/Previous

### Phase 2: Core Features - 2-3 Nächte
- Volume Control
- Playback Progress + Seek
- Playlist Browser (Read-only)
- Cover Caching

### Phase 3: Advanced Features - 2-3 Nächte
- Search Function
- Full Playlist Management
- Offline Cache
- Settings Screen

### Phase 4: Polish - 1 Nacht
- UI Improvements (Animations, Transitions)
- Error Handling
- Stability Fixes
- Documentation

---

## Offene Fragen für Ben (bitte morgen durchgehen)

### Hardware
1. Welches ESP32 Board hast du oder bevorzugst du? (DIY, LilyGo, M5Stack?)
2. Welches Display? (Größe, Auflösung, Touch-Typ?)
3. Audio Output nötig? (Lautsprecher am Controller oder nur Remote Control?)
4. Mobil oder Desktop? (Battery vs USB-Strom)

### Software
5. Playlist Management: Nur read-only oder auch Edit (add tracks, create)?
6. Search: Full search bar mit Tastatur oder Kategorien (Top Hits, Playlists)?
7. Offline Modus: Cache Playlists für Spotify-Free User?
8. Multi-Device: Sollen mehrere Controller parallel laufen können?

### UX/UI
9. Display Orientation: Portrait oder Landscape?
10. Dark Mode only oder auch Light Mode?
11. Screensaver: Automatisch dimmen nach X Minuten?
12. Haptic Feedback: Vibration motor für tactile bestätigung?

### Spotify
13. Premium oder Free Account? (Playback Control requires Premium)
14. Geräte-Einschränkung: Soll der Controller ein eigenes Spotify Device sein oder nur existing devices steuern?
15. Volume Control: System Volume oder Spotify Volume?

### Budget & Zeit
16. Budget für Hardware? (25€ vs 80€ macht großen Unterschied)
17. Zeitrahmen: MVP bis wann? (1 Woche? 2 Wochen?)
18. Priority: Features ranking (1=wichtig, 3=nice-to-have)?

---

## Next Steps

1. **Ben durch Fragen gehen** → Konkrete Hardware & Feature-Spec
2. **Final Requirements** → Architektur finalisieren
3. **Coding beginnt** → Phase 1: MVP
4. **Testing** → Hardware-Integration + Spotify OAuth
5. **Polish** → UI Improvements + Bug Fixes

---

*Created: 2026-01-30*
*Status: Waiting for Requirements Confirmation*
