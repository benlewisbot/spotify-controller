/**
 * @file sketch.ino
 * @brief Spotify Controller - Wokwi Demo
 *
 * This is a Wokwi-compatible demo of the Spotify Controller.
 * It simulates the UI without actual WiFi/Spotify API.
 *
 * To run this demo:
 * 1. Visit https://wokwi.com/projects/new/esp32
 * 2. Copy this file content to the editor
 * 3. Run and see the UI in action!
 */

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// Display configuration for Wokwi
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

// UI Colors (Spotify palette)
#define COLOR_BG 0x121212      // Dark background
#define COLOR_SURFACE 0x282828  // Elevated surface
#define COLOR_PRIMARY 0x1DB954 // Spotify green
#define COLOR_WHITE 0xFFFFFF
#define COLOR_GRAY 0xB3B3B3

// UI Elements
TFT_eSPI tft = TFT_eSPI();

// Mock track data for demo
struct MockTrack {
  const char* title;
  const char* artist;
  const char* album;
  const char* coverText;
};

MockTrack tracks[] = {
  {"Papercut", "Linkin Park", "Hybrid Theory", "LP-HYB"},
  {"In The End", "Linkin Park", "Hybrid Theory", "LP-HYB"},
  {"Numb", "Linkin Park", "Meteora", "LP-MET"},
  {"One Step Closer", "Linkin Park", "Hybrid Theory", "LP-HYB"},
  {"Faint", "Linkin Park", "Meteora", "LP-MET"}
};

int currentTrack = 0;
bool isPlaying = true;
int volume = 75;
int progressMs = 0;
int durationMs = 185000;

// UI state
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 100; // 10 FPS for smooth progress

// Touch handling (mock for Wokwi)
int touchX = 0;
int touchY = 0;
bool touched = false;

void setup() {
  Serial.begin(115200);

  Serial.println("\n========================================");
  Serial.println("  🎵 Spotify Controller - Wokwi Demo");
  Serial.println("  Version: 1.0.0 (Demo Mode)");
  Serial.println("========================================\n");

  // Initialize display
  tft.init();
  tft.setRotation(1); // Landscape
  tft.fillScreen(COLOR_BG);

  Serial.println("✅ Display initialized");
  Serial.println("✅ UI rendering started");

  // Draw initial UI
  drawUI();

  Serial.println("\n🎵 Demo is running!");
  Serial.println("  Tracks will cycle automatically");
  Serial.println("  Touch areas are simulated");
}

void loop() {
  unsigned long now = millis();

  // Update progress
  if (isPlaying && (now - lastUpdate >= UPDATE_INTERVAL)) {
    progressMs += UPDATE_INTERVAL;
    if (progressMs >= durationMs) {
      // Next track
      progressMs = 0;
      currentTrack = (currentTrack + 1) % 5;
      drawTrackInfo();
    }
    drawProgressBar();
    lastUpdate = now;
  }

  // Simulate touch interaction (in real hardware, use touch library)
  // For Wokwi, you can add touch simulation via serial commands
  handleTouch();
}

void drawUI() {
  // Clear screen
  tft.fillScreen(COLOR_BG);

  // Draw album art placeholder
  drawAlbumArt();

  // Draw track info
  drawTrackInfo();

  // Draw progress bar
  drawProgressBar();

  // Draw controls
  drawControls();

  // Draw volume
  drawVolume();
}

void drawAlbumArt() {
  // Album art area (left side)
  int artX = 16;
  int artY = 40;
  int artSize = 220;

  // Draw placeholder with gradient effect
  tft.fillRect(artX, artY, artSize, artSize, COLOR_SURFACE);

  // Draw border
  tft.drawRect(artX, artY, artSize, artSize, COLOR_PRIMARY);

  // Draw cover text (centered)
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(3);
  tft.setTextDatum(MC_DATUM);
  String text = tracks[currentTrack].coverText;
  tft.drawString(text.c_str(), artX + artSize/2, artY + artSize/2);
}

