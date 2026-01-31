# Fragen für Ben - Spotify Controller

Bitte diese Fragen morgen durchgehen und mir Antworten geben (per WhatsApp):

---

## Hardware
1. Welches ESP32 Board bevorzugst du?
   - [x Es soll mit allem funktionieren aber möglichst flxibel sein. Vielleicht is eink compatiblität eine gute idee] DIY (ESP32-WROVER + Display, ~25-35€)
   - [ ] LilyGo T-Display S3 (alles in einem, ~35-50€)
   - [ ] M5Stack Core2 (Premium, alles integriert, ~60-80€)

2. Welches Display?
   - [ ] 2.4" (240x320) - kleiner, günstiger
   - [ ] 3.5" (480x320) - besser, teurer
   - [x ] Hab ich schon:  __recherchiere gängige größen und mach configs für verschiedene displays es soll einfahc austasuchbar sein__ (welches?)

3. Audio Output nötig?
   - [ ] Ja, Lautsprecher am Controller
   - [x über die spotify remote funktion und api ] Nein, nur Remote Control (Wiedergabe auf anderen Geräten)

4. Einsatzort?
   - [x aber berechne gern mal die batterielaufzeiten. ] Desktop (USB-Strom, kein Akku)
   - [ ] Mobil (Battery, transportabel)

---

## Software Features

5. Playlist Management?
   - [x] Nur lesen & play
   - [soll später folgen] Edit (add tracks, create)

6. Search Funktion?
   - [soll in späterer version folgen ] Vollständige Search Bar mit Tastatur
   - [ ] Kategorien (Top Hits, Playlists, Artists)

7. Offline Modus?
   - [ ] Ja, Playlists cache für Spotify Free
   - [x ] Nein, nur Premium API nutzen

8. Multi-Device?
   - [ ] Ja, mehrere Controller parallel
   - [vorerst nur ein controller. sollte doch immer gehen einfahc weitere identische im netzwerkk zu haben ] Nein, nur ein Controller

---

## UX/UI

9. Display Orientation?
   - [ x] Portrait (hochkant)
   - [x verschiedene seitenverhältnisse sollten gehen ] Landscape (quer)

10. Theme?
    - [x ähnlich zur spoitfy app, modern und schick ] Dark Mode only (Spotify Style)
    - [ ] Dark + Light switch

11. Screensaver?
    - [ als option einbauen ] Ja, dimmen nach X Minuten
    - [ ] Nein, immer an

12. Haptic Feedback?
    - [ ] Ja, Vibration motor
    - [x ] Nein, nicht nötig

---

## Spotify

13. Account Typ?
    - [ x] Premium (für Playback Control)
    - [ ] Free (nur Read-Only)

14. Device Typ?
    - [ ] Eigenes Spotify Device
    - [ x] Nur Remote für bestehende Geräte

15. Volume?
    - [ verstehe die frage nicht, es soll die lautstärke auf dem abspielenden gerät anpassen können, so wie wenn ich mit dem handy in der spotify appp meine sonos lautsprecher auf denen die weidergabe läuft leiser machen kann] System Volume
    - [ ] Spotify Volume

---

## Budget & Zeit

16. Budget für Hardware?
    - [x möglcihst günstige lösung mit option mehr geld auszugeben für bessere komponenten] ~25-35€ (DIY Budget)
    - [ ] ~35-50€ (Semi-Integrated)
    - [ ] ~60-80€ (All-in-One Premium)

17. Zeitrahmen für MVP?
    - [ ] 1 Woche (schnell testen)
    - [x ] 2 Wochen (voll durchdacht)
    - [ x] 3+ Wochen (mit extra Features)

18. Feature Priority (1=wichtig, 3=nice-to-have)?
    - Play/Pause/Next/Previous: [1]
    - Playlist Browser: [2/]
    - Search: [2]
    - Volume Control: [1]
    - Progress Bar/Seek: [2]
    - Cover Display: [1]
    - Offline Cache: [nicht nötig]
    - Settings Screen: [2]
19. schau dir bestehende projhekte auf github zur inspriration an und frage mich notfalls nochmal
---

*Bitte die Checkboxen ausfüllen oder mir Antworten per WhatsApp geben!*
