/**
 * @file ST7796UDisplay.hpp
 * @brief ST7796U Display Driver (SPI)
 *
 * Support for ST7796U displays via SPI interface.
 * Common resolution: 320x480 or 240x320.
 */

#ifndef ST7796U_DISPLAY_HPP
#define ST7796U_DISPLAY_HPP

#include "../Display.hpp"
#include <TFT_eSPI.h>

/**
 * @brief ST7796U Display Implementation
 */
class ST7796UDisplay : public DisplayInterface {
public:
    ST7796UDisplay();
    ~ST7796UDisplay() override;

    bool init() override;

    int16_t getWidth() const override { return width; }
    int16_t getHeight() const override { return height; }

    void setOrientation(bool portrait) override;

    void setBrightness(uint8_t brightness) override;
    uint8_t getBrightness() const override { return currentBrightness; }

    void setPower(bool on) override;

    void clear() override;

    bool isInitialized() const override { return initialized; }

    const char* getName() const override { return "ST7796U (SPI)"; }

    TFT_eSPI* getTFT() { return tft; }

private:
    TFT_eSPI* tft;
    bool initialized;
    bool powerOn;
    uint8_t currentBrightness;

    int16_t width;
    int16_t height;
};

#endif // ST7796U_DISPLAY_HPP
