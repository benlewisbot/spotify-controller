# Data Directory - LittleFS Filesystem

## Overview
This directory contains files that will be uploaded to the ESP32's LittleFS partition.

## Files

### config.json
Default configuration file. Will be created on first boot if missing.

Edit this file to pre-configure your device before flashing:

```json
{
  "wifi": {
    "ssid": "YourWiFiName",
    "password": "YourWiFiPassword"
  },
  "spotify": {
    "client_id": "your_spotify_client_id",
    "client_secret": "your_spotify_client_secret"
  }
}
```

## How to Upload to ESP32

Using PlatformIO CLI:
```bash
cd /home/tod/clawd/projects/spotify-controller
pio run --target uploadfs
```

Using VS Code:
1. Press F1 or Ctrl+Shift+P
2. Type "PlatformIO: Upload File System Image"
3. Select the environment (e.g., esp32-wrover)

## Spotify Client ID & Secret

1. Go to https://developer.spotify.com/dashboard
2. Create a new app
3. Copy the Client ID
4. Set Redirect URI: http://YOUR_DEVICE_IP:8080/callback
5. For Client Secret, you'll need to add it via the web interface on first use

## Configuration Structure

- **wifi**: SSID and password for WiFi connection
- **spotify**: Spotify API credentials and tokens
- **display**: Display settings (orientation, brightness, screensaver)
- **volume**: Volume limit setting
- **device**: Unique device ID (auto-generated)
