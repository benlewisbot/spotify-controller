/**
 * @file ST7796UDisplay.cpp
 * @brief ST7796U Display Driver Implementation
 */

#include "ST7796UDisplay.hpp"

ST7796UDisplay::ST7796UDisplay()
    : tft(new TFT_eSPI())
    , initialized(false)
    , powerOn(true)
    , currentBrightness(100)
    , width(320)
    , height(480) {
}

ST7796UDisplay::~ST7796UDisplay() {
    delete tft;
}

bool ST7796UDisplay::init() {
    Serial.println("  📺 Initializing ST7796U...");

    tft->begin();
    tft->setRotation(1);  // Landscape by default

    width = tft->width();
    height = tft->height();

    tft->fillScreen(TFT_BLACK);

    initialized = true;
    Serial.printf("  ✅ ST7796U initialized (%dx%d)\n", width, height);
    return true;
}

void ST7796UDisplay::setOrientation(bool portrait) {
    if (!initialized) return;

    tft->setRotation(portrait ? 0 : 1);

    width = tft->width();
    height = tft->height();
}

void ST7796UDisplay::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
}

void ST7796UDisplay::setPower(bool on) {
    powerOn = on;
    if (!on) {
        tft->writecommand(0x10);
    } else {
        tft->writecommand(0x11);
        delay(120);
    }
}

void ST7796UDisplay::clear() {
    if (initialized && tft) {
        tft->fillScreen(TFT_BLACK);
    }
}
