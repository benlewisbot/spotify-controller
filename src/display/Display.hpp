/**
 * @file Display.hpp
 * @brief Display Interface Abstraction
 *
 * Abstract interface for different display types.
 * Implementations provide support for various SPI/I2C displays.
 */

#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <Arduino.h>

/**
 * @brief Abstract Display Interface
 *
 * All display drivers must implement this interface.
 * This allows easy swapping of display hardware.
 */
class DisplayInterface {
public:
    virtual ~DisplayInterface() = default;

    /**
     * @brief Initialize the display
     * @return true if successful, false otherwise
     */
    virtual bool init() = 0;

    /**
     * @brief Get display width in pixels
     */
    virtual int16_t getWidth() const = 0;

    /**
     * @brief Get display height in pixels
     */
    virtual int16_t getHeight() const = 0;

    /**
     * @brief Set display orientation
     * @param portrait true for portrait, false for landscape
     */
    virtual void setOrientation(bool portrait) = 0;

    /**
     * @brief Set brightness (0-100%)
     */
    virtual void setBrightness(uint8_t brightness) = 0;

    /**
     * @brief Get current brightness (0-100%)
     */
    virtual uint8_t getBrightness() const = 0;

    /**
     * @brief Turn display on/off
     */
    virtual void setPower(bool on) = 0;

    /**
     * @brief Clear the display to black
     */
    virtual void clear() = 0;

    /**
     * @brief Check if display is initialized
     */
    virtual bool isInitialized() const = 0;

    /**
     * @brief Flush the display buffer (if using buffered mode)
     */
    virtual void flush() {}

    /**
     * @brief Get display name/description
     */
    virtual const char* getName() const = 0;
};

/**
 * @brief Touch Input Interface
 *
 * Abstract interface for touch controllers.
 */
class TouchInterface {
public:
    virtual ~TouchInterface() = default;

    /**
     * @brief Initialize the touch controller
     * @return true if successful, false otherwise
     */
    virtual bool init() = 0;

    /**
     * @brief Read touch input
     * @param x Output x coordinate
     * @param y Output y coordinate
     * @return true if touch detected, false otherwise
     */
    virtual bool read(int16_t& x, int16_t& y) = 0;

    /**
     * @brief Check if touch is currently pressed
     */
    virtual bool isTouched() = 0;

    /**
     * @brief Calibrate touch
     */
    virtual void calibrate() {}

    /**
     * @brief Get touch resolution
     */
    virtual void getResolution(int16_t& x, int16_t& y) = 0;

    /**
     * @brief Check if touch is initialized
     */
    virtual bool isInitialized() const = 0;
};

#endif // DISPLAY_HPP
