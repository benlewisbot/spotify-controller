/**
 * @file sketch.ino
 * @brief Spotify Controller - Wokwi Demo
 *
 * Full demonstration of Spotify Controller for LilyGo T-Display S3 Touch (480x480)
 *
 * Hardware: LilyGo T-Display S3 Touch (ESP32-4848S040C)
 * Display: 480x480 round display with capacitive touch (FT6236)
 *
 * To run this demo:
 * 1. Visit https://wokwi.com/projects/new/esp32
 * 2. Copy this file content to the editor
 * 3. Click ▶️ Start to see the UI in action!
 */

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// ============================================================
// CONFIGURATION - LilyGo T-Display S3 Touch (480x480)
// ============================================================

// Display configuration
// Note: Wokwi supports up to 320x320 for ST7789 simulation
// The actual hardware is 480x480 round display
#define TFT_WIDTH 320
#define TFT_HEIGHT 320
#define TFT_CENTER_X (TFT_WIDTH / 2)
#define TFT_CENTER_Y (TFT_HEIGHT / 2)

// Spotify Dark Theme Colors (#121212, #1DB954)
#define COLOR_BG 0x1212      // Dark background
#define COLOR_SURFACE 0x2828  // Elevated surface
#define COLOR_PRIMARY 0x1DB954 // Spotify green
#define COLOR_PRIMARY_DARK 0x17993F // Darker green
#define COLOR_WHITE 0xFFFFFF
#define COLOR_GRAY 0xB3B3B3
#define COLOR_GRAY_DARK 0x727272

// UI Dimensions
#define UI_MARGIN 16
#define ALBUM_ART_SIZE 180
#define ALBUM_ART_X UI_MARGIN
#define ALBUM_ART_Y 60
#define PROGRESS_BAR_HEIGHT 6
#define PROGRESS_BAR_Y ALBUM_ART_Y + ALBUM_ART_SIZE + 24
#define CONTROLS_Y TFT_HEIGHT - 50
#define BUTTON_SIZE 48
#define BUTTON_PLAY_SIZE 56
#define VOLUME_SLIDER_WIDTH 8
#define VOLUME_SLIDER_HEIGHT 120
#define VOLUME_SLIDER_X TFT_WIDTH - UI_MARGIN - VOLUME_SLIDER_WIDTH
#define VOLUME_SLIDER_Y 80

// Touch areas
#define TOUCH_THRESHOLD 10

// ============================================================
// MOCK TRACK DATA
// ============================================================

struct MockTrack {
  const char* title;
  const char* artist;
  const char* album;
  const char* coverText;
  uint16_t bgR, bgG, bgB;  // Cover background color
};

MockTrack tracks[] = {
  {"Papercut", "Linkin Park", "Hybrid Theory", "LP-HYB", 0x40, 0x40, 0x50},
  {"In The End", "Linkin Park", "Hybrid Theory", "LP-HYB", 0x50, 0x30, 0x40},
  {"Numb", "Linkin Park", "Meteora", "LP-MET", 0x30, 0x40, 0x50},
  {"One Step Closer", "Linkin Park", "Hybrid Theory", "LP-HYB", 0x45, 0x35, 0x45},
  {"Faint", "Linkin Park", "Meteora", "LP-MET", 0x35, 0x35, 0x55},
  {"Breaking the Habit", "Linkin Park", "Meteora", "LP-MET", 0x40, 0x45, 0x35}
};

int currentTrackIndex = 0;
bool isPlaying = true;
bool isSaved = false;
int volume = 65;
int progressMs = 0;
int durationMs = 185000;

// UI Update timing
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 100; // 10 FPS

// Touch state
int touchX = 0;
int touchY = 0;
bool touched = false;
unsigned long touchStartTime = 0;

// Display
TFT_eSPI tft = TFT_eSPI();

// ============================================================
// SETUP
// ============================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n" + String("=") * 40);
  Serial.println("  🎵 Spotify Controller - Wokwi Demo");
  Serial.println("  Version: 2.0.0 (Full Feature Demo)");
  Serial.println("  Hardware: LilyGo T-Display S3 Touch");
  Serial.println("  Display: 480x480 (simulated)");
  Serial.println("=" * 40 + "\n");

  // Initialize display
  tft.init();
  tft.setRotation(1); // Landscape for Wokwi simulation
  tft.fillScreen(COLOR_BG);

  Serial.println("✅ Display initialized");

  // Draw initial UI
  drawUI();

  Serial.println("✅ UI rendered");
  Serial.println("\n🎵 Demo is running!");
  Serial.println("  Type 'help' for available commands\n");
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {
  unsigned long now = millis();

  // Update progress if playing
  if (isPlaying && (now - lastUpdate >= UPDATE_INTERVAL)) {
    progressMs += UPDATE_INTERVAL;

    // Check if track ended
    if (progressMs >= durationMs) {
      nextTrack();
    }

    drawProgressBar();
    updatePlayButton();
    lastUpdate = now;
  }

  // Handle touch simulation (via serial)
  handleTouch();

  // Process serial commands
  processSerialCommands();
}

