# Spotify Controller - Wokwi Demo

## 🎮 Live Demo

Try the Spotify Controller UI in your browser without hardware!

[![Open in Wokwi](https://img.shields.io/badge/Wokwi-Demo-green?logo=wokwi)](https://wokwi.com/projects/new/esp32)

**Click the link above or visit:** https://wokwi.com/projects/new/esp32

---

## 📋 Quick Start

### Option 1: Copy-Paste (Easiest) ⭐

1. [Open Wokwi ESP32 Editor](https://wokwi.com/projects/new/esp32)
2. Copy the code from `sketch.ino` in this directory
3. Paste it into the Wokwi editor
4. Press **▶️ Start** or click **Run**
5. Watch the demo!

### Option 2: Upload Files

1. [Open Wokwi ESP32 Editor](https://wokwi.com/projects/new/esp32)
2. Click the **+** button in the file list to add files
3. Upload both `diagram.json` and `sketch.ino`
4. Press **▶️ Start**

---

## 🎛️ Features Demonstrated

| Feature | Status | Description |
|---------|--------|-------------|
| **Display** | ✅ | ST7789 (240x320 simulated) |
| **Touch Input** | ✅ | Via Serial Commands + FT6236 config |
| **Now Playing Screen** | ✅ | Full UI with album art, track info |
| **Progress Bar** | ✅ | Animated, seekable via touch |
| **Play/Pause** | ✅ | Toggle playback state |
| **Next/Previous** | ✅ | Navigate through demo tracks |
| **Volume Slider** | ✅ | Vertical slider, touch-enabled |
| **Save/Like Button** | ✅ | Heart icon with toggle |
| **Time Display** | ✅ | Current and total time |
| **Spotify Theme** | ✅ | Dark theme (#121212, #1DB954) |

---

## 💬 Serial Commands

Open the Serial Monitor (115200 baud) to interact with the demo:

### Playback Controls
| Command | Action |
|---------|--------|
| `play`, `pause`, `toggle` | Toggle Play/Pause |
| `next` | Skip to next track |
| `prev`, `previous` | Go to previous track |

### Volume & Seek
| Command | Action |
|---------|--------|
| `vol <0-100>` | Set volume (e.g., `vol 75`) |
| `seek <0-100>` | Seek to percentage (e.g., `seek 50`) |

### Touch Simulation
| Command | Action |
|---------|--------|
| `touch <x,y>` | Simulate touch at coordinates |
| | Example: `touch 160,240` for center |

### Other Commands
| Command | Action |
|---------|--------|
| `save`, `like` | Toggle saved/liked |
| `goto <1-N>` | Jump to specific track |
| `info` | Show current track info |
| `tracks` | List all available tracks |
| `help`, `?` | Show this help |

---

## 🎨 Demo Tracks

The demo includes 6 Linkin Park tracks:

| # | Title | Album | Duration |
|---|-------|-------|----------|
| 1 | Papercut | Hybrid Theory | 3:05 |
| 2 | In The End | Hybrid Theory | 3:36 |
| 3 | Numb | Meteora | 3:03 |
| 4 | One Step Closer | Hybrid Theory | 2:36 |
| 5 | Faint | Meteora | 2:42 |
| 6 | Breaking the Habit | Meteora | 3:16 |

Tracks auto-cycle after each song ends.

---

## 📐 Touch Areas (for simulation)

The display is 240x320 in Wokwi (landscape mode).

### Volume Slider (Right side)
- **X range:** 220-235
- **Y range:** 65-190
- Touch anywhere in this area to adjust volume

### Progress Bar (Bottom)
- **X range:** 16-200
- **Y range:** 270-280
- Touch to seek to position

### Controls (Bottom row)
- **Previous:** X=40, Y=290
- **Play/Pause:** X=110, Y=290
- **Next:** X=180, Y=290

### Save Button (Top right, left of volume)
- **X range:** 185-220
- **Y range:** 16-52

---

## 🔧 Hardware Configuration

The demo simulates the LilyGo T-Display S3 Touch configuration:

### Pinout (ESP32 → Display)

| ESP32 Pin | Display Pin | Function |
|-----------|-------------|----------|
| D23 | CLK | SPI Clock |
| D18 | MOSI | SPI MOSI |
| D19 | MISO | SPI MISO |
| D5 | CS | Chip Select |
| D2 | DC | Data/Command |
| D4 | RST | Reset |
| D15 | BL | Backlight |
| GND | GND | Ground |
| 3V3 | VCC | Power |

### Pinout (ESP32 → Touch - FT6236)

| ESP32 Pin | Touch Pin | Function |
|-----------|-----------|----------|
| D21 | SDA | I2C Data |
| D22 | SCL | I2C Clock |
| 3V3 | VCC | Power |
| GND | GND | Ground |

---

## 🎨 UI Layout

```
┌─────────────────────────────────────┐
│ [≡]              [♡]     [🔊]      │
│                                      │
│  ┌──────────────────────────────┐  │
│  │                              │  │
│  │       Album Art              │  │
│  │       LP-HYB                 │  │
│  │       🎵                     │  │
│  │                              │  │
│  └──────────────────────────────┘  │
│  Papercut                          │
│  Linkin Park                       │
│  Hybrid Theory                     │
│  ● Playing                         │
│                                      │
│ 0:00 ███████████░░░░░░ 3:05        │
│  45%                                │
│                                      │
│        ⏮      [⏸]      ⏭         │
│   ━━━━╱━━━━━━━━━━━━━                │
│         ━━━━                        │
│   (volume slider)                   │
└─────────────────────────────────────┘
```

---

## 📦 Wokwi Limitations

### What Works in Wokwi ✅
- Display rendering (ST7789, ILI9341, etc.)
- ESP32 CPU simulation
- Serial Monitor (for commands)
- Basic SPI simulation
- Touch simulation via serial

### What Doesn't Work in Wokwi ❌
- WiFi connection (simulated only)
- HTTPS/TLS for Spotify API
- Real touch input (use Serial commands instead)
- Real Spotify authentication
- Image download from URLs
- External HTTPS requests
- LVGL library (simulated with TFT_eSPI)

---

## 🚀 From Demo to Real Hardware

### Required Hardware

- **LilyGo T-Display S3 Touch** (ESP32-4848S040C)
- **USB-C cable** for programming

### Display Specifications

| Spec | Value |
|------|-------|
| Resolution | 480x480 (round display) |
| Controller | ST7789 |
| Touch | FT6236 (Capacitive) |
| Interface | SPI + I2C |

### Steps to Flash Real Hardware

1. **Clone the Repository**
   ```bash
   git clone <repository-url>
   cd spotify-controller
   ```

2. **Configure Hardware**
   Edit `include/config.h`:
   ```cpp
   #define DISPLAY_WIDTH 480
   #define DISPLAY_HEIGHT 480
   #define TOUCH_I2C_ADDR 0x38
   ```

3. **Add WiFi & Spotify Credentials**
   Edit `data/config.json`:
   ```json
   {
     "wifi": {
       "ssid": "YourWiFiSSID",
       "password": "YourWiFiPassword"
     },
     "spotify": {
       "client_id": "your_spotify_client_id",
       "client_secret": "your_spotify_client_secret"
     }
   }
   ```

4. **Build and Upload**
   ```bash
   # Using PlatformIO
   pio run --target upload
   pio run --target uploadfs

   # Or using Arduino IDE
   - Select board: ESP32-S3
   - Set correct partition scheme
   - Upload sketch
   - Upload SPIFFS/LittleFS data
   ```

5. **Monitor Output**
   ```bash
   pio device monitor
   ```

### Getting Spotify Credentials

1. Visit [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
2. Log in with your Spotify account
3. Click **"Create App"**
4. Fill in:
   - App name: `Spotify Controller`
   - App description: `ESP32 Spotify Controller`
   - Redirect URI: `http://<device-ip>:8080/callback`
5. Copy **Client ID** and **Client Secret** to `config.json`

---

## 📖 Complete Feature List (Real Hardware)

### Phase 1: WiFi & Auth ✅
- ✅ WiFi Connection with auto-reconnect
- ✅ LittleFS for configuration storage
- ✅ Spotify OAuth2 Flow (Authorization Code with PKCE)
- ✅ Hotspot Mode for first-time setup
- ✅ Token refresh handling

### Phase 2: Spotify API ✅
- ✅ Currently Playing endpoint
- ✅ Player Controls (Play, Pause, Next, Previous)
- ✅ Volume Control
- ✅ Seek (position in track)
- ✅ Progress Tracking
- ✅ Track Info extraction (title, artist, album, cover)
- ✅ Save/Like Track functionality

### Phase 3: GUI ✅
- ✅ Spotify Dark Theme (#121212 background, #1DB954 primary)
- ✅ Now Playing Screen
- ✅ Album Art placeholder
- ✅ Track Info (Title, Artist, Album)
- ✅ Progress Bar with seek
- ✅ All Controls (Play/Pause, Next/Prev, Volume, Save)

### Phase 4: Touch ✅
- ✅ FT6236 Touch Controller support
- ✅ Touch events for all buttons
- ✅ Volume slider (vertical)
- ✅ Progress bar seek

### Phase 5: Wokwi Demo ✅
- ✅ Full Wokwi configuration
- ✅ Online demo ready
- ✅ Test instructions

---

## 🎯 Usage Examples

### Example 1: Control Playback

```bash
> play
▶ Playing

> pause
⏸ Paused

> toggle
▶ Playing
```

### Example 2: Navigate Tracks

```bash
> next
⏭ Next track: In The End

> prev
⏮ Previous track: Papercut

> goto 3
Track 3: Numb
```

### Example 3: Volume Control

```bash
> vol 50
Volume: 50%

> vol 100
Volume: 100%
```

### Example 4: Seek

```bash
> seek 50
Seek to: 1:32 (50%)

> seek 0
Seek to: 0:00 (0%)
```

### Example 5: Touch Simulation

```bash
> touch 160,290
Touch: (160, 290)
▶ Playing

> touch 240,150
Touch: (240, 150)
Volume: 75%
```

---

## 🐛 Troubleshooting

### Wokwi Issues

| Problem | Solution |
|---------|----------|
| Display shows nothing | Check diagram.json connections |
| Serial not responding | Set baud rate to 115200 |
| Code won't compile | Check for library dependencies (TFT_eSPI) |
| Touch not working | Use Serial commands instead |

### Hardware Issues

| Problem | Solution |
|---------|----------|
| Display blank | Verify wiring and power (3.3V) |
| WiFi won't connect | Check SSID/password, use 2.4GHz only |
| Spotify auth fails | Verify Client ID, Secret, and redirect URI |
| Touch not working | Check I2C connections (SDA/SCL) |

---

## 📚 Additional Documentation

- [Main README](../README.md) - Full project documentation
- [TODO](../TODO.md) - Development roadmap
- [REQUIREMENTS](../REQUIREMENTS.md) - Feature requirements
- [WOKWI.md](../WOKWI.md) - Wokwi integration guide
- [TROUBLESHOOTING](../docs/TROUBLESHOOTING.md) - Troubleshooting guide

---

## 🙏 Resources & Credits

- [Wokwi Documentation](https://docs.wokwi.com/) - Arduino simulator
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI) - Display driver
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [Spotify Web API](https://developer.spotify.com/documentation/web-api)
- [LilyGo T-Display S3](https://www.lilygo.cc/products/t-display-s3)

---

## 🎉 Ready for the Real Thing?

**Get the hardware and start building!**

1. Order LilyGo T-Display S3 Touch
2. Clone the repository
3. Follow the configuration steps above
4. Flash and enjoy your Spotify Controller!

---

Made with ❤️ for music lovers