void drawTrackInfo() {
  // Track title
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(tracks[currentTrack].title, 250, 40);

  // Artist name
  tft.setTextColor(COLOR_GRAY);
  tft.setTextSize(1);
  tft.drawString(tracks[currentTrack].artist, 250, 70);

  // Album name
  tft.drawString(tracks[currentTrack].album, 250, 90);
}

void drawProgressBar() {
  int y = 260;
  int height = 4;
  int x = 16;
  int width = TFT_HEIGHT - 32; // Landscape width

  // Background
  tft.fillRect(x, y, width, height, COLOR_SURFACE);

  // Progress
  int progress = (progressMs * 100) / durationMs;
  int progressWidth = (progress * width) / 100;
  tft.fillRect(x, y, progressWidth, height, COLOR_WHITE);

  // Time labels
  tft.setTextColor(COLOR_GRAY);
  tft.setTextSize(1);

  // Current time
  char timeStr[16];
  sprintf(timeStr, "%d:%02d", progressMs / 60000, (progressMs % 60000) / 1000);
  tft.setTextDatum(BL_DATUM);
  tft.drawString(timeStr, x, y - 4);

  // Total time
  sprintf(timeStr, "%d:%02d", durationMs / 60000, (durationMs % 60000) / 1000);
  tft.setTextDatum(BR_DATUM);
  tft.drawString(timeStr, x + width, y - 4);
}

void drawControls() {
  int y = 290;
  int btnSize = 40;

  // Previous button
  int prevX = 100;
  tft.fillCircle(prevX, y, btnSize/2, COLOR_SURFACE);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("<<", prevX, y);

  // Play/Pause button (larger, green)
  int playX = TFT_HEIGHT / 2;
  int playSize = 50;
  tft.fillCircle(playX, y, playSize/2, COLOR_PRIMARY);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.drawString(isPlaying ? "||" : ">", playX, y);

  // Next button
  int nextX = TFT_HEIGHT - 100;
  tft.fillCircle(nextX, y, btnSize/2, COLOR_SURFACE);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.drawString(">>", nextX, y);
}

void drawVolume() {
  // Volume slider (right side, vertical)
  int x = TFT_WIDTH - 20;
  int y = 100;
  int width = 6;
  int height = 120;

  // Background
  tft.fillRect(x - width/2, y, width, height, COLOR_SURFACE);

  // Fill
  int fillHeight = (volume * height) / 100;
  tft.fillRect(x - width/2, y + height - fillHeight, width, fillHeight, COLOR_PRIMARY);

  // Knob
  int knobY = y + height - fillHeight;
  tft.fillCircle(x, knobY, 10, COLOR_WHITE);

  // Volume icon at top
  tft.setTextColor(COLOR_GRAY);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("VOL", x, y - 10);
}

void handleTouch() {
  // In Wokwi, touch is simulated via serial commands
  // For this demo, auto-cycle tracks

  // Real hardware would use touch library like FT6236 or XPT2046
}

// Serial command handler for Wokwi simulation
void serialEvent() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "play") {
      isPlaying = !isPlaying;
      drawControls();
      Serial.println(isPlaying ? "Playing" : "Paused");
    } else if (cmd == "next") {
      currentTrack = (currentTrack + 1) % 5;
      progressMs = 0;
      drawAlbumArt();
      drawTrackInfo();
      Serial.println("Next track");
    } else if (cmd == "prev") {
      currentTrack = (currentTrack - 1 + 5) % 5;
      progressMs = 0;
      drawAlbumArt();
      drawTrackInfo();
      Serial.println("Previous track");
    } else if (cmd.startsWith("vol ")) {
      int vol = cmd.substring(4).toInt();
      volume = constrain(vol, 0, 100);
      drawVolume();
      Serial.print("Volume: ");
      Serial.println(volume);
    } else if (cmd == "help") {
      Serial.println("\n=== Spotify Controller Demo Commands ===");
      Serial.println("  play     - Toggle play/pause");
      Serial.println("  next     - Next track");
      Serial.println("  prev     - Previous track");
      Serial.println("  vol <N>  - Set volume (0-100)");
      Serial.println("  help     - Show this help");
    }
  }
}
