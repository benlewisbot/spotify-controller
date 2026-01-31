# Spotify Controller ESP32-S3

Ein Spotify Remote Controller mit Touch-Display für ESP32-S3.

**Unterstützte Boards:**
- ✅ LilyGo T-Display S3 Touch (ESP32-4848S040) - **Hauptziel**
- ✅ LilyGo T-Display-S3 Capacitive
- ✅ Cheap Yellow Display CYD 2.4/2.8/3.5/7"
- ✅ Standard ILI9341 2.4"
- ✅ DIY ESP32 + externes Display

**Features:**
- ✅ Multi-Display Support (8 Displays)
- ✅ SPI + UART Dual Mode (Auto-Detect)
- ✅ Capacitive + Resistive Touch
- ✅ Spotify Style UI (Dark Mode)
- ✅ Touch Controls (Play/Pause/Next/Previous/Volume)
- ⏳ WiFi Connection (TODO)
- ⏳ Spotify API Integration (TODO)

---

## 🚀 Schnellstart (ESP32-4848S040 / LilyGo T-Display S3 Touch)

### 1. Hardware
- LilyGo T-Display S3 Touch (ESP32-4848S040)
- USB-C Kabel
- WiFi

### 2. Software

**Voraussetzungen:**
```bash
# PlatformIO installieren
pip install platformio

# VS Code Extension: PlatformIO
```

**Kompilieren:**
```bash
cd /home/tod/clawd/projects/spotify-controller
pio run
```

**Flashen:**
```bash
# Auf das ESP32-4848S040 flashen
pio run --target upload
```

**Serial Monitor:**
```bash
pio device monitor
```

### 3. Konfiguration

**Display-Typ ändern:**
`include/config.h` öffnen und `DISPLAY_TYPE` wählen:

```cpp
// LilyGo T-Display S3 Touch (ESP32-4848S040)
#define DISPLAY_TYPE ILI9488_S3

// Oder andere:
// #define DISPLAY_TYPE ST7789_S3
// #define DISPLAY_TYPE CYD_2_4_ST7789  // Cheap Yellow Display 2.4"
// #define DISPLAY_TYPE CYD_2_8_ST7796U // Cheap Yellow Display 2.8"
// #define DISPLAY_TYPE CYD_3_5_ST7796U // Cheap Yellow Display 3.5"
// #define DISPLAY_TYPE CYD_7_INCH      // 7 Inch Serial Screen
// #define DISPLAY_TYPE ILI9341_S3
// #define DISPLAY_TYPE DIY_ILI9341
```

---

## 📦 Dateistruktur

```
spotify-controller/
├── include/
│   ├── config.h                    # Konfiguration (Display-Typ wählen)
│   ├── display_manager.h           # SPI Display-Management
│   ├── serial_display_manager.h    # UART Display-Management (7 Inch)
│   ├── touch_manager.h             # SPI Touch-Controller
│   └── touch_manager_uart.h        # UART Touch-Controller (7 Inch)
├── src/
│   ├── main.cpp                   # Haupt-Code (SPI Display)
│   └── main_uart.cpp              # Haupt-Code (UART Display + Dual Mode)
├── platformio.ini                 # PlatformIO Konfiguration
├── docs/
│   └── UI_DESIGN.md               # UI Layouts und Pixel-Koordinaten
├── REQUIREMENTS.md                # Detaillierte Anforderungen
├── FRAGEN.md                      # Ben's Antworten
├── TODO.md                        # Fortschritt
└── README.md                      # Diese Datei
```

---

## 🖥 Unterstützte Displays

| Typ | Name | Auflösung | Touch | Pinout | Interface |
|-----|------|-----------|-------|--------|-----------|
| ILI9488_S3 | LilyGo T-Display S3 Touch | 320x480 | FT6236 | Automatisch | SPI |
| ST7789_S3 | LilyGo T-Display-S3 | 320x170 | FT6236 | Automatisch | SPI |
| CYD_2_4_ST7789 | Cheap Yellow Display 2.4" | 240x320 | FT6236 | Automatisch | SPI |
| CYD_2_8_ST7796U | Cheap Yellow Display 2.8" | 240x320 | FT6236 | Automatisch | SPI |
| CYD_3_5_ST7796U | Cheap Yellow Display 3.5" | 320x480 | FT6236 | Automatisch | SPI |
| CYD_7_INCH | 7 Inch Serial Screen | 800x480 | FT6236 | Automatisch | UART |
| ILI9341_S3 | Standard ILI9341 2.4" | 240x320 | XPT2046 | Automatisch | SPI |
| DIY_ILI9341 | Custom DIY | Variabel | XPT2046 | Manual | SPI |

---

## 🔧 Display-Konfiguration

### LilyGo T-Display S3 Touch (ESP32-4848S040)
```cpp
// include/config.h
#define DISPLAY_TYPE ILI9488_S3

// Pinout (automatisch):
// TFT_MISO: 12
// TFT_MOSI: 11
// TFT_SCLK: 13
// TFT_CS:   38
// TFT_DC:   40
// TFT_RST:  39
// TOUCH_INT: 18
```

### Cheap Yellow Display CYD (2.4" / 2.8" / 3.5")
Günstige ESP32-WROVER-TYPE-C Boards mit Touch-Display.

```cpp
// include/config.h
#define DISPLAY_TYPE CYD_2_4_ST7789  // 2.4"
// oder
#define DISPLAY_TYPE CYD_2_8_ST7796U // 2.8"
// oder
#define DISPLAY_TYPE CYD_3_5_ST7796U // 3.5"

// Pinout (automatisch für alle CYD Größen):
// TFT_MISO: 5
// TFT_MOSI: 7
// TFT_SCLK: 6
// TFT_CS:   10
// TFT_DC:   8
// TFT_RST:  9
// TOUCH_INT: 16
```

