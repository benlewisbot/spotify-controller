/**
 * @file FT6236Touch.cpp
 * @brief FT6236 Capacitive Touch Controller Implementation
 */

#include "FT6236Touch.hpp"

FT6236Touch::FT6236Touch()
    : initialized(false)
    , swapXY(false)
    , invertXCoord(false)
    , invertYCoord(false)
    , resolutionX(320)
    , resolutionY(480)
    , sdaPin(21)
    , sclPin(22)
    , wire(&Wire) {
}

FT6236Touch::~FT6236Touch() {
}

bool FT6236Touch::init() {
    Serial.println("  🖱  Initializing FT6236 Touch...");

    // Initialize I2C
    wire->begin(sdaPin, sclPin);

    // Check if FT6236 is present
    uint8_t mode = readRegister(FT6236_REG_MODE);
    if (mode == 0xFF || mode == 0x00) {
        Serial.println("  ❌ FT6236 not detected");
        return false;
    }

    Serial.printf("  ✅ FT6236 detected (mode: 0x%02X)\n", mode);

    initialized = true;
    return true;
}

bool FT6236Touch::read(int16_t& x, int16_t& y) {
    if (!initialized) {
        return false;
    }

    // Check if touch detected
    uint8_t touchCount = readRegister(FT6236_REG_TOUCH_COUNT);
    if (touchCount == 0 || touchCount > 2) {
        return false;
    }

    // Read touch coordinates (touch point 1)
    uint8_t xh = readRegister(FT6236_REG_TOUCH1_XH);
    uint8_t xl = readRegister(FT6236_REG_TOUCH1_XL);
    uint8_t yh = readRegister(FT6236_REG_TOUCH1_YH);
    uint8_t yl = readRegister(FT6236_REG_TOUCH1_YL);

    // Parse coordinates
    int16_t rawX = ((xh & 0x0F) << 8) | xl;
    int16_t rawY = ((yh & 0x0F) << 8) | yl;

    // Apply transformations
    if (swapXY) {
        std::swap(rawX, rawY);
    }

    if (invertXCoord) {
        rawX = resolutionX - rawX;
    }

    if (invertYCoord) {
        rawY = resolutionY - rawY;
    }

    // Scale to display resolution
    x = map(rawX, 0, 4095, 0, resolutionX);
    y = map(rawY, 0, 4095, 0, resolutionY);

    return true;
}

bool FT6236Touch::isTouched() {
    if (!initialized) {
        return false;
    }

    uint8_t touchCount = readRegister(FT6236_REG_TOUCH_COUNT);
    return touchCount > 0 && touchCount <= 2;
}

void FT6236Touch::calibrate() {
    Serial.println("  🎯 FT6236 calibration:");
    Serial.println("  (FT6236 typically doesn't need calibration)");
}

void FT6236Touch::setI2CPins(int sda, int scl) {
    sdaPin = sda;
    sclPin = scl;
}

uint8_t FT6236Touch::readRegister(uint8_t reg) {
    wire->beginTransmission(FT6236_I2C_ADDR);
    wire->write(reg);
    wire->endTransmission();

    wire->requestFrom(FT6236_I2C_ADDR, 1);
    if (wire->available()) {
        return wire->read();
    }
    return 0xFF;
}

bool FT6236Touch::writeRegister(uint8_t reg, uint8_t value) {
    wire->beginTransmission(FT6236_I2C_ADDR);
    wire->write(reg);
    wire->write(value);
    return wire->endTransmission() == 0;
}
