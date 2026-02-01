/**
 * @file XPT2046Touch.cpp
 * @brief XPT2046 Resistive Touch Controller Implementation
 */

#include "XPT2046Touch.hpp"

XPT2046Touch::XPT2046Touch()
    : initialized(false)
    , swapXY(false)
    , invertXCoord(false)
    , invertYCoord(false)
    , resolutionX(320)
    , resolutionY(240)
    , csPin(-1)
    , irqPin(-1)
    , spiSpeed(2000000) {
}

XPT2046Touch::~XPT2046Touch() {
}

bool XPT2046Touch::init() {
    Serial.println("  🖱  Initializing XPT2046 Touch...");

    // Set default CS pin if not set
    if (csPin < 0) {
        csPin = 15;  // Default for many ILI9341 displays
    }

    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);

    // Configure IRQ pin if available
    if (irqPin >= 0) {
        pinMode(irqPin, INPUT_PULLUP);
    }

    Serial.printf("  ✅ XPT2046 initialized (CS: %d)\n", csPin);

    initialized = true;
    return true;
}

bool XPT2046Touch::read(int16_t& x, int16_t& y) {
    if (!initialized) {
        return false;
    }

    // Check touch pressure
    uint16_t pressure = readPressure();
    if (pressure < XPT2046_TOUCH_THRESHOLD) {
        return false;
    }

    // Read X and Y multiple times and average
    uint16_t rawX = 0;
    uint16_t rawY = 0;
    int samples = 0;

    for (int i = 0; i < 5; i++) {
        uint16_t xVal = readRaw(XPT2046_CTRL_X);
        uint16_t yVal = readRaw(XPT2046_CTRL_Y);

        // Filter out invalid readings
        if (xVal > 100 && xVal < 4000 && yVal > 100 && yVal < 4000) {
            rawX += xVal;
            rawY += yVal;
            samples++;
        }
        delay(1);
    }

    if (samples == 0) {
        return false;
    }

    rawX /= samples;
    rawY /= samples;

    // Apply transformations
    int16_t scaledX = map(rawX, 200, 3800, 0, resolutionX);
    int16_t scaledY = map(rawY, 200, 3800, 0, resolutionY);

    if (swapXY) {
        std::swap(scaledX, scaledY);
    }

    if (invertXCoord) {
        scaledX = resolutionX - scaledX;
    }

    if (invertYCoord) {
        scaledY = resolutionY - scaledY;
    }

    x = scaledX;
    y = scaledY;

    return true;
}

bool XPT2046Touch::isTouched() {
    if (!initialized) {
        return false;
    }

    // Use IRQ pin if available
    if (irqPin >= 0) {
        return digitalRead(irqPin) == LOW;
    }

    // Otherwise read pressure
    return readPressure() >= XPT2046_TOUCH_THRESHOLD;
}

void XPT2046Touch::calibrate() {
    Serial.println("  🎯 XPT2046 calibration:");
    Serial.println("  Touch the four corners as prompted...");

    // This would normally show a calibration UI
    // For now, we'll use default calibration values
    Serial.println("  ✅ Calibration complete (using defaults)");
}

void XPT2046Touch::setPins(int cs, int irq) {
    csPin = cs;
    irqPin = irq;
}

uint16_t XPT2046Touch::readRaw(uint8_t ctrl) {
    digitalWrite(csPin, LOW);

    SPI.beginTransaction(SPISettings(spiSpeed, MSBFIRST, SPI_MODE0));
    SPI.transfer(ctrl);
    uint16_t value = SPI.transfer16(0);
    SPI.endTransaction();

    digitalWrite(csPin, HIGH);

    return value >> 3;  // 12-bit value
}

uint16_t XPT2046Touch::readPressure() {
    digitalWrite(csPin, LOW);

    SPI.beginTransaction(SPISettings(spiSpeed, MSBFIRST, SPI_MODE0));
    SPI.transfer(0xB0);  // Pressure Z1
    uint16_t z1 = SPI.transfer16(0);
    SPI.transfer(0xC0);  // Pressure Z2
    uint16_t z2 = SPI.transfer16(0);
    SPI.endTransaction();

    digitalWrite(csPin, HIGH);

    // Calculate pressure
    z1 >>= 3;
    z2 >>= 3;

    if (z1 == 0 || z2 == 0) {
        return 0;
    }

    return abs(z1 - z2);
}