**Features:**
- ESP32-WROVER mit WiFi + Bluetooth
- ST7789 (2.4") oder ST7796U (2.8"/3.5")
- Capacitive Touch
- USB-C Anschluss
- Günstig (~10-20€)

### 7 Inch ESP32-S3 Serial Screen
Großes Display mit ESP32-S3 Serial Screen Interface.

```cpp
// include/config.h
#define DISPLAY_TYPE CYD_7_INCH

// UART Serial Pinout (nicht SPI!):
// TFT_RX:  16
// TFT_TX:  17
// TFT_BAUD: 115200
// TOUCH_INT: 4
// TOUCH_RX: 5
// TOUCH_TX: 18
```

**Hinweis:** Dieses Display nutzt UART (Serial) statt SPI für die Kommunikation.

**Features:**
- ESP32-S3 mit WiFi + Bluetooth
- 7 Zoll Display (800x480)
- Capacitive Touch
- Serial Screen Interface (UART)
- USB-C Anschluss

### Custom DIY Display
```cpp
// include/config.h
#define DISPLAY_TYPE DIY_ILI9341

// Pins manuell setzen:
#define TFT_MISO  19
#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_CS   5
#define TFT_DC   4
#define TFT_RST  33
#define TOUCH_INT  16
```

---

## 🔌 Board-Konfiguration (PlatformIO)

**LilyGo T-Display S3 Touch:**
```bash
pio run -e esp32-s3-touch
```

**Cheap Yellow Display CYD 2.4":**
```bash
pio run -e cyd-2-4-st7789
```

**Cheap Yellow Display CYD 2.8":**
```bash
pio run -e cyd-2-8-st7796u
```

**Cheap Yellow Display CYD 3.5":**
```bash
pio run -e cyd-3-5-st7796u
```

**7 Inch Serial Screen:**
```bash
pio run -e cyd-7-inch-serial
```

**LilyGo T-Display-S3 Capacitive:**
```bash
pio run -e esp32-s3-cap
```

**Standard ILI9341:**
```bash
pio run -e esp32-s3-ili9341
```

**Generic ESP32-S3:**
```bash
pio run -e esp32-s3-generic
```

---

## 📝 Entwicklung

**Kompilieren ohne Flashen:**
```bash
pio run
```

**Flashen:**
```bash
pio run --target upload
```

**Clean Build:**
```bash
pio run -t clean && pio run
```

**Upload Port spezifizieren:**
```bash
pio run --target upload --upload-port /dev/ttyUSB1
```

---

## 🎨 UI Design

Das Design ist in `docs/UI_DESIGN.md` dokumentiert:

**Features:**
- Spotify Style (Dark Mode)
- Album Cover Display
- Track Info (Title, Artist, Album)
- Touch Controls (Play/Pause/Next/Previous)
- Progress Bar
- Volume Slider
- Portrait + Landscape Mode
- Multi-Resolution Support (240x320 bis 800x480)

**Color Scheme:**
- Background: #121212 (Dunkelgrau)
- Surface: #181818
- Primary: #1DB954 (Spotify Green)
- Text Primary: #FFFFFF
- Text Secondary: #B3B3B3

---

## 🔍 Debugging

**Serial Monitor:**
```bash
pio device monitor
```

**Debug-Modus aktivieren:**
```cpp
// include/config.h
#define DEBUG_MODE true
#define DEBUG_DISPLAY true
#define DEBUG_WIFI false
#define DEBUG_SPOTIFY false
#define DEBUG_TOUCH false
```

---

## ⚠️ Bekannte Issues

### TODO (Phase 1)
- [x] WiFi Connection implementieren
- [x] OAuth2 Flow für Spotify
- [x] Spotify API Integration
- [x] Cover Image Download
- [x] Touch-Handling vollenden
- [x] On-Screen UI

### TODO (Phase 1.5 - UART)
- [x] Serial Display Manager implementiert
- [x] Touch Manager über UART
- [x] Dual Mode (SPI + UART Auto-Detect)
- [x] Spotify UI für 800x480
- [ ] FT6236 Library für UART Touch

### TODO (Phase 2)
- [ ] Playlist Browser
- [ ] Search Funktion
- [ ] Settings Screen
- [ ] Volume Slider (Touch)
- [ ] Progress Bar (Seekable)

---

## 📊 Project Progress

**Gesamt:** ~25% ✅

| Phase | Status | Progress |
|-------|--------|----------|
| Phase 1: Grundgerüst | ✅ Abgeschlossen | 100% |
| Phase 1.5: Serial Display (7 Inch) | ✅ Abgeschlossen | 100% |
| Phase 2: WiFi & Auth | ⏳ Geplant | 0% |
| Phase 3: UI & Controls | 🔄 Teilweise | 40% |
| Phase 4: Spotify API | ⏳ Geplant | 0% |
| Phase 5: Settings | ⏳ Geplant | 0% |
| Phase 6: Features | ⏳ Geplant | 0% |

---

## 📄 Lizenz

MIT License - Für persönlichen Gebrauch.

---

## 🤝 Beiträge

Gerne! Pull Requests sind willkommen.

---

## 📞 Support

Bei Problemen: Issue auf GitHub erstellen oder mich anschreiben.

---

*Letztes Update: 31.01.2026 - 18:30*
