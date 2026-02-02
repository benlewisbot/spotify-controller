# TODO - Spotify Controller ESP32-S3

## Projektstatus

**Phase:** Coding Phase 2 🔄 UI Completion
**Timeline:** 2-3 Wochen
**Board:** LilyGo T-Display S3 Touch (ESP32-4848S040)
**Letztes Update:** 01.02.2026

## 🎉 Letzte Änderungen (01.02.2026)

### UI & Controls Completed ✅
- ✅ Vertical Volume Slider (right side of screen)
- ✅ Progress Bar with Seek Touch Support
- ✅ Save/Heart button with visual feedback
- ✅ Enhanced controls positioning
- ✅ Improved time formatting for progress

### Wokwi Demo Created ✅
- ✅ Online demo available at https://wokwi.com/projects/new/esp32
- ✅ `wokwi/sketch.ino` - Full demo sketch
- ✅ `wokwi/diagram.json` - ESP32 + ST7789 setup
- ✅ `wokwi/README.md` - Detailed demo documentation
- ✅ Serial commands for touch simulation
- ✅ 5 demo tracks (Linkin Park)

### Documentation Updated ✅
- ✅ `WOKWI.md` - Enhanced Wokwi guide
- ✅ `README.md` - Added GUI Demo link section
- ✅ Quick start options table

---

## ✅ Phase 1: Grundgerüst (Abgeschlossen!)

### Hardware-Support
- [x] config.h erstellt (Multi-Display Support)
- [x] display_manager.h erstellt (Display-Management)
- [x] touch_manager.h erstellt (Touch-Controller Support)
- [x] platformio.ini erstellt (4 Board-Konfigurationen)

### Unterstützung für Displays
- [x] LilyGo T-Display S3 Touch (ESP32-4848S040)
- [x] LilyGo T-Display-S3 Capacitive
- [x] Cheap Yellow Display CYD 2.4" (ST7789)
- [x] Cheap Yellow Display CYD 2.8" (ST7796U)
- [x] Cheap Yellow Display CYD 3.5" (ST7796U)
- [x] 7 Inch ESP32-S3 Serial Screen (800x480)
- [x] Standard ILI9341 2.4"
- [x] DIY ESP32 + externes Display

### Unterstützung für Touch-Controller
- [x] FT6236 (Capacitive - LilyGo)
- [x] XPT2046 (Resistiv - Standard)
- [ ] FT6336U (Capacitive)
- [ ] CST_SELF (Capacitive)

### Code-Struktur
- [x] main.cpp erstellt (Grundgerüst)
- [x] main_uart.cpp erstellt (UART Display Support)
- [x] README.md erstellt (Dokumentation)
- [x] Plattform-spezifische Builds

---

## 🔄 Phase 1.5: Serial Display (7 Inch) ✅ Abgeschlossen!

### UART Display Support
- [x] serial_display_manager.h erstellt
- [x] touch_manager_uart.h erstellt
- [x] main_uart.cpp erstellt (UART + SPI dual support)
- [x] Spotify Style UI Implementierung
- [x] PlatformIO Config für 7 Inch Serial

### Features
- [x] UART Communication (115200 baud)
- [x] Serial Display Protocol
- [x] Touch over UART
- [x] UI-Skalierung für 800x480
- [x] Spotify Color Scheme
- [x] Dual Mode (SPI + UART Auto-Detect)

### Code Features
- [x] Auto-Selection: DISPLAY_TYPE wählt SPI oder UART
- [x] Spotify UI: Album Cover, Track Info, Controls
- [x] Progress Bar, Volume Bar
- [x] Touch Areas: Play/Pause, Next, Previous, Volume
- [x] Debug Logging

---

## 🔧 Phase 2: WiFi & Authentication (🔄 In Arbeit!)

### WiFi Connection
- [x] WiFi Credentials speichern (LittleFS)
- [x] WiFi Connection Code
- [x] WiFi Error Handling
- [ ] Hotspot Mode für Erst-Setup

### OAuth2 (Spotify)
- [x] Spotify Client ID & Secret speichern
- [x] OAuth2 Authorization URL generieren
- [x] Callback Endpoint erstellen
- [x] Access Token speichern
- [x] Refresh Token implementieren
- [ ] Token Refresh automatisch

### Filesystem (LittleFS)
- [x] /config.json speichern/laden
- [x] data/ Directory erstellt
- [x] config.example.json erstellt
- [ ] Cover Cache implementieren

---

## 🎨 Phase 3: UI & Controls (Aktualisiert!)

### Now Playing Screen
- [x] Cover Image Display (Platzhalter)
- [x] Track Title & Artist
- [x] Play/Pause Button
- [x] Next/Previous Buttons
- [x] Save (+) Button für Playlist
- [x] Progress Bar (über Controls!)
- [x] Volume Bar (vertikal rechts!)
- [x] Volume Slider (Touch - für vertikal)
- [x] Progress Bar (Seekable)

