// ===== UI DESIGN - SPOTIFY CONTROLLER (AKTUALISIERT) =====
// Spotify App Style mit minimalen Apple Liquid Glass Einflüssen

/*
╔══════════════════════════════════════╗
║                                      ║
║      [ ALBUM COVER ]                 ║
║      320x320 (quadratisch)            ║
║      mit rounded corners             ║
║                                      ║
╠══════════════════════════════════════╣
║  🎵 Song Title                      ║
║     Artist Name                      ║
║     Album Name                       ║
╠══════════════════════════════════════╣
║  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━    ║
║  1:23 / 3:45                         ║
╠══════════════════════════════════════╣
║  ◀     ⏸/▶     ▶                     ║
║ Previous Play Next                    ║
╚══════════════════════════════════════╝
          │
          │ Volume vertikal
          ↓


COLOR SCHEME (Spotify App Style):
- Background: #121212 (Dunkelgrau)
- Surface: #1E1E1E (leicht heller für Liquid Glass)
- Primary: #1DB954 (Spotify Green)
- Text Primary: #FFFFFF
- Text Secondary: #B3B3B3
- Accent: #1DB954
- Liquid Glass: rgba(255, 255, 255, 0.05) (Glass Effekt)

LAYOUT FOR 320x480 DISPLAY:

[0]    [320]
   ↓↓↓↓↓↓↓↓↓↓↓↓
[0]  ╔════════════════════════════════════════════╗
     ║                                            ║
     ║                                            ║
[20] ║       [ Album Cover - 260x260 ]            ║
     ║       [ Rounded - 12px ]                    ║
     ║                                            ║
     ║                                            ║
     ║                                            ║
     ║                                            ║
[280]╠════════════════════════════════════════════╣
     ║  🎵 Bohemian Rhapsody                     ║
     ║     Queen                                 ║
     ║     A Night at the Opera                  ║
[325]╠════════════════════════════════════════════╣
     ║  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━  ║
     ║  2:34 / 5:55                              ║
[350]╠════════════════════════════════════════════╣
     ║   [+]   ◀    [⏸/▶]    ▶                ║
     ║  Save   Prev   Play   Next               ║
[400]╚════════════════════════════════════════════╝
          │
          │ Volume Bar (vertikal)
          ▼
         75%
        ━━━
        ━━━
        ━━━


PIXEL COORDINATES (320x480):

Album Cover:
  x: 30, y: 20
  w: 260, h: 260
  radius: 12
  shadow: 0 4px 12px rgba(0,0,0,0.5) (Liquid Glass)

Song Title:
  x: 30, y: 280
  size: 22px
  color: #FFFFFF
  weight: Bold

Artist Name:
  x: 30, y: 310
  size: 16px
  color: #B3B3B3

Album Name:
  x: 30, y: 330
  size: 14px
  color: #B3B3B3

Progress Bar:
  x: 30, y: 350
  w: 260, h: 4
  background: #282828
  progress: #1DB954
  corner radius: 2px

Time Labels:
  Current: x: 30, y: 360, size: 12px, color: #B3B3B3
  Total: x: 290, y: 360, size: 12px, color: #B3B3B3

Controls:
  Save (+): x: 35, y: 375, size: 38px
  Previous: x: 95, y: 375, size: 38px
  Play/Pause: x: 145, y: 370, size: 48px
  Next: x: 195, y: 375, size: 38px
  color: #1DB954
  glass effect: rgba(255, 255, 255, 0.05)

Volume Bar (Vertikal rechts):
  x: 290, y: 20
  w: 4, h: 440
  background: #282828
  volume: #1DB954
  corner radius: 2px

Volume Indicator:
  x: 280, y: 230 (center of volume)
  size: 20px
  color: #FFFFFF
  text: "🔊 75%"

TOUCH AREAS (Buttons):

Button      | x1  | y1  | x2  | y2  | Action
------------|-----|-----|-----|-----|------------------
Save (+)    | 25  | 365 | 75  | 415 | add_to_playlist()
Previous    | 80  | 365 | 130 | 415 | previous_track()
Play/Pause  | 125 | 360 | 175 | 430 | toggle_play()
Next        | 180 | 365 | 230 | 415 | next_track()
Volume      | 280 | 20  | 300 | 460 | set_volume(y)

LANDSCAPE MODE (480x320):

[0]   [480]
  ↓↓↓↓↓↓↓↓
[0] ╔══════════════════════════════════════════════════════════╗
    ║                                                        ║
    ║  [ Cover ]  [ Title   ]  [ Progress ]  [ Controls ]   ║
    ║  [ 260x260]              [ ◀ ⏸ ▶ ]                  ║
    ║                                                        ║
    ║                                                        ║
[320]╚══════════════════════════════════════════════════════════╝
          │
          │ Volume vertikal
          ▼

LAYOUT FOR 800x480 (7 Inch):

[0]     [800]
   ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
[0]  ╔════════════════════════════════════════════════════════════════╗
     ║                                                                ║
     ║                                                                ║
[30] ║       [ Album Cover - 400x400 (Groß!) ]                        ║
     ║       [ Rounded - 15px ]                                        ║
     ║                                                                ║
     ║                                                                ║
     ║                                                                ║
     ║                                                                ║
[430]╠════════════════════════════════════════════════════════════════╣
     ║  🎵 Bohemian Rhapsody                     🎤 Queen            ║
     ║     A Night at the Opera                     💿 Album        ║
[460]╠════════════════════════════════════════════════════════════════╣
     ║  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━  ║
     ║  2:34 / 5:55                                                       ║
[485]╠════════════════════════════════════════════════════════════════╣
     ║   [+]       ◀        [ ⏸/▶ ]        ▶                    ║
     ║  Save     Prev       Play/Pause      Next                   ║
[525]╚════════════════════════════════════════════════════════════════╝
           │
           │ Volume vertikal
           ▼
          75%


PIXEL COORDINATES (800x480):

Album Cover:
  x: 50, y: 30
  w: 400, h: 400
  radius: 15

Song Title:
  x: 50, y: 430
  size: 28px

Artist Name:
  x: 500, y: 430
  size: 18px

Album Name:
  x: 50, y: 460
  size: 16px

Progress Bar:
  x: 50, y: 485
  w: 700, h: 6
  background: #282828
  progress: #1DB954

Controls:
  Save (+): x: 80, y: 495, size: 50px
  Previous: x: 180, y: 495, size: 50px
  Play/Pause: x: 300, y: 488, size: 62px
  Next: x: 420, y: 495, size: 50px
  color: #1DB954

Volume Bar (Vertikal rechts):
  x: 780, y: 30
  w: 4, h: 440
  background: #282828
  volume: #1DB954

LIQUID GLASS EFFECTS:
- Background: rgba(30, 30, 30, 0.8) auf Surface
- Border: 1px solid rgba(255, 255, 255, 0.1)
- Shadow: 0 4px 12px rgba(0, 0, 0, 0.5)
- Backdrop Blur: 8px (wenn unterstützt)


COLORS DEFINED:
#define COLOR_BG        0x1212
#define COLOR_SURFACE   0x1E1E  // Helles Grau für Liquid Glass
#define COLOR_GLASS     0x1A1A  // Glass Hintergrund
#define COLOR_PRIMARY   0x1DB954  // Spotify Green
#define COLOR_TEXT      0xFFFFFF
#define COLOR_TEXT_SEC  0xB3B3B3
#define COLOR_ACCENT    0x1DB954
#define COLOR_PROGRESS_BG 0x2828

// Use with TFT_eSPI:
tft.fillScreen(COLOR_BG);
tft.fillRoundRect(x, y, w, h, radius, COLOR_SURFACE);
tft.setTextColor(COLOR_TEXT);
tft.setTextSize(size);
tft.setCursor(x, y);

// Liquid Glass Effect (simuliert):
tft.fillRoundRect(x, y, w, h, radius, COLOR_GLASS);
tft.drawRoundRect(x, y, w, h, radius, 0x3030); // Subtle border
*/

