/**
 * @file ST7701SDisplay.cpp
 * @brief ST7701S Display Driver Implementation
 */

#include "ST7701SDisplay.hpp"
#include <esp_log.h>

// Global pointer for GT911Touch to access the display's LovyanGFX touch
static ST7701SDisplay* g_st7701sInstance = nullptr;

ST7701SDisplay::ST7701SDisplay()
    : lgfx(nullptr)
    , initialized(false)
    , powerOn(true)
    , currentBrightness(75)
    , width(SCREEN_WIDTH)
    , height(SCREEN_HEIGHT) {
}

ST7701SDisplay::~ST7701SDisplay() {
    g_st7701sInstance = nullptr;
    delete lgfx;
}

bool ST7701SDisplay::init() {
    ESP_LOGI("ST7701S", "Initializing display...");

    // Create LovyanGFX with Panel_ST7701_guition_esp32_4848S040
    // This panel type has built-in SPI init commands for the Guition board
    lgfx = new LGFX_ST7701S();
    lgfx->init();

    // Backlight
    lgfx->setBrightness(currentBrightness);
    // Force backlight GPIO HIGH as fallback (PWM may not work on all units)
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Clear screen
    lgfx->fillScreen(TFT_BLACK);

    // Store global pointer for touch access
    g_st7701sInstance = this;

    initialized = true;
    ESP_LOGI("ST7701S", "init complete (%dx%d)", width, height);
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

// GT911 Touch Implementation - delegates to ST7701SDisplay's LovyanGFX

GT911Touch::GT911Touch()
    : initialized(false)
    , resolutionX(SCREEN_WIDTH)
    , resolutionY(SCREEN_HEIGHT)
    , display(nullptr) {
}

GT911Touch::~GT911Touch() {
}

bool GT911Touch::init() {
    // Touch is initialized as part of the LovyanGFX display
    initialized = true;
    return true;
}

bool GT911Touch::read(int16_t& x, int16_t& y) {
    if (!initialized) return false;
    
    // Delegate to the global ST7701SDisplay instance which has the LovyanGFX object
    if (g_st7701sInstance) {
        return g_st7701sInstance->readTouch(x, y);
    }
    return false;
}

bool GT911Touch::isTouched() {
    if (!initialized) return false;
    
    if (g_st7701sInstance) {
        return g_st7701sInstance->isTouchPressed();
    }
    return false;
}

void GT911Touch::calibrate() {
    // GT911 typically doesn't require calibration
}

void GT911Touch::getResolution(int16_t& x, int16_t& y) {
    x = resolutionX;
    y = resolutionY;
}