// ============================================================
// UI RENDERING
// ============================================================

void drawUI() {
  // Clear screen with background
  tft.fillScreen(COLOR_BG);

  // 1. Header area (menu, volume icon)
  drawHeader();

  // 2. Album art placeholder
  drawAlbumArt();

  // 3. Track info (title, artist, album)
  drawTrackInfo();

  // 4. Progress bar with time labels
  drawProgressBar();

  // 5. Playback controls (previous, play/pause, next)
  drawControls();

  // 6. Volume slider (vertical)
  drawVolumeSlider();
}

void drawHeader() {
  // Menu button (top left)
  tft.fillRoundRect(UI_MARGIN, UI_MARGIN, 36, 36, 8, COLOR_SURFACE);
  tft.setTextColor(COLOR_GRAY);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("≡", UI_MARGIN + 18, UI_MARGIN + 18);

  // Save/Heart button (top, right of volume slider)
  drawSaveButton();

  // Volume icon (top of slider)
  tft.setTextColor(COLOR_GRAY);
  tft.setTextSize(1);
  tft.drawString("🔊", VOLUME_SLIDER_X + 4, VOLUME_SLIDER_Y - 16);
}

void drawAlbumArt() {
  MockTrack* track = &tracks[currentTrackIndex];

  // Album art background with track-specific color
  uint16_t bg = tft.color565(track->bgR, track->bgG, track->bgB);
  tft.fillRoundRect(ALBUM_ART_X, ALBUM_ART_Y, ALBUM_ART_SIZE, ALBUM_ART_SIZE, 16, bg);

  // Border
  tft.drawRoundRect(ALBUM_ART_X, ALBUM_ART_Y, ALBUM_ART_SIZE, ALBUM_ART_SIZE, 16, COLOR_SURFACE);

  // Cover text (centered)
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(3);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(track->coverText,
                  ALBUM_ART_X + ALBUM_ART_SIZE / 2,
                  ALBUM_ART_Y + ALBUM_ART_SIZE / 2);

  // Music icon below text
  tft.setTextSize(2);
  tft.drawString("🎵",
                  ALBUM_ART_X + ALBUM_ART_SIZE / 2,
                  ALBUM_ART_Y + ALBUM_ART_SIZE / 2 + 30);
}

void drawTrackInfo() {
  MockTrack* track = &tracks[currentTrackIndex];

  int infoX = ALBUM_ART_X + ALBUM_ART_SIZE + UI_MARGIN;
  int startY = UI_MARGIN + 8;

  // Track title
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(track->title, infoX, startY);

  // Artist name
  tft.setTextColor(COLOR_GRAY);
  tft.setTextSize(1);
  tft.drawString(track->artist, infoX, startY + 30);

  // Album name
  tft.drawString(track->album, infoX, startY + 50);

  // Playback indicator
  tft.setTextColor(isPlaying ? COLOR_PRIMARY : COLOR_GRAY_DARK);
  tft.setTextSize(1);
  tft.drawString(isPlaying ? "● Playing" : "○ Paused", infoX, startY + 80);
}

void drawProgressBar() {
  int y = PROGRESS_BAR_Y;
  int x = UI_MARGIN;
  int width = TFT_WIDTH - UI_MARGIN * 2 - VOLUME_SLIDER_WIDTH - 8;
  int height = PROGRESS_BAR_HEIGHT;

  // Background track
  tft.fillRoundRect(x, y, width, height, 3, COLOR_SURFACE);

  // Progress (filled)
  int progress = (progressMs * 100) / durationMs;
  int progressWidth = (progress * width) / 100;
  tft.fillRoundRect(x, y, progressWidth, height, 3, COLOR_PRIMARY);

  // Time labels
  tft.setTextColor(COLOR_GRAY);
  tft.setTextSize(1);

  // Current time (left)
  char currentTimeStr[16];
  formatTime(currentTimeStr, progressMs);
  tft.setTextDatum(BL_DATUM);
  tft.drawString(currentTimeStr, x, y - 6);

  // Total time (right)
  char totalTimeStr[16];
  formatTime(totalTimeStr, durationMs);
  tft.setTextDatum(BR_DATUM);
  tft.drawString(totalTimeStr, x + width, y - 6);

  // Progress percentage (center)
  tft.setTextDatum(BC_DATUM);
  tft.printf("%d%%", progress);
}

