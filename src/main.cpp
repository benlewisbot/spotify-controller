#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Display Manager - Automatisch wählen basierend auf DISPLAY_TYPE
#if DISPLAY_TYPE == CYD_7_INCH
  // UART Serial Display
  #include "serial_display_manager.h"
  #include "touch_manager_uart.h"
  SerialDisplayManager display;
  TouchManagerUART touch;
#else
  // SPI Display
  #include <TFT_eSPI.h>
  #include "display_manager.h"
  #include "touch_manager.h"
  TFT_eSPI tft = TFT_eSPI();
  DisplayManager display;
  TouchManager touch;
#endif

// Spotify Manager
#include "spotify_manager.h"
SpotifyManager spotify;

// Globals
unsigned long lastSpotifyPoll = 0;
unsigned long lastTouchCheck = 0;
bool isPlaying = false;
bool isMockMode = true; // Wokwi Simulation = true, Hardware = false

// Color Definitions (Spotify App Style)
#if !DISPLAY_TYPE == CYD_7_INCH
  #define COLOR_BG           0x1212
  #define COLOR_SURFACE      0x1E1E
  #define COLOR_GLASS        0x1A1A
  #define COLOR_PRIMARY      0x1DB954
  #define COLOR_TEXT         0xFFFF
  #define COLOR_TEXT_SEC     0xB3B3
  #define COLOR_PROGRESS_BG  0x2828
#endif

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);

  Serial.println("\n==== Spotify Controller ESP32 ===");
  Serial.printf("Version: 1.0.0 - Prototype\n");
  Serial.printf("Mode: %s\n", isMockMode ? "Wokwi Simulation" : "Live Hardware");

  // 1. Hardware initialisieren
  initHardware();

  // 2. Spotify initialisieren
  initSpotify();

  // 3. UI initialisieren
  initUI();

  Serial.println("✅ Setup abgeschlossen!");
  Serial.println("🎵 Spotify Controller bereit!");
}

void loop() {
  // Spotify Status alle SPOTIFY_API_POLL_MS prüfen
  if (millis() - lastSpotifyPoll > SPOTIFY_API_POLL_MS) {
    updateNowPlaying();
    lastSpotifyPoll = millis();
  }

  // Touch-Handling alle 50ms
  if (millis() - lastTouchCheck > 50) {
    handleTouch();
    lastTouchCheck = millis();
  }
}

// ===== HARDWARE INITIALISIERUNG =====

void initHardware() {
  Serial.println("🔧 Hardware wird initialisiert...");

  #if DISPLAY_TYPE == CYD_7_INCH
    // UART Serial Display
    display.init(Serial2, TFT_TX, TFT_RX, TFT_BAUD);
    touch.init(Serial2, TOUCH_RX, TOUCH_TX);
    Serial.println("  - Serial Display (UART)");
  #else
    // SPI Display
    display.init(tft);
    touch.init();
    Serial.println("  - SPI Display");
  #endif

  Serial.println("✅ Hardware initialisiert");
}

// ===== SPOTIFY INITIALISIERUNG =====

void initSpotify() {
  Serial.println("🎵 Spotify wird initialisiert...");

  // Mock Mode für Wokwi Simulation
  spotify.init(isMockMode);

  #if !isMockMode
    // Echte Credentials (nur im Live Mode)
    // TODO: Aus LittleFS laden
    spotify.setCredentials("YOUR_CLIENT_ID", "YOUR_CLIENT_SECRET");
    spotify.setTokens("YOUR_ACCESS_TOKEN", "YOUR_REFRESH_TOKEN");
  #endif

  Serial.println("✅ Spotify initialisiert");
}

// ===== SPOTIFY UPDATE =====

void updateNowPlaying() {
  // Track Info abrufen
  SpotifyManager::TrackInfo track = spotify.getNowPlaying();

  #if DEBUG_SPOTIFY
    Serial.println("🎵 Now Playing:");
    Serial.printf("  Titel: %s\n", track.title.c_str());
    Serial.printf("  Artist: %s\n", track.artist.c_str());
    Serial.printf("  Album: %s\n", track.album.c_str());
    Serial.printf("  Status: %s\n", track.isPlaying ? "Playing" : "Paused");
    Serial.printf("  Fortschritt: %d:%02d / %d:%02d\n",
      track.progressMs / 60000, (track.progressMs % 60000) / 1000,
      track.durationMs / 60000, (track.durationMs % 60000) / 1000);
    Serial.printf("  Volume: %d%%\n", track.volumePercent);
  #endif

  // UI updaten
  drawNowPlaying(track);
}

