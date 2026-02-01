# 🎵 Spotify Controller ESP32

A modern, touch-enabled Spotify controller built with ESP32 and LVGL. Features a Spotify-inspired UI with Apple Liquid Glass effects.

## ✨ Features

### Phase 1 (MVP) - Current Version
- ✅ WiFi connectivity with auto-reconnect
- ✅ Spotify OAuth 2.0 authentication with PKCE
- ✅ Playback control (play/pause, next, prev, volume)
- ✅ Beautiful UI inspired by Spotify + Apple Liquid Glass
- ✅ Modular display interface (supports multiple displays)
- ✅ Touch input with visual feedback
- ✅ Now Playing screen with album art, title, artist
- ✅ Real-time playback updates

### Planned Features
- [ ] Playlist browser
- [ ] Progress bar scrubbing
- [ ] Settings screen
- [ ] Screensaver
- [ ] Search functionality
- [ ] Device selection

## 🖥️ Supported Displays

- ILI9341 (240x320) - Common resistive touch displays
- ILI9488 (320x480) - Larger displays
- ST7789 (240x240, 135x240) - LilyGo T-Display S3
- ST7796U (320x480) - Cheap Yellow Display

## 🔧 Hardware Requirements

### Components
- ESP32-WROVER with PSRAM (4MB+)
- SPI Touch Display (see supported list above)
- USB-C power supply
- (Optional) 3D printed case

### Recommended Boards
- LilyGo T-Display S3 Touch (ESP32-4848S040)
- Generic ESP32-WROVER + ILI9341 display
- Cheap Yellow Display (CYD) series

## 📦 Installation

### Prerequisites
- [PlatformIO](https://platformio.org/)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [Spotify Premium account](https://www.spotify.com/premium)
- WiFi network

### Setup Steps

1. **Clone the repository**
```bash
git clone https://github.com/BenLewisbot/spotify-controller-esp32.git
cd spotify-controller-esp32
```

2. **Install dependencies**
```bash
pio install
```

3. **Configure Spotify credentials**
- Register your app at [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
- Create a new application
- Set redirect URI to: `http://<device-ip>:8080/callback`
- Copy Client ID and Client Secret

4. **Edit configuration**
```cpp
// src/config/Config.hpp
#define SPOTIFY_CLIENT_ID "your-client-id"
#define SPOTIFY_CLIENT_SECRET "your-client-secret"
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"
```

5. **Build and upload**
```bash
pio run --target upload
```

6. **Monitor serial output**
```bash
pio device monitor
```

## 🚀 Initial Setup

1. Power on the device
2. Connect to WiFi (configured in setup)
3. The device will start an auth server
4. Open browser to: `http://<device-ip>:8080`
5. Click "Connect Spotify" and authorize
6. Device will automatically connect and start showing now playing info

## 📱 Usage

### Playback Controls
- **Previous**: Skip to previous track
- **Play/Pause**: Toggle playback
- **Next**: Skip to next track
- **Volume**: Adjust volume (slider or buttons)

### Navigation
- Swipe left/right to navigate between screens
- Tap menu icon (⋮) for additional options

## 🎨 UI Design

The UI is inspired by:
- **Spotify App** - Dark theme, green accent color, album-focused design
- **Apple Liquid Glass** - Translucency, blur effects, smooth animations

### Color Palette
```
Background:    #121212 (Dark gray)
Surface:       #282828 (Elevated surface)
Primary:       #1DB954 (Spotify green)
Text:          #FFFFFF (White)
Text Secondary:#B3B3B3 (Gray)
```

## 🏗️ Architecture

```
src/
├── app/                    # Application controller
│   ├── App.hpp/cpp         # Main app class
│   ├── State.hpp           # App state management
│   └── EventBus.hpp        # Event system
├── config/                 # Configuration
│   ├── Config.hpp/cpp      # Config manager
│   └── lv_conf.h           # LVGL configuration
├── display/                # Display subsystem
│   ├── DisplayManager.hpp/cpp
│   ├── Display.hpp         # Display interface
│   ├── themes/             # UI themes
│   │   └── SpotifyTheme.hpp
│   └── drivers/            # Display drivers
│       ├── ILI9341Display.hpp
│       ├── ILI9488Display.hpp
│       ├── ST7789Display.hpp
│       ├── ST7796UDisplay.hpp
│       ├── FT6236Touch.hpp
│       └── XPT2046Touch.hpp
├── spotify/                # Spotify API
│   ├── SpotifyClient.hpp/cpp
│   ├── AuthManager.hpp/cpp
│   └── PlaybackController.hpp
├── ui/                     # UI components
│   ├── WindowManager.hpp/cpp
│   └── screens/
│       ├── NowPlaying.hpp/cpp
│       ├── Auth.hpp/cpp
│       └── Settings.hpp/cpp
├── network/                # Network
│   └── WiFiManager.hpp/cpp
└── utils/                  # Utilities
    ├── Logger.hpp/cpp
    └── Timer.hpp/cpp
```

## 🔌 API Endpoints Used

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

## 📝 Configuration

### Display Type
Edit `src/config/Config.hpp` to select your display:
```cpp
#define DISPLAY_TYPE ILI9341  // Options: ILI9341, ILI9488, ST7789, ST7796U
```

### WiFi
Configure in `src/config/Config.hpp` or via web UI:
```cpp
#define WIFI_SSID "YourNetwork"
#define WIFI_PASSWORD "YourPassword"
```

### Spotify
Set your credentials in `src/config/Config.hpp`:
```cpp
#define SPOTIFY_CLIENT_ID "your-client-id"
#define SPOTIFY_CLIENT_SECRET "your-client-secret"
```

## 🐛 Troubleshooting

### WiFi won't connect
- Check SSID and password in config
- Verify 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Check router settings

### Display shows nothing
- Verify wiring matches User_Setup.h
- Check 3.3V power supply is adequate
- Try different display type in config

### Authentication fails
- Verify Client ID and Secret are correct
- Check redirect URI matches in Spotify Dashboard
- Make sure Spotify Premium is active

### Touch not working
- Calibrate touch in settings
- Verify touch controller is detected on boot
- Check touch pin configuration

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development
```bash
# Build
pio run

# Upload
pio run --target upload

# Monitor
pio device monitor

# Clean
pio run --target clean
```

## 📄 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [Spotify Web API](https://developer.spotify.com/documentation/web-api)
- [LVGL](https://lvgl.io/) - Light and Versatile Graphics Library
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) - Display driver library
- [ArduinoJson](https://arduinojson.org/) - JSON parsing library

## 📞 Support

- Create an [Issue](https://github.com/BenLewisbot/spotify-controller-esp32/issues) for bugs
- Start a [Discussion](https://github.com/BenLewisbot/spotify-controller-esp32/discussions) for questions

---

Made with ❤️ for music lovers
