# Schnellstart-Guide - Spotify Controller ESP32

## 🎯 Schnellstart in 5 Schritten

### 1. Hardware anschließen
```
ESP32 LilyGo T-Display S3 Touch
├── USB-C Kabel an Computer
└── Power ON (automatisch über USB)
```

### 2. Projekt klonen
```bash
cd ~/clawd/projects/spotify-controller
```

### 3. WiFi und Spotify konfigurieren

**Option A: Via Datei (empfohlen)**
```bash
cd data
cp config.example.json config.json
# Editiere config.json mit deinen Daten:
# - wifi.ssid = Dein WiFi Name
# - wifi.password = Dein WiFi Passwort
# - spotify.client_id = Deine Spotify Client ID
```

**Option B: Via Web Interface (nach erstem Flash)**
1. Gerät booten, Auto-AP startet
2. Mit "Spotify-Controller" WiFi verbinden
3. Browser öffnen → http://192.168.4.1
4. WiFi Daten eintragen

### 4. Firmware flashen
```bash
# Installiere PlatformIO (falls nicht vorhanden)
pip install platformio

# Kompilieren und hochladen
cd /home/tod/.openclaw/workspace/projects/spotify-controller
pio run --target upload --environment esp32-wrover

# Optional: Konfiguration hochladen
pio run --target uploadfs --environment esp32-wrover
```

### 5. Spotify App erstellen

1. Gehe zu: https://developer.spotify.com/dashboard
2. "Create App" klicken
3. Daten eintragen:
   - **App name:** "Spotify Controller ESP32"
   - **App description:** "Touch controller for Spotify"
   - **Redirect URI:** `http://DEVICE_IP:8080/callback`
4. **Client ID** kopieren und in `config.json` einfügen

### 6. Erste Authentifizierung

1. Gerät neu starten
2. Nach WiFi-Verbindung auf Display schauen
3. QR Code scannen oder URL eingeben
4. Spotify Login und App autorisieren
5. Fertig! 🎵

## 📋 Konfigurationsdatei

`data/config.json`:
```json
{
  "wifi": {
    "ssid": "DeinWiFiName",
    "password": "DeinWiFiPasswort"
  },
  "spotify": {
    "client_id": "deine_client_id",
    "client_secret": "dein_client_secret",
    "access_token": "",
    "refresh_token": ""
  },
  "display": {
    "orientation": 1,
    "brightness": 75,
    "screensaver": {
      "enabled": true,
      "timeout_minutes": 10
    }
  },
  "volume": {
    "limit": 80
  }
}
```

## 🔧 Serielle Ausgabe debuggen

```bash
# Mit Serial Monitor verbinden
pio device monitor --baud 115200

# Erwartete Ausgabe:
========================================
  🎵 Spotify Controller ESP32
  Version: 1.0.0 (Phase 1 MVP)
========================================

📁 Initializing ConfigManager...
✅ LittleFS mounted
📄 Configuration loaded

✅ Config initialized
✅ Logger initialized
✅ WiFi initialized
📶 WiFi connected!
  IP: 192.168.1.100

✅ Spotify initialized
🔐 AuthManager initialized
```

## 🎵 Spotify nutzen

Wenn alles eingerichtet ist:

- **🎶 Now Playing:** Album Cover, Titel, Künstler
- **▶️ Play/Pause:** Play/Pause Button
- **⏭️ Skip:** Next/Previous Buttons
- **🔊 Volume:** Volume Slider (vertikal rechts)
- **💾 Save:** Track zu "Liked Songs" hinzufügen

## ❌ Troubleshooting

### WiFi verbindet nicht
```
⚠️ WiFi connection failed, will retry...
```
- SSID und Passwort prüfen
- 2.4GHz vs 5GHz (nur 2.4GHz!)
- WLAN-Range prüfen

### Spotify Auth schlägt fehl
```
❌ Spotify authentication error
```
- Redirect URI prüfen (http://IP:8080/callback)
- Client ID korrekt?
- Spotify Premium Account nötig!

### Display bleibt schwarz
- USB-Kabel prüfen
- Serial Monitor für Fehlermeldungen
- Firmware neu flashen

## 📚 Weitere Dokumentation

- [WIFI_AUTH.md](WIFI_AUTH.md) - Detaillierte WiFi & OAuth2 Docs
- [README.md](../README.md) - Projektübersicht
- [HARDWARE.md](HARDWARE.md) - Hardware Details
