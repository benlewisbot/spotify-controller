/**
 * @file ILI9341Display.cpp
 * @brief ILI9341 Display Driver Implementation
 */

#include "ILI9341Display.hpp"

// Default pin configuration for ILI9341 (can be overridden in User_Setup.h)
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_SCLK 13
#define TFT_CS   15
#define TFT_DC   4
#define TFT_RST  33

ILI9341Display::ILI9341Display()
    : tft(new TFT_eSPI())
    , initialized(false)
    , powerOn(true)
    , currentBrightness(100)
    , width(240)
    , height(320) {
}

ILI9341Display::~ILI9341Display() {
    delete tft;
}

bool ILI9341Display::init() {
    Serial.println("  📺 Initializing ILI9341...");

    // Initialize TFT
    tft->begin();
    tft->setRotation(1);  // Landscape by default

    // Get actual dimensions after rotation
    width = tft->width();
    height = tft->height();

    // Clear screen
    tft->fillScreen(TFT_BLACK);

    initialized = true;
    Serial.printf("  ✅ ILI9341 initialized (%dx%d)\n", width, height);
    return true;
}

void ILI9341Display::setOrientation(bool portrait) {
    if (!initialized) return;

    // ILI9341 rotation values:
    // 0 = Portrait, 1 = Landscape, 2 = Portrait (inverted), 3 = Landscape (inverted)
    tft->setRotation(portrait ? 0 : 1);

    // Update dimensions
    width = tft->width();
    height = tft->height();
}

void ILI9341Display::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    // Note: ILI9341 doesn't have hardware brightness control
    // This can be emulated via PWM on backlight pin if available
}

void ILI9341Display::setPower(bool on) {
    powerOn = on;
    if (!on) {
        // Send sleep command
        tft->writecommand(0x10);  // Enter Sleep Mode
    } else {
        // Wake up
        tft->writecommand(0x11);  // Exit Sleep Mode
        delay(120);  // Wait for wake up
    }
}

void ILI9341Display::clear() {
    if (initialized && tft) {
        tft->fillScreen(TFT_BLACK);
    }
}

void ILI9341Display::configurePins() {
    // Pins are configured in TFT_eSPI User_Setup.h
    // This is a placeholder for future pin reconfiguration
}

void ILI9341Display::initSequence() {
    // ILI9341 initialization sequence is handled by TFT_eSPI library
    // This is a placeholder for custom initialization if needed
}
