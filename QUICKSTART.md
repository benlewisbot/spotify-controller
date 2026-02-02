# 🎵 Spotify Controller - Quick Start Guide

**Version:** 1.0.0
**Status:** GUI Ready for Testing!

---

## 📋 What's Included

✅ **Now Playing Screen** - Complete UI with Spotify-inspired design
✅ **Playback Controls** - Play/Pause, Next, Previous, Volume
✅ **Progress Bar** - Seek to any position in track
✅ **Volume Slider** - Vertical slider on right side
✅ **Save/Heart Button** - Toggle save with visual feedback
✅ **Spotify Web API** - Get current playing track info
✅ **OAuth2 Authentication** - Secure PKCE flow
✅ **WiFi Support** - Auto-connect and reconnect
✅ **Wokwi Demo** - Try without hardware!

---

## 🚀 Get Started in 3 Minutes

### Option 1: Try Wokwi Demo (No Hardware)

1. **Open Wokwi**
   - Visit: https://wokwi.com/projects/new/esp32

2. **Load Demo**
   - Copy code from `wokwi/sketch.ino`
   - Paste into Wokwi editor
   - Click ▶️ Start

3. **Enjoy!**
   - Watch the UI render
   - Use Serial Monitor to control:
     - `play` - Toggle play/pause
     - `next` - Next track
     - `vol 50` - Set volume

### Option 2: Build & Flash (With Hardware)

1. **Install PlatformIO**
   ```bash
   pip install platformio
   ```

2. **Configure WiFi & Spotify**
   Edit `data/config.json`:
   ```json
   {
     "wifi": {
       "ssid": "YourWiFiSSID",
       "password": "YourWiFiPassword"
     },
     "spotify": {
       "client_id": "your_client_id",
       "client_secret": "your_client_secret"
     }
   }
   ```

3. **Get Spotify Credentials**
   - Visit: https://developer.spotify.com/dashboard
   - Create new app
   - Set redirect URI: `http://<device-ip>:8080/callback`
   - Copy Client ID and Secret to config

4. **Build & Upload**
   ```bash
   cd /home/tod/clawd/projects/spotify-controller

   # Option A: Full upload (recommended)
   ./upload.sh

   # Option B: Step by step
   ./build.sh
   pio run -t upload
   pio run -t uploadfs
   ```

5. **Monitor**
   ```bash
   pio device monitor
   ```

---

## 🎮 Usage

### Initial Setup

1. **Power on device**
2. **Connect to WiFi** (configured in config)
3. **Open browser to device IP**: `http://<device-ip>:8080`
4. **Click "Connect Spotify"** and authorize

### Daily Use

Just wake up and:
1. Check the display - shows current track
2. Touch controls to adjust playback
3. Volume slider on the right
4. Progress bar for seeking

---

## 📱 UI Layout

```
┌─────────────────────────────────────┐
│  [⋮]  Volume Slider │     [♥]      │
│                        Save/Like    │
│  ┌─────────┐                      │
│  │         │  Track Title         │
│  │  Album  │  Artist Name         │
│  │   Art   │  Album Name          │
│  │         │                      │
│  │         │                      │
│  └─────────┘                      │
│                                   │
│  0:00 ━━━━━━━━━━━━━━ 3:05        │
│  Progress Bar (seekable)          │
│                                   │
│     <<   [▶]   >>                │
│  Prev  Play/Pause  Next           │
└─────────────────────────────────────┘
```

---

## 🛠️ Troubleshooting

### WiFi won't connect
- ✅ Check SSID/password in `config.json`
- ✅ Use 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- ✅ Check router security settings

### Display shows nothing
- ✅ Verify wiring matches `User_Setup.h`
- ✅ Check 3.3V power supply
- ✅ Try different display type in `platformio.ini`

### Spotify auth fails
- ✅ Verify Client ID and Secret are correct
- ✅ Check redirect URI matches Spotify Dashboard
- ✅ Make sure Spotify Premium is active

### Touch not working
- ✅ Calibrate touch in settings
- ✅ Verify touch controller is detected (check serial)
- ✅ Check touch pin configuration

---

## 📚 Documentation

- [README.md](../README.md) - Full documentation
- [TODO.md](../TODO.md) - Development roadmap
- [WOKWI.md](../WOKWI.md) - Wokwi demo guide
- [REQUIREMENTS.md](../REQUIREMENTS.md) - Feature list

---

## 🔧 Build Scripts

### Quick Build
```bash
./build.sh                    # Build only
./upload.sh                   # Build + upload
```

### PlatformIO Commands
```bash
pio run                       # Build
pio run -t upload             # Upload firmware
pio run -t uploadfs           # Upload filesystem
pio device monitor            # Serial monitor
pio run -t clean              # Clean build
```

---

## 🎨 Supported Hardware

### Displays
- ILI9341 (240x320)
- ILI9488 (320x480)
- ST7789 (240x320, 135x240)
- ST7796U (320x480)

### Boards
- ESP32-WROVER (4MB+ PSRAM)
- LilyGo T-Display S3 Touch
- Cheap Yellow Display (CYD) series
- Any ESP32 + SPI display combo

### Touch Controllers
- FT6236 (Capacitive)
- XPT2046 (Resistive)

---

## 🚀 Next Steps

1. **Test the GUI** - Try the Wokwi demo first!
2. **Get Hardware** - Order ESP32 + Display kit
3. **Flash Device** - Use the upload script
4. **Enjoy Music** - Control Spotify from your display!

---

**Made with ❤️ for music lovers**

For questions or issues, see the main README or open an issue on GitHub.
