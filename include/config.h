#ifndef CONFIG_H
#define CONFIG_H

// ===== DISPLAY-KONFIGURATION (AUSTAUSCHBAR) =====
// Ändere DISPLAY_TYPE für dein Display
// Alle Displays werden automatisch erkannt und initialisiert

// Display-Typ Auswahl
// ILI9341_S3, ILI9488_S3, ST7789_S3 für LilyBoards mit Touch
// CYD_2_4_ST7789, CYD_2_8_ST7796U, CYD_3_5_ST7796U für Cheap Yellow Displays
#define DISPLAY_TYPE ILI9488_S3  // LilyGo T-Display S3 Touch = ESP32-4848S040

// ============================ ESP32-4848S040 (LilyGo T-Display S3 Touch) ============================
#if DISPLAY_TYPE == ILI9488_S3
  #define DISPLAY_NAME "LilyGo T-Display S3 Touch (ESP32-4848S040)"
  #define DISPLAY_DRIVER ILI9488
  #define DISPLAY_WIDTH 320
  #define DISPLAY_HEIGHT 480
  #define TOUCH_CONTROLLER FT6236

  // Display-Pins (LilyGo T-Display S3 Standard)
  #define TFT_MISO  12
  #define TFT_MOSI  11
  #define TFT_SCLK  13
  #define TFT_CS   38
  #define TFT_DC   40
  #define TFT_RST  39

  // Touch-Pins
  #define TOUCH_INT  18
  #define TOUCH_SDA  8
  #define TOUCH_SCL  48

// ============================ LilyGo T-Display-S3 Capacitive Touch (ST7789) ============================
#elif DISPLAY_TYPE == ST7789_S3
  #define DISPLAY_NAME "LilyGo T-Display-S3 Capacitive"
  #define DISPLAY_DRIVER ST7789
  #define DISPLAY_WIDTH 320
  #define DISPLAY_HEIGHT 170
  #define TOUCH_CONTROLLER FT6236

  #define TFT_MISO  12
  #define TFT_MOSI  11
  #define TFT_SCLK  13
  #define TFT_CS   6
  #define TFT_DC   7
  #define TFT_RST  5

  #define TOUCH_INT  21
  #define TOUCH_SDA  39
  #define TOUCH_SCL  40

// ============================ Cheap Yellow Display CYD 2.4" (ST7789) ============================
#elif DISPLAY_TYPE == CYD_2_4_ST7789
  #define DISPLAY_NAME "Cheap Yellow Display CYD 2.4\" (ST7789)"
  #define DISPLAY_DRIVER ST7789
  #define DISPLAY_WIDTH 240
  #define DISPLAY_HEIGHT 320
  #define TOUCH_CONTROLLER FT6236

  // Standard Pinout für CYD 2.4"
  #define TFT_MISO  5
  #define TFT_MOSI  7
  #define TFT_SCLK  6
  #define TFT_CS   10
  #define TFT_DC   8
  #define TFT_RST  9

  // Touch-Pins
  #define TOUCH_INT  16

// ============================ Cheap Yellow Display CYD 2.8" (ST7796U) ============================
#elif DISPLAY_TYPE == CYD_2_8_ST7796U
  #define DISPLAY_NAME "Cheap Yellow Display CYD 2.8\" (ST7796U)"
  #define DISPLAY_DRIVER ST7796U
  #define DISPLAY_WIDTH 240
  #define DISPLAY_HEIGHT 320
  #define TOUCH_CONTROLLER FT6236

  // Standard Pinout für CYD 2.8"
  #define TFT_MISO  5
  #define TFT_MOSI  7
  #define TFT_SCLK  6
  #define TFT_CS   10
  #define TFT_DC   8
  #define TFT_RST  9

  // Touch-Pins
  #define TOUCH_INT  16

// ============================ Cheap Yellow Display CYD 3.5" (ST7796U) ============================
#elif DISPLAY_TYPE == CYD_3_5_ST7796U
  #define DISPLAY_NAME "Cheap Yellow Display CYD 3.5\" (ST7796U)"
  #define DISPLAY_DRIVER ST7796U
  #define DISPLAY_WIDTH 320
  #define DISPLAY_HEIGHT 480
  #define TOUCH_CONTROLLER FT6236

  // Standard Pinout für CYD 3.5"
  #define TFT_MISO  5
  #define TFT_MOSI  7
  #define TFT_SCLK  6
  #define TFT_CS   10
  #define TFT_DC   8
  #define TFT_RST  9

  // Touch-Pins
  #define TOUCH_INT  16

