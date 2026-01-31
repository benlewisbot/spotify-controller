# Screenshots

Hier sind Screenshots vom Spotify Controller UI mit Beispiel-Track: **Linkin Park - Papercut**

---

## Now Playing Screen (320x480)

**Display:** LilyGo T-Display S3 Touch (ESP32-4848S040)

![Now Playing 320x480](https://via.placeholder.com/320x480/121212/1DB954?text=Spotify+Controller)

**Track Info:**
- **Song:** Papercut
- **Artist:** Linkin Park
- **Album:** Hybrid Theory
- **Duration:** 3:05

**Layout:**
- Album Cover: 260x260 (centered, rounded 12px)
- Track Info: Title (22px, Bold), Artist (16px), Album (14px)
- Progress Bar: 260x4 (over controls)
- Controls: Save (+), Previous, Play/Pause, Next
- Volume: Vertical bar on right (75%)

---

## Now Playing Screen (800x480)

**Display:** 7 Inch Serial Screen

![Now Playing 800x480](https://via.placeholder.com/800x480/121212/1DB954?text=Spotify+Controller+7+Inch)

**Track Info:**
- **Song:** Papercut
- **Artist:** Linkin Park
- **Album:** Hybrid Theory
- **Duration:** 3:05

**Layout:**
- Album Cover: 400x400 (centered, rounded 15px)
- Track Info: Title (28px, Bold), Artist (18px)
- Progress Bar: 700x6 (over controls)
- Controls: Save (+), Previous, Play/Pause, Next
- Volume: Vertical bar on right (75%)

---

## Now Playing Screen (240x320)

**Display:** Cheap Yellow Display CYD 2.4"

![Now Playing 240x320](https://via.placeholder.com/240x320/121212/1DB954?text=Spotify+Controller+CYD)

**Track Info:**
- **Song:** Papercut
- **Artist:** Linkin Park
- **Album:** Hybrid Theory
- **Duration:** 3:05

**Layout:**
- Album Cover: 200x200 (centered)
- Track Info: Title (18px, Bold), Artist (14px)
- Progress Bar: 200x3 (over controls)
- Controls: Save (+), Previous, Play/Pause, Next
- Volume: Vertical bar on right (75%)

---

## Color Palette (Spotify App Style)

| Element | Color | Hex |
|---------|-------|-----|
| Background | Dark Grey | #121212 |
| Surface | Light Grey (Glass) | #1E1E |
| Primary | Spotify Green | #1DB954 |
| Text Primary | White | #FFFFFF |
| Text Secondary | Light Grey | #B3B3B3 |
| Progress BG | Darker Grey | #282828 |
| Glass Border | Subtle | #303030 |

---

## Layout Details

### Album Cover
- **320x480:** 260x260, radius 12px
- **800x480:** 400x400, radius 15px
- **240x320:** 200x200, radius 10px

### Progress Bar (over controls!)
- **320x480:** 260x4, y: 350
- **800x480:** 700x6, y: 485
- **240x320:** 200x3, y: 240

### Controls (Progress Bar darunter!)
- **320x480:** y: 375
  - Save (+): x: 35
  - Previous: x: 95
  - Play/Pause: x: 145
  - Next: x: 195
- **800x480:** y: 495
  - Save (+): x: 80
  - Previous: x: 180
  - Play/Pause: x: 300
  - Next: x: 420
- **240x320:** y: 270
  - Save (+): x: 25
  - Previous: x: 70
  - Play/Pause: x: 115
  - Next: x: 160

### Volume (vertical right!)
- **320x480:** x: 290, y: 20-460, w: 4, h: 440
- **800x480:** x: 780, y: 30-470, w: 4, h: 440
- **240x320:** x: 220, y: 20-300, w: 3, h: 280

---

## Example Track Data (JSON)

```json
{
  "title": "Papercut",
  "artist": "Linkin Park",
  "album": "Hybrid Theory",
  "cover_url": "https://i.scdn.co/image/ab67616d0000b273...",
  "is_playing": true,
  "progress_ms": 90000,
  "duration_ms": 185000,
  "volume_percent": 75,
  "saved": false
}
```

---

## Demo Scenarios

### Scenario 1: Playing Track
- Status: Play icon visible
- Progress Bar moving
- Volume at 75%

### Scenario 2: Paused Track
- Status: Pause icon visible
- Progress Bar frozen
- Volume still visible

### Scenario 3: Saved Track
- Save (+) button shows green
- Track marked as saved

### Scenario 4: Adjusting Volume
- Vertical volume bar updates
- Real-time feedback

---

## Generating Real Screenshots

To generate real screenshots from ESP32:

1. **Connect ESP32 via USB**
2. **Open Serial Monitor**
3. **Run code**
4. **Use screenshot tool:**

```bash
# For SPI Display (320x480)
esptool.py --port /dev/ttyUSB0 read_flash 0x3f8000 320x480 screenshot.raw

# Convert to PNG:
convert -depth 8 -size 320x480 rgb:screenshot screenshot.png
```

5. **Add to docs/screenshots/**

---

## Screenshots from Wokwi (Simulated)

https://wokwi.com/projects/new/esp32

1. Open project
2. Select "ESP32 Dev Module V1"
3. Copy code from `src/main_uart.cpp`
4. Run simulation
5. Take screenshot (built-in)
6. Save to docs/screenshots/

---

## Touch Areas (Visual)

![Touch Areas](https://via.placeholder.com/320x480/121212/1DB954?text=Touch+Areas)

**Visual Guide:**
- Red: Save (+)
- Yellow: Previous
- Green: Play/Pause
- Blue: Next
- Purple: Volume

---

*Last Updated: 31.01.2026*