void drawControls() {
  int y = CONTROLS_Y;
  int spacing = 16;
  int totalWidth = BUTTON_SIZE * 2 + BUTTON_PLAY_SIZE + spacing * 2;
  int startX = (TFT_WIDTH - VOLUME_SLIDER_WIDTH - UI_MARGIN * 2 - totalWidth) / 2 + UI_MARGIN;

  // Previous button
  drawButton(startX, y, BUTTON_SIZE, BUTTON_SIZE, COLOR_SURFACE,
             "⏮", COLOR_WHITE);

  // Play/Pause button (larger, centered)
  int playX = startX + BUTTON_SIZE + spacing;
  drawButton(playX, y, BUTTON_PLAY_SIZE, BUTTON_PLAY_SIZE, COLOR_PRIMARY,
             isPlaying ? "⏸" : "▶", COLOR_WHITE);

  // Next button
  int nextX = playX + BUTTON_PLAY_SIZE + spacing;
  drawButton(nextX, y, BUTTON_SIZE, BUTTON_SIZE, COLOR_SURFACE,
             "⏭", COLOR_WHITE);
}

void drawButton(int x, int y, int size, const char* symbol, uint16_t bg, uint16_t fg) {
  tft.fillCircle(x + size/2, y + size/2, size/2, bg);
  tft.setTextColor(fg);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(symbol, x + size/2, y + size/2);
}

void updatePlayButton() {
  // Redraw just the play button
  int y = CONTROLS_Y;
  int spacing = 16;
  int totalWidth = BUTTON_SIZE * 2 + BUTTON_PLAY_SIZE + spacing * 2;
  int startX = (TFT_WIDTH - VOLUME_SLIDER_WIDTH - UI_MARGIN * 2 - totalWidth) / 2 + UI_MARGIN;
  int playX = startX + BUTTON_SIZE + spacing;

  // Clear area
  tft.fillCircle(playX + BUTTON_PLAY_SIZE/2, y + BUTTON_PLAY_SIZE/2, BUTTON_PLAY_SIZE/2 + 2, COLOR_BG);
  drawButton(playX, y, BUTTON_PLAY_SIZE, BUTTON_PLAY_SIZE, COLOR_PRIMARY,
             isPlaying ? "⏸" : "▶", COLOR_WHITE);
}

void drawSaveButton() {
  int x = TFT_WIDTH - UI_MARGIN - VOLUME_SLIDER_WIDTH - 40;
  int y = UI_MARGIN;
  int size = 36;

  tft.fillRoundRect(x, y, size, size, 8, isSaved ? COLOR_PRIMARY : COLOR_SURFACE);

  tft.setTextColor(isSaved ? COLOR_WHITE : COLOR_GRAY);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(isSaved ? "♥" : "♡", x + size/2, y + size/2);
}

void drawVolumeSlider() {
  int x = VOLUME_SLIDER_X;
  int y = VOLUME_SLIDER_Y;
  int height = VOLUME_SLIDER_HEIGHT;

  // Background track
  tft.fillRoundRect(x, y, VOLUME_SLIDER_WIDTH, height, VOLUME_SLIDER_WIDTH/2, COLOR_SURFACE);

  // Fill (from bottom up)
  int fillHeight = (volume * height) / 100;
  tft.fillRoundRect(x, y + height - fillHeight, VOLUME_SLIDER_WIDTH, fillHeight, VOLUME_SLIDER_WIDTH/2, COLOR_PRIMARY);

  // Knob
  int knobY = y + height - fillHeight;
  tft.fillCircle(x + VOLUME_SLIDER_WIDTH/2, knobY, 12, COLOR_WHITE);
}

// ============================================================
// TOUCH HANDLING
// ============================================================

void handleTouch() {
  // In Wokwi, touch is simulated via serial commands
  // Real hardware would read from FT6236 touch controller
}

