// User_Setup.h - Configure for ESP32 Spotify Controller
//
// This file is included by TFT_eSPI library to configure display drivers.

// Define display type for your hardware
// Uncomment the one you're using:

// ILI9341 (240x320 or 320x240) - Common resistive touch display
#define ILI9341_DRIVER

// ILI9488 (320x480 or 480x320) - Larger displays
// #define ILI9488_DRIVER

// ST7789 (240x240 or 135x240) - LilyGo T-Display S3
// #define ST7789_DRIVER

// ST7796U (320x480 or 240x320) - Cheap Yellow Display
// #define ST7796U_DRIVER

// Display Pin Configuration for ESP32-WROVER
// Adjust these pins for your specific board

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

// Touch Pin Configuration (if your display has touch)
// FT6236 Capacitive Touch (LilyGo T-Display S3 Touch)
#define TOUCH_CS -1  // Not used for I2C touch

// XPT2046 Resistive Touch (common with ILI9341)
// #define TOUCH_CS 16

// Display Orientation
// 0 = Portrait, 1 = Landscape, 2 = Portrait inverted, 3 = Landscape inverted
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH

#define SMOOTH_FONT

// SPI frequency
// Lower for stability, higher for performance
#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000

// DMA support (if available)
// #define SPI_TOUCH_FREQUENCY  2500000