// ===== UI =====

void initUI() {
  Serial.println("🖥 UI wird initialisiert...");

  #if DISPLAY_TYPE == CYD_7_INCH
    // UART Serial Display
    display.setFillColor(0x12, 0x12, 0x12);
    display.clearScreen();
  #else
    // SPI Display
    display.init(tft);
    display.fillScreen(COLOR_BG);
  #endif

  Serial.println("✅ UI initialisiert");
}

void drawNowPlaying(SpotifyManager::TrackInfo track) {
  // Clear screen
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(0x12, 0x12, 0x12);
    display.clearScreen();
  #else
    display.fillScreen(COLOR_BG);
  #endif

  // Album Cover
  drawAlbumCover();

  // Track Info
  drawTrackInfo(track);

  // Progress Bar
  drawProgressBar(track);

  // Controls
  drawControls(track);

  // Volume
  drawVolumeBar(track);
}

void drawAlbumCover() {
  // Album Cover Bereich
  uint16_t coverSize = 260;
  uint16_t coverX = (DISPLAY_WIDTH - coverSize) / 2;
  uint16_t coverY = 20;

  #if DISPLAY_TYPE == CYD_7_INCH
    // 7 Inch: Größes Cover
    coverSize = 400;
    coverX = (DISPLAY_WIDTH - coverSize) / 2;
    coverY = 30;

    display.setFillColor(0x1E, 0x1E, 0x1E);
    display.drawRoundRect(coverX, coverY, coverSize, coverSize, 15);
  #else
    display.fillRoundRect(coverX, coverY, coverSize, coverSize, 12, COLOR_SURFACE);
  #endif
}

void drawTrackInfo(SpotifyManager::TrackInfo track) {
  uint16_t y;

  #if DISPLAY_TYPE == CYD_7_INCH
    // 7 Inch Layout
    y = 430;

    // Song Title
    display.setFillColor(0xFF, 0xFF, 0xFF);
    display.setCursor(50, y);
    display.print(track.title);

    // Artist
    display.setFillColor(0xB3, 0xB3, 0xB3);
    display.setCursor(500, y);
    display.print(track.artist);
  #else
    // Standard Layout (320x480)
    y = 280;

    // Song Title
    display.setTextColor(COLOR_TEXT);
    display.setCursor(30, y);
    display.print(track.title);

    // Artist Name
    y += 30;
    display.setTextColor(COLOR_TEXT_SEC);
    display.setCursor(30, y);
    display.print(track.artist);

    // Album Name
    y += 20;
    display.setTextColor(COLOR_TEXT_SEC);
    display.setCursor(30, y);
    display.print(track.album);
  #endif
}

void drawProgressBar(SpotifyManager::TrackInfo track) {
  uint16_t x, y, w, h, progressW;

  #if DISPLAY_TYPE == CYD_7_INCH
    // 7 Inch
    x = 50;
    y = 485;
    w = 700;
    h = 6;
  #else
    // Standard (320x480)
    x = 30;
    y = 350;
    w = 260;
    h = 4;
  #endif

  progressW = (track.progressMs * w) / track.durationMs;

  // Background
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(0x28, 0x28, 0x28);
    display.fillRect(x, y, w, h);
  #else
    display.fillRect(x, y, w, h, COLOR_PROGRESS_BG);
  #endif

  // Progress
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(0x1D, 0xB9, 0x54);
    display.fillRect(x, y, progressW, h);
  #else
    display.fillRect(x, y, progressW, h, COLOR_PRIMARY);
  #endif

  // Time Labels
  uint16_t progressMin = track.progressMs / 60000;
  uint16_t progressSec = (track.progressMs % 60000) / 1000;
  uint16_t durationMin = track.durationMs / 60000;
  uint16_t durationSec = (track.durationMs % 60000) / 1000;

  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(0xB3, 0xB3, 0xB3);
    display.setCursor(50, y + 15);
    display.printf("%d:%02d", progressMin, progressSec);
    display.setCursor(750, y + 15);
    display.printf("%d:%02d", durationMin, durationSec);
  #else
    display.setTextColor(COLOR_TEXT_SEC);
    display.setCursor(x, y + 10);
    display.printf("%d:%02d", progressMin, progressSec);
    display.setCursor(x + w - 60, y + 10);
    display.printf("%d:%02d", durationMin, durationSec);
  #endif
}

