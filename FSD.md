# Spotify Controller - Functional Specification Document (FSD)

## 1. Project Overview

### 1.1 Purpose
A hardware Spotify controller device with a 480x480 touchscreen display that allows users to control playback, browse playlists, and manage their Spotify account without using the phone app.

### 1.2 Target Hardware
- **Board:** Guition ESP32-S3-4848S040
- **MCU:** ESP32-S3 (240MHz, 320KB RAM)
- **Flash:** 16MB
- **PSRAM:** 8MB Octal
- **Display:** 480x480 ST7701S RGB Parallel
- **Touch:** GT911 Capacitive (I2C)

### 1.3 Build Status
✅ **BUILDING** - Minimal version compiles successfully

| Component | Status | Notes |
|-----------|--------|-------|
| Core System | ✅ Working | WiFi, Config, Display initialization |
| Display Driver | ✅ Working | ST7701S RGB parallel via LovyanGFX |
| Touch Driver | ✅ Configured | GT911 via I2C |
| LVGL UI | ✅ Working | Basic placeholder screen |
| WiFi Manager | ✅ Working | Connection and reconnection |
| Config Manager | ✅ Working | LittleFS storage |
| Spotify Client | ⚠️ Partial | Core functions work, needs testing |
| Auth Manager | ⚠️ Partial | PKCE implemented, needs testing |
| UI Screens | 🔴 Disabled | Temporarily disabled for minimal build |

---

## 2. System Architecture

### 2.1 Component Overview

| Component | File(s) | Status |
|-----------|---------|--------|
| App Controller | `src/app/App.cpp` | ✅ Working |
| Display Manager | `src/display/DisplayManager.cpp` | ✅ Working |
| WiFi Manager | `src/network/WiFiManager.cpp` | ✅ Working |
| Config Manager | `src/config/Config.cpp` | ✅ Working |
| Spotify Client | `src/spotify/SpotifyClient.cpp` | ⚠️ Partial |
| Auth Manager | `src/spotify/AuthManager.cpp` | ⚠️ Partial |
| Window Manager | `src/ui/WindowManager.cpp` | ✅ Minimal |
| Theme | `src/display/themes/SpotifyTheme.cpp` | ✅ Working |
| Cover Manager | `src/cover_manager.cpp` | ✅ Working |

### 2.2 Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| LVGL | 9.4.0 | Graphics/UI framework |
| ArduinoJson | 6.21.5 | JSON parsing |
| LovyanGFX | 1.2.19 | Display driver (RGB parallel) |
| TFT_eSPI | 2.5.43 | SPI display fallback |
| ESPAsyncWebServer | 3.4.0 | OAuth captive portal |

---

## 3. Build Instructions

### 3.1 Prerequisites
```bash
pip install platformio
```

### 3.2 First-Time Setup
After cloning, remove ARM-specific assembly files (LVGL 9 bug):
```bash
rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/convert/helium
rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/sw/blend/helium
```

### 3.3 Build Command
```bash
pio run -e esp32-s3-4848s040
```

### 3.4 Upload Command
```bash
pio run -e esp32-s3-4848s040 --target upload
```

### 3.5 Monitor
```bash
pio device monitor -b 115200
```

---

## 4. Flashing Instructions

### 4.1 Enter Download Mode
1. Connect ESP32-S3-4848S040 via USB
2. Hold **BOOT** button
3. Press and release **RESET** button
4. Release **BOOT** button
5. Run upload command

### 4.2 Upload
```bash
pio run -e esp32-s3-4848s040 --target upload
```

### 4.3 After Flashing
Press **RESET** button to start the application

---

## 5. Hardware Pin Configuration

### 5.1 Display (ST7701S RGB Parallel)
```
Data Pins: 0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 20, 46
Control:   DE=18, HSYNC=16, VSYNC=17, PCLK=21
Backlight: GPIO 38 (PWM)
```

### 5.2 Touch (GT911 I2C)
```
SDA: GPIO 19
SCL: GPIO 45
Address: 0x5D
```

---

## 6. Disabled Features (Minimal Build)

The following files are temporarily disabled (renamed to .bak):
- `src/ui/screens/NowPlaying.cpp` - Main playback screen
- `src/ui/screens/Auth.cpp` - OAuth authentication screen
- `src/ui/screens/Settings.cpp` - Settings menu
- `src/ui/screens/WiFiSettingsScreen.cpp` - WiFi configuration
- `src/ui/screens/SpotifyTokenScreen.cpp` - Token management
- `src/ui/screens/DisplaySettingsScreen.cpp` - Display settings
- `src/ui/screens/AboutScreen.cpp` - About screen
- `src/spotify_gui.cpp` - Legacy GUI code
- `src/main_uart.cpp` - UART-based control
- `src/playback_controller.cpp` - Playback controller
- `src/spotify_manager.cpp` - Legacy Spotify manager

---

## 7. Known Issues

### 7.1 LVGL 9 Compatibility
- ARM Helium assembly must be manually removed
- Some LVGL 8 API calls need updating in screen files
- Custom symbols may not exist in LVGL 9

### 7.2 To Re-enable Screens
1. Rename .bak files back to .cpp
2. Update LVGL API calls for v9 compatibility
3. Fix namespace prefixes (use `ui::` prefix)
4. Rebuild

---

## 8. Changelog

### 2026-02-16 (Final Build Success)
- ✅ Achieved successful minimal build
- Removed ARM Helium assembly from LVGL
- Disabled problematic UI screens temporarily
- Created minimal WindowManager with placeholder screen
- Fixed all singleton destructor visibility issues
- Fixed ArduinoJson API compatibility
- Implemented inline Base64 encoding/decoding
- Implemented mbedtls SHA256 for PKCE

### 2026-02-16 (Session 1)
- Upgraded LVGL from 8.x to 9.4.0
- Created ST7701S display driver for Guition board
- Fixed WiFiManager type issues
- Fixed ArduinoJson template issues
- Added missing includes (vector, map, functional)
- Configured fonts in lv_conf.h
- Created ST7701S RGB parallel display driver

---

## 9. Future Work

### 9.1 Required for Full Functionality
1. Re-enable and fix UI screens for LVGL 9
2. Test Spotify OAuth flow
3. Implement album art display
4. Add playlist browsing
5. Test touch calibration

### 9.2 Known LVGL 9 API Changes
- `lv_spinner_create` → `lv_arc_create`
- `lv_switch_create` → different API
- `lv_obj_set_style_pad_all(obj, 0)` → `lv_obj_set_style_pad_all(obj, 0, 0)`
- Various symbol constants removed

---

*Document last updated: 2026-02-16*
*Build Status: ✅ SUCCESS*
