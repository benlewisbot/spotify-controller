/**
 * @file ST7701SDisplay.cpp
 * @brief ST7701S Display Driver Implementation
 */

#include "ST7701SDisplay.hpp"

ST7701SDisplay::ST7701SDisplay()
    : lgfx(nullptr)
    , initialized(false)
    , powerOn(true)
    , currentBrightness(75)
    , width(SCREEN_WIDTH)
    , height(SCREEN_HEIGHT) {
}

ST7701SDisplay::~ST7701SDisplay() {
    delete lgfx;
}

bool ST7701SDisplay::init() {
    Serial.println("  📺 Initializing ST7701S...");

    lgfx = new LGFX_ST7701S();
    lgfx->init();
    lgfx->setBrightness(currentBrightness);
    lgfx->fillScreen(TFT_BLACK);

    initialized = true;
    Serial.printf("  ✅ ST7701S initialized (%dx%d)\n", width, height);
    return true;
}

void ST7701SDisplay::setOrientation(bool portrait) {
    if (!initialized || !lgfx) return;

    if (portrait) {
        lgfx->setRotation(0);
    } else {
        lgfx->setRotation(1);
    }

    width = lgfx->width();
    height = lgfx->height();
}

void ST7701SDisplay::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    if (lgfx) {
        lgfx->setBrightness(brightness);
    }
}

void ST7701SDisplay::setPower(bool on) {
    powerOn = on;
    if (!lgfx) return;
    
    if (!on) {
        lgfx->setBrightness(0);
    } else {
        lgfx->setBrightness(currentBrightness);
    }
}

void ST7701SDisplay::clear() {
    if (initialized && lgfx) {
        lgfx->fillScreen(TFT_BLACK);
    }
}

bool ST7701SDisplay::readTouch(int16_t& x, int16_t& y) {
    if (!initialized || !lgfx) return false;
    
    uint16_t tx, ty;
    if (lgfx->getTouch(&tx, &ty)) {
        x = tx;
        y = ty;
        return true;
    }
    return false;
}

bool ST7701SDisplay::isTouchPressed() {
    if (!initialized || !lgfx) return false;
    
    uint16_t tx, ty;
    return lgfx->getTouch(&tx, &ty);
}

// GT911 Touch Implementation

GT911Touch::GT911Touch()
    : initialized(false)
    , resolutionX(SCREEN_WIDTH)
    , resolutionY(SCREEN_HEIGHT)
    , display(nullptr) {
}

GT911Touch::~GT911Touch() {
}

bool GT911Touch::init() {
    // Touch is initialized as part of the display
    // The LovyanGFX library handles this internally
    initialized = true;
    return true;
}

bool GT911Touch::read(int16_t& x, int16_t& y) {
    if (!initialized) return false;
    
    // Touch reading is handled by LovyanGFX internally
    // This is called by the LVGL touch callback
    return false;
}

bool GT911Touch::isTouched() {
    if (!initialized) return false;
    return false;
}

void GT911Touch::calibrate() {
    // GT911 typically doesn't require calibration
}

void GT911Touch::getResolution(int16_t& x, int16_t& y) {
    x = resolutionX;
    y = resolutionY;
}