void drawControls(SpotifyManager::TrackInfo track) {
  uint16_t y;

  #if DISPLAY_TYPE == CYD_7_INCH
    // 7 Inch
    y = 495;
  #else
    // Standard (320x480)
    y = 375;
  #endif

  // Save (+) Button
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(track.saved ? 0x1D : 0xB3,
                        track.saved ? 0xB9 : 0xB3,
                        track.saved ? 0x54 : 0xB3);
    display.setCursor(80, y);
    display.print("+");
  #else
    display.setTextColor(track.saved ? COLOR_PRIMARY : COLOR_TEXT_SEC);
    display.setCursor(35, y);
    display.print("+");
  #endif

  // Previous Button
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(0x1D, 0xB9, 0x54);
    display.setCursor(180, y);
    display.print("◀");
  #else
    display.setTextColor(COLOR_PRIMARY);
    display.setCursor(95, y);
    display.print("◀");
  #endif

  // Play/Pause Button
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setCursor(300, y - 7);
    if (track.isPlaying) {
      display.print("⏸");
    } else {
      display.print("▶");
    }
  #else
    display.setTextColor(COLOR_PRIMARY);
    display.setCursor(145, y - 5);
    if (track.isPlaying) {
      display.print("⏸");
    } else {
      display.print("▶");
    }
  #endif

  // Next Button
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setCursor(420, y);
    display.print("▶");
  #else
    display.setTextColor(COLOR_PRIMARY);
    display.setCursor(195, y);
    display.print("▶");
  #endif
}

void drawVolumeBar(SpotifyManager::TrackInfo track) {
  uint16_t x, y, w, h, volH;

  #if DISPLAY_TYPE == CYD_7_INCH
    // 7 Inch
    x = 780;
    y = 30;
    w = 4;
    h = 440;
  #else
    // Standard (320x480)
    x = 290;
    y = 20;
    w = 4;
    h = 440;
  #endif

  volH = (track.volumePercent * h) / 100;

  // Background
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(0x28, 0x28, 0x28);
    display.fillRect(x, y, w, h);
  #else
    display.fillRect(x, y, w, h, COLOR_PROGRESS_BG);
  #endif

  // Volume
  #if DISPLAY_TYPE == CYD_7_INCH
    display.setFillColor(0x1D, 0xB9, 0x54);
    display.fillRect(x, y + h - volH, w, volH);
  #else
    display.fillRect(x, y + h - volH, w, volH, COLOR_PRIMARY);
  #endif
}

// ===== TOUCH HANDLING =====

void handleTouch() {
  #if DISPLAY_TYPE == CYD_7_INCH
    // UART Touch
    auto touchPoint = touch.getTouchPoint();
    if (touchPoint.pressed) {
      handleTouchPoint(touchPoint.x, touchPoint.y);
    }
  #else
    // Resistive/Capacitive Touch
    auto touchPoint = touch.getTouchPoint();
    if (touchPoint.pressed) {
      handleTouchPoint(touchPoint.x, touchPoint.y);
    }
  #endif
}

void handleTouchPoint(uint16_t x, uint16_t y) {
  // Save (+) Button
  if (x >= 25 && x <= 75 && y >= 365 && y <= 415) {
    Serial.println("+ Save Track toggled");
    // TODO: spotify.toggleSaveTrack();
    SpotifyManager::TrackInfo track = spotify.getNowPlaying();
    track.saved = !track.saved;
    drawControls(track);
  }
  // Previous Button
  else if (x >= 80 && x <= 130 && y >= 365 && y <= 415) {
    Serial.println("⏮ Previous Track");
    if (spotify.previousTrack()) {
      updateNowPlaying();
    }
  }
  // Play/Pause Button
  else if (x >= 125 && x <= 175 && y >= 360 && y <= 430) {
    Serial.println("▶ Play/Pause toggled");
    if (spotify.togglePlay()) {
      updateNowPlaying();
    }
  }
  // Next Button
  else if (x >= 180 && x <= 230 && y >= 365 && y <= 415) {
    Serial.println("⏭ Next Track");
    if (spotify.nextTrack()) {
      updateNowPlaying();
    }
  }
  // Volume Bar (vertikal rechts)
  else if (x >= 280 && x <= 300 && y >= 20 && y <= 460) {
    int volume = map(y, 20, 460, 100, 0); // Invert: oben = 100%
    Serial.printf("🔊 Volume: %d%%\n", volume);
    if (spotify.setVolume(volume)) {
      updateNowPlaying();
    }
  }
}

// ===== UTILITIES =====

void restartESP() {
  Serial.println("🔄 Neustart...");
  ESP.restart();
}
