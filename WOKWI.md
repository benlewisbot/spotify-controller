# Spotify Controller - Wokwi Preview

## 🎮 Live Wokwi Demo

Try the Spotify Controller UI in your browser right now!

[![Open in Wokwi](https://wokwi.com/projects/new/esp32.png)](https://wokwi.com/projects/new/esp32)

**Click the image above or visit:** https://wokwi.com/projects/new/esp32

---

## 📋 Quick Start

### Option 1: Copy-Paste (Easiest)

1. [Open Wokwi ESP32 Editor](https://wokwi.com/projects/new/esp32)
2. Copy the code from `wokwi/sketch.ino` in this repository
3. Paste it into the Wokwi editor
4. Press **▶️ Start** or click **Run**
5. Watch the demo!

### Option 2: Upload Files

1. [Open Wokwi ESP32 Editor](https://wokwi.com/projects/new/esp32)
2. Click the **+** button to add files
3. Upload `wokwi/diagram.json` and `wokwi/sketch.ino`
4. Press **▶️ Start**

---

## 🎛️ What's Simulated

The Wokwi demo shows the full Spotify Controller UI without needing hardware:

| Feature | Status |
|---------|--------|
| **Display** | ST7789 (240x320) simulated |
| **Touch Input** | Via Serial Commands |
| **WiFi** | None (mock data) |
| **Spotify API** | Mock data only |
| **Progress Bar** | ✅ Animated |
| **Volume Slider** | ✅ Visual only |
| **Play/Pause** | ✅ Via Serial |
| **Next/Prev** | ✅ Via Serial |

---

## 💬 Serial Commands

Open the Serial Monitor (115200 baud) to interact with the demo:

| Command | Action |
|---------|--------|
| `play` | Toggle Play/Pause |
| `next` | Skip to next track |
| `prev` | Go to previous track |
| `vol 50` | Set volume (0-100) |
| `help` | Show help |

### Example

```
> play
Playing

> next
Next track

> vol 80
Volume: 80
```

---

## 🎨 Demo Tracks

The demo includes 5 tracks from Linkin Park:

1. **Papercut** - Hybrid Theory (3:05)
2. **In The End** - Hybrid Theory (3:36)
3. **Numb** - Meteora (3:03)
4. **One Step Closer** - Hybrid Theory (2:36)
5. **Faint** - Meteora (2:42)

Tracks auto-cycle after each song ends.

---

## 🔧 Hardware Configuration

The demo uses the following pinout (compatible with LilyGo T-Display S3):

| ESP32 Pin | Display Pin | Function |
|-----------|-------------|----------|
| D23 | CLK | SPI Clock |
| D18 | MOSI | SPI MOSI |
| D19 | MISO | SPI MISO |
| D5 | CS | Chip Select |
| D2 | DC | Data/Command |
| D4 | RST | Reset |
| D15 | BL | Backlight |

---

## 📦 Wokwi Limitations

### What Works in Wokwi

✅ Display rendering (ST7789, ILI9341, etc.)
✅ ESP32 CPU simulation
✅ Serial Monitor (for commands)
✅ Basic SPI simulation

### What Doesn't Work in Wokwi

❌ WiFi connection (simulated only)
❌ HTTPS/TLS for Spotify API
❌ Touch input (use Serial commands instead)
❌ Real Spotify authentication
❌ Image download from URLs
❌ External HTTPS requests

---

## 🚀 From Demo to Real Hardware

### Required Hardware

- ESP32-WROVER with PSRAM (4MB+)
- SPI Display (ST7789, ILI9341, ILI9488, etc.)
- Touch controller (FT6236 or XPT2046)
- USB-C cable for programming

### Steps to Flash Real Hardware

1. **Install PlatformIO**
   ```bash
   pip install platformio
   ```

2. **Clone the Repository**
   ```bash
   git clone https://github.com/BenLewisbot/spotify-controller-esp32.git
   cd spotify-controller-esp32
   ```

3. **Configure Display**
   Edit `include/config.h` to select your display type:
   ```cpp
   #define DISPLAY_TYPE ST7789_S3  // For LilyGo T-Display S3
   // Or: ILI9488_S3, ILI9341_S3, ST7796U_S3, etc.
   ```

4. **Add WiFi & Spotify Credentials**
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

5. **Build and Upload**
   ```bash
   pio run --target upload
   pio run --target uploadfs  # Upload config
   ```

6. **Monitor Output**
   ```bash
   pio device monitor
   ```

### Getting Spotify Credentials

1. Visit [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
2. Log in with your Spotify account
3. Click "Create App"
4. Fill in:
   - App name: "Spotify Controller"
   - App description: "ESP32 Spotify Controller"
   - Redirect URI: `http://<device-ip>:8080/callback`
5. Copy **Client ID** and **Client Secret** to `config.json`

---

## 📖 More Documentation

- [wokwi/README.md](wokwi/README.md) - Detailed Wokwi demo guide
- [README.md](README.md) - Full project documentation
- [TODO.md](TODO.md) - Development roadmap
- [REQUIREMENTS.md](REQUIREMENTS.md) - Feature requirements

---

## 💡 Tips

### Wokwi Tips

- Use Serial Monitor for touch simulation
- Press **Ctrl+Shift+P** and type "Serial" to open
- Run at maximum speed for smooth animation

### Hardware Tips

- Check `User_Setup.h` for display pin configuration
- Use 3.3V logic level for displays
- Ensure adequate power supply (500mA+ recommended)
- Calibrate touch controller in settings

---

## 🐛 Troubleshooting

### Wokwi Issues

- **Display shows nothing**: Check diagram.json connections
- **Serial not responding**: Set baud rate to 115200
- **Code won't compile**: Check for library dependencies

### Hardware Issues

- **Display blank**: Verify wiring and power
- **WiFi won't connect**: Check SSID/password, use 2.4GHz
- **Spotify auth fails**: Verify Client ID, Secret, and redirect URI
- **Touch not working**: Check touch controller detection

---

## 🙏 Resources

- [Wokwi Documentation](https://docs.wokwi.com/)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [Spotify Web API](https://developer.spotify.com/documentation/web-api)

---

**Ready to test the full experience?** [Get the hardware](../README.md#hardware-requirements) and start building!
