# Spotify Controller - Wokwi Demo

## 🎮 Live Demo

Try the Spotify Controller UI in your browser without hardware!

[![Wokwi Demo](https://img.shields.io/badge/Wokwi-Demo-green?logo=wokwi)](https://wokwi.com/projects/new/esp32)

## 📋 How to Use the Demo

### Quick Start (Online)

1. Visit [wokwi.com/projects/new/esp32](https://wokwi.com/projects/new/esp32)
2. Copy the content from `wokwi/sketch.ino` in this repo
3. Paste it into the Wokwi editor
4. Click the ▶️ Start button
5. Watch the demo run!

### Demo Features

The Wokwi demo simulates the full Spotify Controller UI with:

- ✅ **Now Playing Screen** - Shows current track info
- ✅ **Album Art** - Placeholder with track info
- ✅ **Progress Bar** - Animated, shows playback position
- ✅ **Play/Pause** - Toggle playback state
- ✅ **Next/Previous** - Navigate through demo tracks
- ✅ **Volume Slider** - Visual volume control
- ✅ **Track Cycling** - Auto-advances after each track

### Demo Tracks

The demo includes 5 Linkin Park tracks:
1. Papercut (Hybrid Theory)
2. In The End (Hybrid Theory)
3. Numb (Meteora)
4. One Step Closer (Hybrid Theory)
5. Faint (Meteora)

## 🎛️ Serial Commands

While the demo is running, you can control it via Serial Monitor (115200 baud):

| Command | Description |
|---------|-------------|
| `play` | Toggle play/pause |
| `next` | Skip to next track |
| `prev` | Go to previous track |
| `vol 50` | Set volume (0-100) |
| `help` | Show available commands |

### Example Session

```
> help
=== Spotify Controller Demo Commands ===
  play     - Toggle play/pause
  next     - Next track
  prev     - Previous track
  vol <N>  - Set volume (0-100)
  help     - Show this help

> next
Next track

> vol 80
Volume: 80

> play
Playing
```

## 📦 Hardware vs Demo

| Feature | Wokwi Demo | Real Hardware |
|---------|------------|---------------|
| Display | ST7789 (simulated) | ST7789, ILI9341, ILI9488, etc. |
| Touch | Serial commands only | FT6236, XPT2046 touch controllers |
| WiFi | None (simulated) | Real WiFi connection |
| Spotify API | Mock data only | Real Spotify Web API |
| Album Art | Placeholder | Real cover images from Spotify |
| Play/Pause | Simulation only | Real Spotify playback control |

## 🔧 Adapting for Real Hardware

To use the demo code on real ESP32 hardware:

1. Install the required libraries:
   - `TFT_eSPI` - Display driver
   - `WiFi` - WiFi connection
   - `HTTPClient` - Spotify API calls
   - `ArduinoJson` - JSON parsing

2. Configure your display in `User_Setup.h`

3. Add your WiFi credentials:
   ```cpp
   const char* WIFI_SSID = "YourWiFi";
   const char* WIFI_PASSWORD = "YourPassword";
   ```

4. Add Spotify credentials (get from [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)):
   ```cpp
   const char* SPOTIFY_CLIENT_ID = "your_client_id";
   const char* SPOTIFY_CLIENT_SECRET = "your_client_secret";
   ```

5. Implement OAuth2 flow (see full project code)

## 🎨 UI Layout

```
┌─────────────────────────────────────────┐
│  [Menu]  Volume Slider │     [♥] [⋮]  │
│                                          │
│  ┌─────────┐  Track Title               │
│  │         │  Artist Name               │
│  │  Album  │  Album Name                │
│  │   Art   │                            │
│  │         │                            │
│  │         │                            │
│  └─────────┘                            │
│                                          │
│  0:00 ━━━━━━━━━━━━━━━━━━ 3:05          │
│     Progress Bar                        │
│                                          │
│        <<   [▶]   >>                   │
│     Controls                             │
│                                          │
└─────────────────────────────────────────┘
```

## 📚 Documentation

- [Main README](../README.md) - Full project documentation
- [TODO](../TODO.md) - Development roadmap
- [REQUIREMENTS](../REQUIREMENTS.md) - Feature list

## 🚀 Next Steps

After trying the demo:

1. Get the hardware (ESP32 + Display)
2. Clone the full repository
3. Follow the [Quick Start Guide](../README.md#installation)
4. Set up Spotify credentials
5. Flash to your device

## 🙏 Acknowledgments

- [Wokwi](https://wokwi.com/) - Arduino simulator
- [Spotify Web API](https://developer.spotify.com/documentation/web-api)
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) - Display driver

---

Made with ❤️ for music lovers