void processTouch(int x, int y) {
  Serial.printf("Touch: (%d, %d)\n", x, y);

  // Check volume slider
  if (x >= VOLUME_SLIDER_X - 15 && x <= VOLUME_SLIDER_X + VOLUME_SLIDER_WIDTH + 15 &&
      y >= VOLUME_SLIDER_Y - 15 && y <= VOLUME_SLIDER_Y + VOLUME_SLIDER_HEIGHT + 15) {
    // Calculate volume from touch position
    int touchHeight = VOLUME_SLIDER_Y + VOLUME_SLIDER_HEIGHT - y;
    volume = constrain((touchHeight * 100) / VOLUME_SLIDER_HEIGHT, 0, 100);
    drawVolumeSlider();
    Serial.printf("Volume: %d%%\n", volume);
    return;
  }

  // Check progress bar (seek)
  int progressBarX = UI_MARGIN;
  int progressBarWidth = TFT_WIDTH - UI_MARGIN * 2 - VOLUME_SLIDER_WIDTH - 8;
  if (y >= PROGRESS_BAR_Y - 10 && y <= PROGRESS_BAR_Y + PROGRESS_BAR_HEIGHT + 10 &&
      x >= progressBarX && x <= progressBarX + progressBarWidth) {
    int seekPercent = ((x - progressBarX) * 100) / progressBarWidth;
    progressMs = (seekPercent * durationMs) / 100;
    drawProgressBar();
    char timeStr[16];
    formatTime(timeStr, progressMs);
    Serial.printf("Seek to: %s (%d%%)\n", timeStr, seekPercent);
    return;
  }

  // Check controls area
  if (y >= CONTROLS_Y - BUTTON_PLAY_SIZE && y <= CONTROLS_Y + BUTTON_PLAY_SIZE) {
    int spacing = 16;
    int totalWidth = BUTTON_SIZE * 2 + BUTTON_PLAY_SIZE + spacing * 2;
    int startX = (TFT_WIDTH - VOLUME_SLIDER_WIDTH - UI_MARGIN * 2 - totalWidth) / 2 + UI_MARGIN;
    int playX = startX + BUTTON_SIZE + spacing;
    int nextX = playX + BUTTON_PLAY_SIZE + spacing;

    // Previous button
    if (x >= startX && x <= startX + BUTTON_SIZE) {
      previousTrack();
      return;
    }

    // Play/Pause button
    if (x >= playX && x <= playX + BUTTON_PLAY_SIZE) {
      togglePlayPause();
      return;
    }

    // Next button
    if (x >= nextX && x <= nextX + BUTTON_SIZE) {
      nextTrack();
      return;
    }
  }

  // Check save button
  int saveX = TFT_WIDTH - UI_MARGIN - VOLUME_SLIDER_WIDTH - 40;
  if (x >= saveX && x <= saveX + 36 && y >= UI_MARGIN && y <= UI_MARGIN + 36) {
    toggleSave();
    return;
  }
}

// ============================================================
// PLAYER CONTROLS
// ============================================================

void togglePlayPause() {
  isPlaying = !isPlaying;
  updatePlayButton();

  // Update playback indicator
  MockTrack* track = &tracks[currentTrackIndex];
  int infoX = ALBUM_ART_X + ALBUM_ART_SIZE + UI_MARGIN;
  tft.fillRect(infoX, UI_MARGIN + 80, 100, 12, COLOR_BG);
  tft.setTextColor(isPlaying ? COLOR_PRIMARY : COLOR_GRAY_DARK);
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(isPlaying ? "● Playing" : "○ Paused", infoX, UI_MARGIN + 80);

  Serial.println(isPlaying ? "▶ Playing" : "⏸ Paused");
}

void nextTrack() {
  currentTrackIndex = (currentTrackIndex + 1) % (sizeof(tracks) / sizeof(tracks[0]));
  progressMs = 0;
  isSaved = false; // Reset save state
  drawAlbumArt();
  drawTrackInfo();
  drawSaveButton();
  drawProgressBar();
  Serial.printf("⏭ Next track: %s\n", tracks[currentTrackIndex].title);
}

void previousTrack() {
  currentTrackIndex = (currentTrackIndex - 1 + (sizeof(tracks) / sizeof(tracks[0]))) % (sizeof(tracks) / sizeof(tracks[0]));
  progressMs = 0;
  isSaved = false;
  drawAlbumArt();
  drawTrackInfo();
  drawSaveButton();
  drawProgressBar();
  Serial.printf("⏮ Previous track: %s\n", tracks[currentTrackIndex].title);
}

void toggleSave() {
  isSaved = !isSaved;
  drawSaveButton();
  Serial.println(isSaved ? "♥ Added to Liked Songs" : "♡ Removed from Liked Songs");
}

void setVolume(int vol) {
  volume = constrain(vol, 0, 100);
  drawVolumeSlider();
  Serial.printf("Volume: %d%%\n", volume);
}

void seek(int percent) {
  percent = constrain(percent, 0, 100);
  progressMs = (percent * durationMs) / 100;
  drawProgressBar();
  char timeStr[16];
  formatTime(timeStr, progressMs);
  Serial.printf("Seek to: %s (%d%%)\n", timeStr, percent);
}

