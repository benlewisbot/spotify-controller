/**
 * @file FT6236Touch.hpp
 * @brief FT6236 Capacitive Touch Controller Driver
 *
 * Support for FT6236 capacitive touch controller via I2C.
 * Common on LilyGo T-Display S3 and similar boards.
 */

#ifndef FT6236_TOUCH_HPP
#define FT6236_TOUCH_HPP

#include "../Display.hpp"
#include <Wire.h>

// FT6236 I2C address
#define FT6236_I2C_ADDR 0x38

// FT6236 registers
#define FT6236_REG_MODE 0x00
#define FT6236_REG_TOUCH1_XH 0x03
#define FT6236_REG_TOUCH1_XL 0x04
#define FT6236_REG_TOUCH1_YH 0x05
#define FT6236_REG_TOUCH1_YL 0x06
#define FT6236_REG_TOUCH_COUNT 0x02

/**
 * @brief FT6236 Capacitive Touch Controller Implementation
 */
class FT6236Touch : public TouchInterface {
public:
    FT6236Touch();
    ~FT6236Touch() override;

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
     * @brief Set I2C pins (for custom pinout)
     */
    void setI2CPins(int sda, int scl);

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

private:
    bool initialized;
    bool swapXY;
    bool invertXCoord;
    bool invertYCoord;

    int16_t resolutionX;
    int16_t resolutionY;

    int sdaPin;
    int sclPin;
    TwoWire* wire;

    /**
     * @brief Read a byte from FT6236 register
     */
    uint8_t readRegister(uint8_t reg);

    /**
     * @brief Write a byte to FT6236 register
     */
    bool writeRegister(uint8_t reg, uint8_t value);
};

#endif // FT6236_TOUCH_HPP
