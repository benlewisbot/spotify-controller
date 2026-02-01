/**
 * @file ST7789Display.hpp
 * @brief ST7789 Display Driver (SPI)
 *
 * Support for ST7789 displays via SPI interface.
 * Common resolution: 240x240 (square) or 135x240 (rectangle).
 */

#ifndef ST7789_DISPLAY_HPP
#define ST7789_DISPLAY_HPP

#include "../Display.hpp"
#include <TFT_eSPI.h>

/**
 * @brief ST7789 Display Implementation
 */
class ST7789Display : public DisplayInterface {
public:
    ST7789Display();
    ~ST7789Display() override;

    bool init() override;

    int16_t getWidth() const override { return width; }
    int16_t getHeight() const override { return height; }

    void setOrientation(bool portrait) override;

    void setBrightness(uint8_t brightness) override;
    uint8_t getBrightness() const override { return currentBrightness; }

    void setPower(bool on) override;

    void clear() override;

    bool isInitialized() const override { return initialized; }

    const char* getName() const override { return "ST7789 (SPI)"; }

    TFT_eSPI* getTFT() { return tft; }

private:
    TFT_eSPI* tft;
    bool initialized;
    bool powerOn;
    uint8_t currentBrightness;

    int16_t width;
    int16_t height;
};

#endif // ST7789_DISPLAY_HPP