// ============================================================
// UTILITIES
// ============================================================

void formatTime(char* buffer, int ms) {
  int minutes = ms / 60000;
  int seconds = (ms % 60000) / 1000;
  sprintf(buffer, "%d:%02d", minutes, seconds);
}

// ============================================================
// SERIAL COMMANDS (for Wokwi simulation)
// ============================================================

void processSerialCommands() {
  static String commandBuffer = "";

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (!commandBuffer.isEmpty()) {
        executeCommand(commandBuffer);
        commandBuffer = "";
      }
    } else {
      commandBuffer += c;
    }
  }
}

void executeCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "play" || cmd == "pause" || cmd == "toggle") {
    togglePlayPause();
  }
  else if (cmd == "next") {
    nextTrack();
  }
  else if (cmd == "prev" || cmd == "previous") {
    previousTrack();
  }
  else if (cmd == "save" || cmd == "like") {
    toggleSave();
  }
  else if (cmd.startsWith("vol ")) {
    int vol = cmd.substring(4).toInt();
    setVolume(vol);
  }
  else if (cmd.startsWith("seek ")) {
    int percent = cmd.substring(5).toInt();
    seek(percent);
  }
  else if (cmd.startsWith("touch ")) {
    // Simulate touch: "touch x,y" or "touch x y"
    cmd = cmd.substring(6);
    int commaPos = cmd.indexOf(',');
    int spacePos = cmd.indexOf(' ');
    int separator = (commaPos > 0) ? commaPos : spacePos;
    if (separator > 0) {
      int x = cmd.substring(0, separator).toInt();
      int y = cmd.substring(separator + 1).toInt();
      processTouch(x, y);
    }
  }
  else if (cmd == "info") {
    Serial.println("\n=== Track Info ===");
    Serial.printf("Title: %s\n", tracks[currentTrackIndex].title);
    Serial.printf("Artist: %s\n", tracks[currentTrackIndex].artist);
    Serial.printf("Album: %s\n", tracks[currentTrackIndex].album);
    Serial.printf("Progress: %d / %d ms\n", progressMs, durationMs);
    Serial.printf("Volume: %d%%\n", volume);
    Serial.printf("Saved: %s\n", isSaved ? "Yes" : "No");
  }
  else if (cmd == "tracks") {
    Serial.println("\n=== All Tracks ===");
    for (int i = 0; i < sizeof(tracks) / sizeof(tracks[0]); i++) {
      Serial.printf("%d. %s - %s\n", i + 1, tracks[i].title, tracks[i].artist);
    }
  }
  else if (cmd == "goto") {
    Serial.printf("Usage: goto <1-%d>\n", sizeof(tracks) / sizeof(tracks[0]));
  }
  else if (cmd.startsWith("goto ")) {
    int index = cmd.substring(5).toInt() - 1;
    if (index >= 0 && index < sizeof(tracks) / sizeof(tracks[0])) {
      currentTrackIndex = index;
      progressMs = 0;
      isSaved = false;
      drawAlbumArt();
      drawTrackInfo();
      drawSaveButton();
      drawProgressBar();
      Serial.printf("Track %d: %s\n", index + 1, tracks[index].title);
    } else {
      Serial.printf("Invalid track index. Use 1-%d\n", sizeof(tracks) / sizeof(tracks[0]));
    }
  }
  else if (cmd == "help" || cmd == "?") {
    printHelp();
  }
  else if (!cmd.isEmpty()) {
    Serial.printf("Unknown command: %s (type 'help' for commands)\n", cmd.c_str());
  }
}

void printHelp() {
  Serial.println("\n" + String("=") * 40);
  Serial.println("  🎵 Spotify Controller Demo Commands");
  Serial.println("=" * 40);
  Serial.println("  play / pause / toggle  - Toggle playback");
  Serial.println("  next                  - Skip to next track");
  Serial.println("  prev / previous       - Go to previous track");
  Serial.println("  save / like           - Toggle saved/liked");
  Serial.println("  vol <0-100>           - Set volume");
  Serial.println("  seek <0-100>          - Seek to percentage");
  Serial.println("  touch <x,y>           - Simulate touch at (x,y)");
  Serial.println("  goto <1-N>            - Jump to track N");
  Serial.println("  info                  - Show current track info");
  Serial.println("  tracks                - List all tracks");
  Serial.println("  help / ?              - Show this help");
  Serial.println("=" * 40 + "\n");
}

// String helper for header
String operator*(String s, int n) {
  String result = "";
  for (int i = 0; i < n; i++) result += s;
  return result;
}
