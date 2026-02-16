/**
 * @file ST7701SDisplay.hpp
 * @brief ST7701S Display Driver for RGB Parallel Interface
 *
 * Support for ST7701S displays via RGB parallel interface on ESP32-S3.
 * Resolution: 480x480 (Guition ESP32-S3-4848S040)
 */

#ifndef ST7701S_DISPLAY_HPP
#define ST7701S_DISPLAY_HPP

#include "../Display.hpp"
#include <LovyanGFX.hpp>

// Include ESP32-S3 specific LovyanGFX components
#ifdef ESP32
  #include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
  #include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#endif

class LGFX_ST7701S : public lgfx::LGFX_Device {
public:
    lgfx::Bus_RGB _bus_instance;
    lgfx::Panel_RGB _panel_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;

    LGFX_ST7701S(void) {
        // RGB bus configuration for ESP32-S3
        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;
            
            // Data pins
            cfg.pin_d0 = GPIO_NUM_4;   // B0
            cfg.pin_d1 = GPIO_NUM_5;   // B1
            cfg.pin_d2 = GPIO_NUM_6;   // B2
            cfg.pin_d3 = GPIO_NUM_7;   // B3
            cfg.pin_d4 = GPIO_NUM_15;  // B4
            cfg.pin_d5 = GPIO_NUM_8;   // G0
            cfg.pin_d6 = GPIO_NUM_20;  // G1
            cfg.pin_d7 = GPIO_NUM_3;   // G2
            cfg.pin_d8 = GPIO_NUM_46;  // G3
            cfg.pin_d9 = GPIO_NUM_9;   // G4
            cfg.pin_d10 = GPIO_NUM_10; // G5
            cfg.pin_d11 = GPIO_NUM_11; // R0
            cfg.pin_d12 = GPIO_NUM_12; // R1
            cfg.pin_d13 = GPIO_NUM_13; // R2
            cfg.pin_d14 = GPIO_NUM_14; // R3
            cfg.pin_d15 = GPIO_NUM_0;  // R4
            
            cfg.pin_henable = GPIO_NUM_18;  // DE
            cfg.pin_vsync = GPIO_NUM_17;    // VSYNC
            cfg.pin_hsync = GPIO_NUM_16;    // HSYNC
            cfg.pin_pclk = GPIO_NUM_21;     // PCLK
            cfg.freq_write = 16000000;      // 16MHz
            
            cfg.hsync_polarity = 0;
            cfg.hsync_front_porch = 10;
            cfg.hsync_pulse_width = 8;
            cfg.hsync_back_porch = 50;
            
            cfg.vsync_polarity = 0;
            cfg.vsync_front_porch = 10;
            cfg.vsync_pulse_width = 8;
            cfg.vsync_back_porch = 20;
            
            cfg.pclk_active_neg = 1;
            cfg.de_idle_high = 0;
            cfg.pclk_idle_high = 0;
            
            _bus_instance.config(cfg);
        }
        
        // Panel configuration
        {
            auto cfg = _panel_instance.config();
            cfg.memory_width = SCREEN_WIDTH;
            cfg.memory_height = SCREEN_HEIGHT;
            cfg.panel_width = SCREEN_WIDTH;
            cfg.panel_height = SCREEN_HEIGHT;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = false;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            
            _panel_instance.config(cfg);
        }
        
        // Backlight configuration
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = TFT_BL;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;
            
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        
        // Touch configuration (GT911)
        {
            auto cfg = _touch_instance.config();
            cfg.x_min = 0;
            cfg.x_max = SCREEN_WIDTH;
            cfg.y_min = 0;
            cfg.y_max = SCREEN_HEIGHT;
            cfg.pin_int = -1;
            cfg.pin_rst = -1;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;
            
            cfg.i2c_port = 1;
            cfg.pin_sda = TOUCH_SDA;
            cfg.pin_scl = TOUCH_SCL;
            cfg.freq = 400000;
            cfg.i2c_addr = 0x5D;
            
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        
        setPanel(&_panel_instance);
    }
};

/**
 * @brief ST7701S Display Implementation for Guition ESP32-S3-4848S040
 */
class ST7701SDisplay : public DisplayInterface {
public:
    ST7701SDisplay();
    ~ST7701SDisplay() override;

    bool init() override;

    int16_t getWidth() const override { return width; }
    int16_t getHeight() const override { return height; }

    void setOrientation(bool portrait) override;

    void setBrightness(uint8_t brightness) override;
    uint8_t getBrightness() const override { return currentBrightness; }

    void setPower(bool on) override;

    void clear() override;

    bool isInitialized() const override { return initialized; }

    const char* getName() const override { return "ST7701S (RGB Parallel)"; }

    LGFX_ST7701S* getLGFX() { return lgfx; }
    
    bool readTouch(int16_t& x, int16_t& y);
    bool isTouchPressed();

private:
    LGFX_ST7701S* lgfx;
    bool initialized;
    bool powerOn;
    uint8_t currentBrightness;

    int16_t width;
    int16_t height;
};

/**
 * @brief GT911 Touch Implementation
 */
class GT911Touch : public TouchInterface {
public:
    GT911Touch();
    ~GT911Touch() override;

    bool init() override;
    bool read(int16_t& x, int16_t& y) override;
    bool isTouched() override;
    void calibrate() override;
    void getResolution(int16_t& x, int16_t& y) override;
    bool isInitialized() const override { return initialized; }

private:
    bool initialized;
    int16_t resolutionX;
    int16_t resolutionY;
    ST7701SDisplay* display;
};

#endif // ST7701S_DISPLAY_HPP
