# Fragen für Ben - Spotify Controller

Bitte diese Fragen morgen durchgehen und mir Antworten geben (per WhatsApp):

---

## Hardware
1. Welches ESP32 Board bevorzugst du?
   - [ x] Es soll mit allem funktionieren aber möglichst flxibel sein. Vielleicht ist ein ESP32-WROVER + Display, ~25-35€
   - [ ] LilyGo T-Display S3 (alles in einem, ~35-50€)
   - [ ] M5Stack Core2 (Premium, alles integriert, ~60-80€)

2. Welches Display?
   - [ ] 2.4" (240x320) - kleiner, günstiger
   - [ ] 3.5" (480x320) - besser, teurer
   - [x ] Flexibble auflösungen und verschiedene modelle
   - [ ] Hab ich schon: __recerchiere gängige größen und mach configs für verschiedene displays es soll einfach austauschbar sein__ (welches?)


3. Audio Output nötig?
   - [ ] Ja, Lautsprecher am Controller
   - [x ] Nein, kein Lautsprecher (nur Remote Control)

4. Einsatzort?
   - [x ] Desktop (USB-Strom, kein Akku)
   - [ ] Mobil (Battery, transportabel)

---

## Software Features

5. Playlist Management?
   - [ ] Nur lesen & play
   - [ x] Soll später folgen: Edit (add tracks, create)

6. Search Funktion?
   - [x ] Soll in späterer Version folgen: Vollständige Search Bar mit Tastatur
   - [ ] Kategorien (Top Hits, Playlists, Artists)

7. Offline Modus?
   - [ ] Ja, Playlists cache für Spotify Free
   - [ x] Nein, nur Premium API nutzen

8. Multi-Device?
   - [ x] Ja, mehrere Controller parallel
   - [ ] Vorerst nur ein Controller sollte doch immer gehen
   - [ ] Nein, immer ein Controller - mehrere weitere identische im Netzwerk zu haben

---

## UX/UI

9. Display Orientation?
   - [ ] Portrait (hochkant)
   - [ ] Landscape (quer)
   - [ x] Verschiedene Seitenverhältnisse sollten gehen

10. Theme?
    - [x ] Ähnlich zur Spotify App, modern und schick
    - [ ] Dark Mode only (Spotify Style)
    - [ ] Dark + Light switch

11. Screensaver?
    - [ x] Als Option einbauen
    - [ ] Ja, dimmen nach X Minuten
    - [ ] Nein, immer an

12. Haptic Feedback?
    - [ ] Ja, Vibration motor
    - [x ] Nein, nicht nötig

---

## Spotify

13. Account Typ?
    - [x ] Premium (für Playback Control)
    - [ ] Free (nur Read-Only)

14. Device Typ?
    - [ ] Eigenes Spotify Device
    - [x ] Nur Remote für bestehende Geräte

15. Volume?
    - [ x] Spotify Volume
    - [ ] System Volume (Controller passt Lautstärke an)

---

## Budget & Zeit

16. Budget für Hardware?
    - [x ] ~25-35€ (DIY Budget)
    - [ ] ~35-50€ (Semi-Integrated)
    - [ ] ~60-80€ (All-in-One Premium)

17. Zeitrahmen für MVP?
    - [ x] 1 Woche (schnell testen)
    - [ ] 2 Wochen (voll durchdacht)
    - [ ] 3+ Wochen (mit extra Features)

18. Feature Priority (wichtig, nice-to-have)?
    - [1 ] Play/Pause/Next/Previous: [1]
    - [ 2] Playlist Browser: [2]
    - [2 ] Search: [2]
    - [ 1] Volume Control: [1]
    - [ 2] Progress Bar/Seek: [2]
    - [ 1] Cover Display: [1]
    - [ 3] Offline Cache: [ ]
    - [ 2] Settings Screen: [2]

---

## Setup & Development

19. Projektstruktur
    - [ x] Ordner: src, include, platformio, docs
    - [x ] Main-File: main.cpp (ESP32 Firmware)
    - [x ] Config: platformio.ini (Home Assistant Konfiguration)
    - [ x] Dokumentation: docs/ (README, Architecture)

20. Tooling
    - [x ] PlatformIO (ESPHome, MQTT, Web Server)
    - [ x] VS Code / PlatformIO Extension
    - [x ] Debugging über Serial Monitor

---

*Bitte die Checkboxen ausfüllen oder mir Antworten per WhatsApp geben!*