// ===== EXAMPLE UI RENDERING CODE =====
void drawNowPlayingScreen() {
  // Clear screen
  tft.fillScreen(COLOR_BG);

  // Draw Album Cover placeholder with Liquid Glass effect
  tft.fillRoundRect(30, 20, 260, 260, 12, COLOR_SURFACE);
  tft.drawRoundRect(30, 20, 260, 260, 12, 0x3030); // Glass border

  // Draw Song Title
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(22 / 8);
  tft.setCursor(30, 280);
  tft.println("Song Title");

  // Draw Artist
  tft.setTextColor(COLOR_TEXT_SEC);
  tft.setTextSize(16 / 8);
  tft.setCursor(30, 310);
  tft.println("Artist Name");

  // Draw Album Name
  tft.setTextColor(COLOR_TEXT_SEC);
  tft.setTextSize(14 / 8);
  tft.setCursor(30, 330);
  tft.println("Album Name");

  // Draw Progress Bar
  tft.fillRect(30, 350, 260, 4, COLOR_PROGRESS_BG);
  tft.fillRect(30, 350, 140, 4, COLOR_PRIMARY); // 54% progress

  // Draw Time Labels
  tft.setTextColor(COLOR_TEXT_SEC);
  tft.setTextSize(12 / 8);
  tft.setCursor(30, 360);
  tft.println("2:34");
  tft.setCursor(290, 360);
  tft.println("5:55");

  // Draw Controls
  tft.setTextColor(COLOR_PRIMARY);
  tft.setTextSize(38 / 8);

  // Save (+)
  tft.setCursor(35, 375);
  tft.println("+");

  // Previous
  tft.setCursor(95, 375);
  tft.println("◀");

  // Play/Pause
  tft.setTextSize(48 / 8);
  tft.setCursor(145, 370);
  if (isPlaying) {
    tft.println("⏸");
  } else {
    tft.println("▶");
  }

  // Next
  tft.setTextSize(38 / 8);
  tft.setCursor(195, 375);
  tft.println("▶");

  // Draw Vertical Volume Bar (rechts)
  uint16_t volH = 330; // 75% of 440
  tft.fillRect(290, 20, 4, 440, COLOR_PROGRESS_BG);
  tft.fillRect(290, 440 - volH, 4, volH, COLOR_PRIMARY);

  // Volume Indicator
  tft.setTextColor(COLOR_TEXT_SEC);
  tft.setTextSize(12 / 8);
  tft.setCursor(275, 230);
  tft.println("75%");
}
