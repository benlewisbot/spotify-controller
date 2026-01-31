#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "display_manager.h"
#include "touch_manager.h"

// Global Objects
TFT_eSPI tft = TFT_eSPI();
DisplayManager displayManager;
TouchManager touchManager;

// Globals für Spotify
String accessToken = "";
String refreshToken = "";
unsigned long lastSpotifyPoll = 0;
unsigned long lastTouchCheck = 0;
bool isPlaying = false;

// Track Info
struct TrackInfo {
  String title = "";
  String artist = "";
  String album = "";
  String coverUrl = "";
  bool isPlaying = false;
  int progress = 0;
  int duration = 0;
  int volume = 50;
} currentTrack;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);

  Serial.println("\n==== Spotify Controller ESP32-S3 ===");

  // 1. Hardware initialisieren
  initHardware();

  // 2. WiFi verbinden
  connectWiFi();

  // 3. Spotify Token prüfen
  checkSpotifyToken();

  // 4. UI initialisieren
  initUI();

  Serial.println("✅ Setup abgeschlossen!");
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

  // 1. Display Manager
  displayManager.init(tft);
  
  // 2. Touch Manager
  touchManager.init();

  Serial.println("✅ Hardware initialisiert");
}

// ===== WIFI =====

void connectWiFi() {
  // WiFi Credentials laden
  File file = LittleFS.open("/config/wifi.json", "r");
  if (!file) {
    Serial.println("⚠️ Keine WiFi Config - Hotspot Mode");
    // TODO: Hotspot starten für Erst-Setup
    return;
  }

  DynamicJsonDocument doc(256);
  deserializeJson(doc, file);
  file.close();

  String ssid = doc["ssid"];
  String password = doc["password"];

  Serial.printf("Verbinde mit %s...\n", ssid.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < WIFI_RETRIES) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi verbunden");
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n❌ WiFi fehlgeschlagen");
  }
}

// ===== SPOTIFY =====

void checkSpotifyToken() {
  File file = LittleFS.open("/config/spotify.json", "r");
  if (!file) {
    Serial.println("⚠️ Kein Spotify Token - OAuth Setup nötig");
    // TODO: OAuth Flow starten
    return;
  }

  DynamicJsonDocument doc(512);
  deserializeJson(doc, file);
  file.close();

  accessToken = doc["access_token"].as<String>();
  refreshToken = doc["refresh_token"].as<String>();

  Serial.println("✅ Spotify Token geladen");
}

void updateNowPlaying() {
  if (accessToken == "") {
    Serial.println("⚠️ Kein Token");
    return;
  }

  // TODO: HTTP GET zu /v1/me/player/currently-playing
  // TODO: JSON parsen und Track Info extrahieren
  // TODO: Cover Image herunterladen wenn URL geändert
  // TODO: UI updaten

  #if DEBUG_SPOTIFY
    Serial.println("🔍 Spotify Status geprüft");
  #endif
}

void handleTrackChange() {
  // TODO: Cover Image herunterladen wenn sich Track ändert
}

// ===== UI =====

void initUI() {
  Serial.println("🖥 UI wird initialisiert...");
  
  // Display Manager initialisieren
  displayManager.setAutoDetect(AUTO_DETECT_DISPLAY);
  
  // Touch Manager initialisieren
  touchManager.init();
  
  // Touch-Controller anpassen falls XPT2046 gefunden wurde
  DisplayConfig currentConfig = displayManager.getCurrentConfig();
  if (currentConfig.touch_int != 255) {
    // Touch-Pin aus Config lesen und an Touch-Manager weitergeben
    touchManager.setTouchPin(currentConfig.touch_int);
  }

  // Initialisieren
  displayManager.init(tft);
  
  Serial.println("✅ UI initialisiert");
  drawNowPlaying();
}

void drawNowPlaying() {
  tft.fillScreen(TFT_BLACK);

  // Platzhalter Cover (quadratisch, zentriert)
  uint16_t coverSize = min(tft.width(), tft.height()) - 40;
  uint16_t coverX = (tft.width() - coverSize) / 2;
  uint16_t coverY = 20;
  
  tft.fillRect(coverX, coverY, coverSize, coverSize, TFT_DARKGREY);

  // Track Info
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, coverY + coverSize + 20);
  tft.println(currentTrack.title);

  tft.setTextSize(1);
  tft.setCursor(10, coverY + coverSize + 50);
  tft.println(currentTrack.artist);

  // Controls
  tft.setTextColor(TFT_BLUE);
  tft.setTextSize(3);
  tft.setCursor(20, coverY + coverSize + 100);
  tft.println("◀  ⏸/▶  ▶");

  // Volume
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, coverY + coverSize + 160);
  tft.printf("Volume: %d%%", currentTrack.volume);
}

void drawProgressBar() {
  // TODO: Fortschrittsbalken zeichnen
}

// ===== TOUCH HANDLING =====

void handleTouch() {
  // Platzhalter für Touch-Handling
  // TODO: XPT2046 Library einbinden
  // TODO: Buttons erkennen (Play, Pause, Next, Previous)
  // TODO: Spotify API Calls senden

  #if DEBUG_DISPLAY
    // Touch debug-logging wenn aktiv
  #endif
}

// ===== SETTINGS SCREEN =====

void drawSettingsScreen() {
  // TODO: Settings Screen implementieren
}

// ===== UTILITIES =====

void restartESP() {
  Serial.println("🔄 Neustart...");
  ESP.restart();
}

void saveWiFiConfig(String ssid, String password) {
  // TODO: WiFi Config speichern
}

void saveSpotifyToken(String access, String refresh) {
  // TODO: Spotify Token speichern
}
