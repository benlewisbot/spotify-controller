/**
 * config.h - Konfiguration für Spotify Controller
 * LilyGo T-Display S3 Touch (ESP32-4848S040C)
 */

#ifndef CONFIG_H
#define CONFIG_H

// Display Konfiguration
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 480
#define TOUCH_I2C_ADDR 0x38

// Spotify API Konfiguration
#define SPOTIFY_CLIENT_ID "" // Wird bei Setup erstellt
#define SPOTIFY_CLIENT_SECRET "" // Wird bei Setup erstellt
#define SPOTIFY_REDIRECT_URI "http://spotify.local/callback"
#define SPOTIFY_AUTH_URL "https://accounts.spotify.com/authorize"
#define SPOTIFY_TOKEN_URL "https://accounts.spotify.com/api/token"
#define SPOTIFY_API_BASE "https://api.spotify.com/v1"

// WiFi Konfiguration
#define WIFI_SSID "" // Wird bei Setup erstellt
#define WIFI_PASSWORD "" // Wird bei Setup erstellt
#define WIFI_TIMEOUT 10000 // ms

// LittleFS Pfade
#define CREDENTIALS_FILE "/credentials.json"
#define TOKEN_FILE "/token.json"
#define SETTINGS_FILE "/settings.json"

// Web Server
#define WEB_PORT 80
#define HOTSPOT_SSID "SpotifyController"
#define HOTSPOT_PASSWORD "spotify123"
#define HOTSPOT_IP IPAddress(192, 168, 4, 1)
#define HOTSPOT_GATEWAY IPAddress(192, 168, 4, 1)
#define HOTSPOT_SUBNET IPAddress(255, 255, 255, 0)

// Polling Intervalle
#define PLAYER_POLL_INTERVAL 1000 // ms
#define TOKEN_REFRESH_CHECK 300000 // ms (5 min)

// GUI Farben (Spotify Dark Theme)
#define COLOR_SPOTIFY_BLACK 0x121212
#define COLOR_SPOTIFY_DARK 0x181818
#define COLOR_SPOTIFY_GRAY 0x282828
#define COLOR_SPOTIFY_GREEN 0x1DB954
#define COLOR_SPOTIFY_WHITE 0xFFFFFF
#define COLOR_SPOTIFY_TEXT 0xB3B3B3

// Touch Thresholds
#define TOUCH_THRESHOLD 30
#define SWIPE_THRESHOLD 50

#endif // CONFIG_H
