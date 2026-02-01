/**
 * @file ST7789Display.cpp
 * @brief ST7789 Display Driver Implementation
 */

#include "ST7789Display.hpp"

ST7789Display::ST7789Display()
    : tft(new TFT_eSPI())
    , initialized(false)
    , powerOn(true)
    , currentBrightness(100)
    , width(240)
    , height(240) {
}

ST7789Display::~ST7789Display() {
    delete tft;
}

bool ST7789Display::init() {
    Serial.println("  📺 Initializing ST7789...");

    tft->begin();
    tft->setRotation(1);  // Landscape by default

    // Get actual dimensions after rotation
    width = tft->width();
    height = tft->height();

    tft->fillScreen(TFT_BLACK);

    initialized = true;
    Serial.printf("  ✅ ST7789 initialized (%dx%d)\n", width, height);
    return true;
}

void ST7789Display::setOrientation(bool portrait) {
    if (!initialized) return;

    tft->setRotation(portrait ? 0 : 1);

    width = tft->width();
    height = tft->height();
}

void ST7789Display::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
}

void ST7789Display::setPower(bool on) {
    powerOn = on;
    if (!on) {
        tft->writecommand(0x10);
    } else {
        tft->writecommand(0x11);
        delay(120);
    }
}

void ST7789Display::clear() {
    if (initialized && tft) {
        tft->fillScreen(TFT_BLACK);
    }
}
