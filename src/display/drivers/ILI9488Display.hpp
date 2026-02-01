/**
 * @file ILI9488Display.hpp
 * @brief ILI9488 Display Driver (SPI)
 *
 * Support for ILI9488 displays via SPI interface.
 * Common resolution: 320x480 or 480x320.
 */

#ifndef ILI9488_DISPLAY_HPP
#define ILI9488_DISPLAY_HPP

#include "../Display.hpp"
#include <TFT_eSPI.h>

/**
 * @brief ILI9488 Display Implementation
 */
class ILI9488Display : public DisplayInterface {
public:
    ILI9488Display();
    ~ILI9488Display() override;

    bool init() override;

    int16_t getWidth() const override { return width; }
    int16_t getHeight() const override { return height; }

    void setOrientation(bool portrait) override;

    void setBrightness(uint8_t brightness) override;
    uint8_t getBrightness() const override { return currentBrightness; }

    void setPower(bool on) override;

    void clear() override;

    bool isInitialized() const override { return initialized; }

    const char* getName() const override { return "ILI9488 (SPI)"; }

    /**
     * @brief Get the underlying TFT_eSPI instance
     */
    TFT_eSPI* getTFT() { return tft; }

private:
    TFT_eSPI* tft;
    bool initialized;
    bool powerOn;
    uint8_t currentBrightness;

    int16_t width;
    int16_t height;
};

#endif // ILI9488_DISPLAY_HPP