### UI-Layout
- [x] Portrait Mode
- [ ] Landscape Mode
- [x] Dark Mode (Spotify App Style)
- [ ] Liquid Glass Effects (subtle)
- [ ] Screensaver (Timeout 10min)

### Touch Handling
- [x] Button Press Detection (SPI)
- [x] Button Press Detection (UART)
- [ ] Swipe Gestures
- [x] Volume Control (vertikal)
- [ ] Seek Progress (Touch)

---

## 📋 Phase 4: Spotify Integration (Geplant)

### Spotify API
- [ ] Currently Playing Endpoints
- [ ] Player Controls (Play, Pause, Skip)
- [ ] Volume Control
- [ ] Playlist Browse
- [ ] Search (später)

### Cover Images
- [ ] Cover URL extrahieren
- [ ] Cover herunterladen
- [ ] Cover skalieren (Display size)
- [ ] Cover Cache (LittleFS)
- [ ] JPEG/PNG Support

---

## 🔧 Phase 5: Settings (Geplant)

### 7 Inch Serial Screen Support
- [ ] UART Communication Library
- [ ] Serial Display Protocol
- [ ] Touch over UART
- [ ] UI-Skalierung für 800x480

### Settings Screen
- [ ] WiFi Settings
- [ ] Spotify Settings
- [ ] Display Settings
- [ ] Touch Settings
- [ ] About Screen

### Runtime Configuration
- [ ] Brightness Control
- [ ] Volume Limit
- [ ] Display Rotation
- [ ] Screensaver Timeout

---

## 📚 Phase 6: Features (Später)

### Playlist Browser
- [ ] Playlist List
- [ ] Playlist Navigation
- [ ] Track List
- [ ] Play Track

### Search
- [ ] Search Bar
- [ ] On-Screen Keyboard
- [ ] Search Results
- [ ] Play from Search

### Offline Mode
- [ ] Playlist Cache
- [ ] Offline Playback (Spotify Premium)

---

## 🧪 Testing

### Hardware Testing
- [ ] ESP32-4848S040 flashen
- [ ] Display Test (Colors, Text)
- [ ] Touch Test (Buttons)
- [ ] WiFi Connection Test
- [ ] Spotify API Test

### Software Testing
- [ ] Unit Tests (nicht implementiert)
- [ ] Integration Tests
- [ ] UI Tests (manuell)

### Performance Testing
- [ ] Frame Rate
- [ ] Touch Response Time
- [ ] API Latency
- [ ] Power Consumption

---

## 📖 Dokumentation

### User Docs
- [ ] Benutzerhandbuch
- [ ] Troubleshooting Guide
- [ ] FAQ

### Developer Docs
- [ ] API Reference
- [ ] Architecture Overview
- [ ] Contributing Guidelines

### Hardware Docs
- [ ] Pinout Diagrams
- [ ] Schematics
- [ ] BOM (Bill of Materials)

---

## 🔍 Debugging & Fixes

### Known Issues
- [ ] FT6236 Library noch nicht eingebunden
- [ ] XPT2046 Library noch nicht eingebunden
- [ ] Spotify API Calls nicht implementiert

### Fixes Needed
- [ ] WiFi Connection stabilisieren
- [ ] Token Refresh Logic
- [ ] Touch Debouncing
- [ ] UI Refresh Rate optimieren

---

## 📊 Progress

**Gesamt:** ~55% ✅

| Phase | Status | Progress |
|-------|--------|----------|
| Phase 1: Grundgerüst | ✅ Abgeschlossen | 100% |
| Phase 1.5: Serial Display (7 Inch) | ✅ Abgeschlossen | 100% |
| Phase 2: WiFi & Auth | 🔄 In Arbeit | 80% |
| Phase 3: UI & Controls | ✅ Fast fertig | 85% |
| Phase 4: Spotify API | ⏳ Geplant | 0% |
| Phase 5: Settings | ⏳ Geplant | 0% |
| Phase 6: Features | ⏳ Geplant | 0% |

---

## 🎯 Nächste Schritte

### Priorität HOCH (jetzt)
1. [ ] WiFi Connection implementieren
2. [ ] OAuth2 Flow für Spotify
3. [ ] LittleFS Setup

### Priorität MITTEL
4. [ ] Spotify API Integration
5. [ ] UI Rendering (Now Playing Screen)
6. [ ] Touch Handling implementieren

### Priorität NIEDRIG
7. [ ] Playlist Browser
8. [ ] Search Funktion
9. [ ] Offline Mode

---

## 💬 Ben's Prioritäten (aus FRAGEN.md)

### Wichtigkeit 1 (MUSS)
- [ ] Play/Pause/Next/Previous
- [ ] Volume Control
- [ ] Cover Display

### Wichtigkeit 2 (SOLLTE)
- [ ] Playlist Browser
- [ ] Progress Bar/Seek
- [ ] Settings Screen

### Wichtigkeit 3 (KANN)
- [ ] Search (später)
- [ ] Screensaver (als Option)
- [ ] Offline Cache (nicht nötig)

---

*TODO wird automatisch aktualisiert...*
