# Wokwi Simulation Guide

## 🚀 Schnellstart für Simulation

### 1. Wokwi öffnen
https://wokwi.com/projects/new/esp32

### 2. ESP32 wählen
- Board: ESP32 Dev Module V1

### 3. Code kopieren
- Öffne `src/main.cpp` im Projekt
- Kopiere den kompletten Code
- Paste in Wokwi Editor

### 4. Konfiguration (optional)
- Mock Mode ist bereits aktiviert (`isMockMode = true`)
- Für Live Mode: `isMockMode = false` + Credentials setzen

### 5. Simulation starten
- Klicke auf ▶ (Run)
- Serial Monitor öffnen (rechts unten)

### 6. Beobachten
- Console zeigt: "==== Spotify Controller ESP32 ==="
- Nach 3s: Track Info wird angezeigt
- Mock Track: Linkin Park - Papercut (oder anderer zufälliger)

---

## 📱 Was du siehst

### Serial Monitor:
```
==== Spotify Controller ESP32 ===
Version: 1.0.0 - Prototype
Mode: Wokwi Simulation

🔧 Hardware wird initialisiert...
  - SPI Display
✅ Hardware initialisiert

🎵 Spotify wird initialisiert...
🎵 Spotify Manager initialisiert
  Mode: Mock (Simulation)
✅ Spotify initialisiert

🖥 UI wird initialisiert...
✅ UI initialisiert

✅ Setup abgeschlossen!
🎵 Spotify Controller bereit!

🎵 Now Playing:
  Titel: Papercut
  Artist: Linkin Park
  Album: Hybrid Theory
  Status: Playing
  Fortschritt: 1:30 / 3:05
  Volume: 75%
```

### Virtual Display:
- Album Cover (Platzhalter)
- Track Info (Title, Artist, Album)
- Progress Bar (über Controls)
- Controls (Save +, Previous, Play/Pause, Next)
- Volume Bar (vertikal rechts)

---

## 🎮 Interaktion (Simulation)

### Touch Simulation (Console):
Da Wokwi kein echtes Touch hat, simuliere Buttons per Serial:

```
⏮ Previous Track
▶ Play/Pause toggled
⏭ Next Track
+ Save Track toggled
🔊 Volume: 75%
```

### Manuelle Steuerung:
Du kannst in Wokwi Serial Monitor Befehle eingeben:

- `play` - Play
- `pause` - Pause
- `next` - Next Track
- `prev` - Previous Track
- `vol 75` - Volume auf 75%

---

## 🎵 Mock Tracks (Zufällig)

Der Prototyp wählt zufällig aus 5 Linkin Park Tracks:

1. **Papercut** - Hybrid Theory (3:05)
2. **In The End** - Hybrid Theory (3:36)
3. **Numb** - Meteora (3:03)
4. **One Step Closer** - Hybrid Theory (2:36)
5. **Faint** - Meteora (2:42)

---

## ⚙️ Live Mode (Hardware)

Für echten Spotify API Zugriff:

### 1. Spotify App erstellen
https://developer.spotify.com/dashboard

### 2. Credentials bekommen
- Client ID
- Client Secret

### 3. Access Token bekommen
OAuth2 Flow oder Spotify PKCE (TODO in Phase 2)

### 4. Code anpassen
```cpp
// In src/main.cpp:
bool isMockMode = false; // Auf false setzen!

spotify.setCredentials("CLIENT_ID", "CLIENT_SECRET");
spotify.setTokens("ACCESS_TOKEN", "REFRESH_TOKEN");
```

### 5. Auf Hardware flashen
```bash
cd /home/tod/.openclaw/workspace/projects/spotify-controller
pio run --target upload
```

---

## 🔍 Debugging

### Debug Mode aktivieren:
```cpp
// In include/config.h:
#define DEBUG_SPOTIFY true
#define DEBUG_DISPLAY true
#define DEBUG_TOUCH true
```

### Mehr Informationen:
```
🎵 Now Playing:
  Titel: Papercut
  Artist: Linkin Park
  Album: Hybrid Theory
  Status: Playing
  Fortschritt: 1:30 / 3:05
  Volume: 75%
```

---

## 🐛 Bekannte Issues

### Wokwi Simulation:
- ❌ Echtes Touch funktioniert nicht (normal - Hardware nötig)
- ✅ Serial Output funktioniert
- ✅ Mock API funktioniert
- ✅ UI Rendering funktioniert

### Hardware:
- ⏳ WiFi Connection TODO (Phase 2)
- ⏳ OAuth2 Flow TODO (Phase 2)
- ⏳ Cover Images TODO (Phase 2)

---

## 📞 Probleme?

- Serial Monitor ist leer → Reset Simulation (klicke erneut ▶)
- Keine Track Info → Warte 3s auf ersten Update
- Touch funktioniert nicht → Normal in Wokwi (Hardware nötig)

---

*Letztes Update: 31.01.2026 - 21:00*
