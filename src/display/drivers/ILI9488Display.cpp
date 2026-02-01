/**
 * @file ILI9488Display.cpp
 * @brief ILI9488 Display Driver Implementation
 */

#include "ILI9488Display.hpp"

ILI9488Display::ILI9488Display()
    : tft(new TFT_eSPI())
    , initialized(false)
    , powerOn(true)
    , currentBrightness(100)
    , width(320)
    , height(480) {
}

ILI9488Display::~ILI9488Display() {
    delete tft;
}

bool ILI9488Display::init() {
    Serial.println("  📺 Initializing ILI9488...");

    // Initialize TFT
    tft->begin();
    tft->setRotation(1);  // Landscape by default

    // Get actual dimensions after rotation
    width = tft->width();
    height = tft->height();

    // Clear screen
    tft->fillScreen(TFT_BLACK);

    initialized = true;
    Serial.printf("  ✅ ILI9488 initialized (%dx%d)\n", width, height);
    return true;
}

void ILI9488Display::setOrientation(bool portrait) {
    if (!initialized) return;

    tft->setRotation(portrait ? 0 : 1);

    // Update dimensions
    width = tft->width();
    height = tft->height();
}

void ILI9488Display::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    // Note: ILI9488 doesn't have hardware brightness control
}

void ILI9488Display::setPower(bool on) {
    powerOn = on;
    if (!on) {
        tft->writecommand(0x10);  // Enter Sleep Mode
    } else {
        tft->writecommand(0x11);  // Exit Sleep Mode
        delay(120);
    }
}

void ILI9488Display::clear() {
    if (initialized && tft) {
        tft->fillScreen(TFT_BLACK);
    }
}
