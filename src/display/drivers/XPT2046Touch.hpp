/**
 * @file XPT2046Touch.hpp
 * @brief XPT2046 Resistive Touch Controller Driver
 *
 * Support for XPT2046 resistive touch controller via SPI.
 * Common on ILI9341 displays with resistive touch overlay.
 */

#ifndef XPT2046_TOUCH_HPP
#define XPT2046_TOUCH_HPP

#include "../Display.hpp"
#include <SPI.h>

// XPT2046 SPI command codes
#define XPT2046_CTRL_X 0x90
#define XPT2046_CTRL_Y 0xD0

// XPT2046 pressure threshold
#define XPT2046_TOUCH_THRESHOLD 300

/**
 * @brief XPT2046 Resistive Touch Controller Implementation
 */
class XPT2046Touch : public TouchInterface {
public:
    XPT2046Touch();
    ~XPT2046Touch() override;

    bool init() override;

    bool read(int16_t& x, int16_t& y) override;

    bool isTouched() override;

    void calibrate() override;

    void getResolution(int16_t& x, int16_t& y) override {
        x = resolutionX;
        y = resolutionY;
    }

    bool isInitialized() const override { return initialized; }

    /**
     * @brief Set SPI pins
     */
    void setPins(int cs, int irq = -1);

    /**
     * @brief Set display resolution for coordinate scaling
     */
    void setResolution(int16_t x, int16_t y) {
        resolutionX = x;
        resolutionY = y;
    }

    /**
     * @brief Swap X and Y coordinates
     */
    void setSwapXY(bool swap) { swapXY = swap; }

    /**
     * @brief Invert coordinates
     */
    void setInvert(bool invertX, bool invertY) {
        invertXCoord = invertX;
        invertYCoord = invertY;
    }

    /**
     * @brief Set SPI speed
     */
    void setSPISpeed(uint32_t speed) { spiSpeed = speed; }

private:
    bool initialized;
    bool swapXY;
    bool invertXCoord;
    bool invertYCoord;

    int16_t resolutionX;
    int16_t resolutionY;

    int csPin;
    int irqPin;
    uint32_t spiSpeed;

    /**
     * @brief Read raw coordinate from SPI
     */
    uint16_t readRaw(uint8_t ctrl);

    /**
     * @brief Read pressure value
     */
    uint16_t readPressure();
};

#endif // XPT2046_TOUCH_HPP
