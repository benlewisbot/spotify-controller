/**
 * @file ILI9341Display.hpp
 * @brief ILI9341 Display Driver (SPI)
 *
 * Support for ILI9341 displays via SPI interface.
 * Common resolution: 240x320 or 320x240 (rotated).
 */

#ifndef ILI9341_DISPLAY_HPP
#define ILI9341_DISPLAY_HPP

#include "../Display.hpp"
#include <TFT_eSPI.h>

/**
 * @brief ILI9341 Display Implementation
 */
class ILI9341Display : public DisplayInterface {
public:
    ILI9341Display();
    ~ILI9341Display() override;

    bool init() override;

    int16_t getWidth() const override { return width; }
    int16_t getHeight() const override { return height; }

    void setOrientation(bool portrait) override;

    void setBrightness(uint8_t brightness) override;
    uint8_t getBrightness() const override { return currentBrightness; }

    void setPower(bool on) override;

    void clear() override;

    bool isInitialized() const override { return initialized; }

    const char* getName() const override { return "ILI9341 (SPI)"; }

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

    /**
     * @brief Configure SPI pins
     */
    void configurePins();

    /**
     * @brief Send initialization sequence
     */
    void initSequence();
};

#endif // ILI9341_DISPLAY_HPP