// ============================ 7 Inch ESP32-S3 Serial Screen (800x480) ============================
#elif DISPLAY_TYPE == CYD_7_INCH
  #define DISPLAY_NAME "7 Inch ESP32-S3 Serial Screen (800x480)"
  #define DISPLAY_DRIVER ST7796U
  #define DISPLAY_WIDTH 800
  #define DISPLAY_HEIGHT 480
  #define TOUCH_CONTROLLER FT6236

  // UART Serial Screen Pinout
  // Serial Display nutzt UART statt SPI
  #define TFT_RX  16
  #define TFT_TX  17
  #define TFT_BAUD 115200

  // Touch-Pins (UART)
  #define TOUCH_INT  4
  #define TOUCH_RX  5
  #define TOUCH_TX  18

// ============================ Standard ILI9341 2.4" ============================
#elif DISPLAY_TYPE == ILI9341_S3
  #define DISPLAY_NAME "ILI9341 2.4\" (Standard)"
  #define DISPLAY_DRIVER ILI9341
  #define DISPLAY_WIDTH 240
  #define DISPLAY_HEIGHT 320
  #define TOUCH_CONTROLLER XPT2046

  #define TFT_MISO  13
  #define TFT_MOSI  11
  #define TFT_SCLK  12
  #define TFT_CS   15
  #define TFT_DC   14
  #define TFT_RST  26

  #define TOUCH_INT  16

// ============================ DIY ESP32 + externes Display ============================
#elif DISPLAY_TYPE == DIY_ILI9341
  #define DISPLAY_NAME "DIY ESP32 + ILI9341 (Custom Pins)"
  #define DISPLAY_DRIVER ILI9341
  #define DISPLAY_WIDTH 240
  #define DISPLAY_HEIGHT 320
  #define TOUCH_CONTROLLER XPT2046

  // Hier deine Pins eintragen
  #define TFT_MISO  19
  #define TFT_MOSI  23
  #define TFT_SCLK  18
  #define TFT_CS   5
  #define TFT_DC   4
  #define TFT_RST  33

  #define TOUCH_INT  16

#else
  #error "Unbekannter DISPLAY_TYPE! Wähle einen aus: ILI9488_S3, ST7789_S3, ILI9341_S3, CYD_2_4_ST7789, CYD_2_8_ST7796U, CYD_3_5_ST7796U, CYD_7_INCH, DIY_ILI9341"
#endif

// ===== SPI-KONFIGURATION =====
#define SPI_FREQUENCY 40000000  // 40 MHz (ESP32-S3 Standard)

// ===== DISPLAY-ORIENTATION =====
#define DISPLAY_ORIENTATION_PORTRAIT  1
#define DISPLAY_ORIENTATION_LANDSCAPE  0
#define DISPLAY_ORIENTATION_DEFAULT DISPLAY_ORIENTATION_PORTRAIT

// ===== SPOTIFY API =====
#define SPOTIFY_API_POLL_MS 3000     // Alle 3s Status prüfen
#define SPOTIFY_RATE_LIMIT_MS 1000    // Min 1s zwischen Requests

// ===== FILESYSTEM =====
#define MAX_CACHE_SIZE_MB 50         // 50MB Max für Cover Cache

// ===== WIFI =====
#define WIFI_TIMEOUT 30000          // 30s
#define WIFI_RETRIES 5

// ===== PERFORMANCE =====
#define DEFAULT_BRIGHTNESS 70       // 0-100%
#define SCREENSAVER_TIMEOUT_MIN 10   // 10min
#define VOLUME_MAX 100
#define VOLUME_LIMIT_DEFAULT 80

// ===== DEBUG =====
#define SERIAL_BAUD 115200
#define DEBUG_MODE true
#define DEBUG_DISPLAY false
#define DEBUG_SPOTIFY false
#define DEBUG_WIFI false
#define DEBUG_TOUCH false

// ===== FEATURES =====
#define FEATURE_PLAYLIST_BROWSER true
#define FEATURE_SEARCH false          // In späterer Version
#define FEATURE_SCREENSAVER true
#define FEATURE_VOLUME_SLIDER true
#define FEATURE_PROGRESS_BAR true
#define FEATURE_TOUCH_CONTROLS true

// ===== AUTO-DETECTION =====
#define AUTO_DETECT_DISPLAY false     // Auf false für ESP32-4848S040

#endif // CONFIG_H
