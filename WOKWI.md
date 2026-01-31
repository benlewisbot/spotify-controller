# Spotify Controller - Wokwi Preview

## Web-Preview ohne Flashen
https://wokwi.com/projects/new/esp32

## Wie nutzen:
1. Klicke auf den Link oben
2. Wähle "ESP32 Dev Module V1"
3. Kopiere den Code aus `src/main.cpp`
4. Past ihn in den Editor
5. Start-Symbol klicken → ESP32 simuliert

## Simuliert:
- ✅ ESP32 CPU
- ✅ WiFi (simuliert)
- ✅ Serial Monitor
- ✅ LCD Display (virtuell)
- ❌ Touch (Hardware nötig)

## Für Tests anpassen:
```cpp
// In main.cpp hinzufügen:
#ifdef WOKWI
  Serial.println("⚠️  Wokwi Simulation - Hardware-APIs deaktiviert");
#else
  // Echter ESP32 Code
#endif
```

## Alternative: ESP-IDF Simulator (lokal)
- Installation: `sudo apt install esp-idf`
- LCD-Simulation erfordert zusätzliche Arbeit

## Empfehlung
- **Wokwi:** Einfach, browser-basiert, schnell für UI-Tests
- **Echte Hardware:** Für finalen Test (WiFi, Touch, Audio)

## TODO
- [ ] Code für Wokwi-Vorschau vorbereiten
- [ ] UI-Simulation (LVGL ohne echtes Display)
- [ ] Serial Output für Debugging
