# Guition ESP32-S3-4848S040 Display Reference

> Definitive hardware configuration for the 480x480 square touchscreen board.
> This document captures every hard-won debugging result so it never needs to be repeated.

## Board Overview

| Property | Value |
|---|---|
| Board | Guition ESP32-S3-4848S040 |
| MCU | ESP32-S3 (QFN56) revision v0.2 |
| Display | 480x480 IPS, ST7701S controller |
| Display Interface | 16-bit RGB parallel (not SPI for pixel data) |
| Panel Init | 3-wire SPI (separate from RGB data bus) |
| Touch | GT911 capacitive, I2C |
| Flash | 16MB **QIO (quad)** |
| PSRAM | 8MB **OPI (octal)** |
| Framework | Arduino on ESP-IDF (PlatformIO) |
| Display Library | LovyanGFX 1.2.x |
| UI Library | LVGL 9.4.0 |

## Critical Hardware Facts

### Flash and PSRAM

```ini
# platformio.ini
board_build.arduino.memory_type = qio_opi   # Flash=QIO, PSRAM=OPI
board_build.arduino.psram_type = opi
board_upload.flash_size = 16MB
board_build.partitions = default_16MB.csv
```

**WARNING**: Setting `memory_type = opi_opi` will fail to boot. The flash chip is quad-SPI, not octal.

### Pin Map

#### RGB Data Bus (16-bit, active display data)

| Signal | GPIO | Signal | GPIO |
|--------|------|--------|------|
| B0 (d0) | 4 | B1 (d1) | 5 |
| B2 (d2) | 6 | B3 (d3) | 7 |
| B4 (d4) | 15 | G0 (d5) | 8 |
| G1 (d6) | 20 | G2 (d7) | 3 |
| G3 (d8) | 46 | G4 (d9) | 9 |
| G5 (d10) | 10 | R0 (d11) | 11 |
| R1 (d12) | 12 | R2 (d13) | 13 |
| R3 (d14) | 14 | R4 (d15) | 0 |

#### RGB Control Signals

| Signal | GPIO |
|--------|------|
| DE (data enable / henable) | 18 |
| VSYNC | 17 |
| HSYNC | 16 |
| PCLK (pixel clock) | 21 |

#### ST7701S 3-Wire SPI (panel initialization only)

| Signal | GPIO |
|--------|------|
| CS | 39 |
| SCL (clock) | 48 |
| SDA (data/MOSI) | 47 |

#### Backlight

| Signal | GPIO |
|--------|------|
| BL (backlight) | 38 |

**IMPORTANT**: After calling `lgfx->setBrightness()`, you must also force:
```cpp
pinMode(38, OUTPUT);
digitalWrite(38, HIGH);
```
PWM brightness control works on some units but not reliably on all. The GPIO force ensures the backlight is always on.

#### Touch (GT911)

| Signal | GPIO |
|--------|------|
| SDA | 19 |
| SCL | 45 |
| INT | -1 (not connected) |
| RST | -1 (not connected) |
| I2C Address | 0x5D |
| I2C Port | 1 (not default port 0) |
| I2C Frequency | 400kHz |

## Display Driver: LovyanGFX

### Panel Type

Use `lgfx::Panel_ST7701_guition_esp32_4848S040` - this is a LovyanGFX built-in panel type specifically for this board. It contains the correct ST7701S initialization command sequence.

**Do NOT use** `lgfx::Panel_RGB` directly - it won't send the required SPI init commands and the display will show garbage or stay blank.

### Complete LovyanGFX Configuration Class

```cpp
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

class LGFX_ST7701S : public lgfx::LGFX_Device {
public:
    lgfx::Bus_RGB _bus_instance;
    lgfx::Panel_ST7701_guition_esp32_4848S040 _panel_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;

    LGFX_ST7701S(void) {
        // === RGB Bus ===
        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;

            cfg.pin_d0 = GPIO_NUM_4;   cfg.pin_d1 = GPIO_NUM_5;
            cfg.pin_d2 = GPIO_NUM_6;   cfg.pin_d3 = GPIO_NUM_7;
            cfg.pin_d4 = GPIO_NUM_15;  cfg.pin_d5 = GPIO_NUM_8;
            cfg.pin_d6 = GPIO_NUM_20;  cfg.pin_d7 = GPIO_NUM_3;
            cfg.pin_d8 = GPIO_NUM_46;  cfg.pin_d9 = GPIO_NUM_9;
            cfg.pin_d10 = GPIO_NUM_10; cfg.pin_d11 = GPIO_NUM_11;
            cfg.pin_d12 = GPIO_NUM_12; cfg.pin_d13 = GPIO_NUM_13;
            cfg.pin_d14 = GPIO_NUM_14; cfg.pin_d15 = GPIO_NUM_0;

            cfg.pin_henable = GPIO_NUM_18;
            cfg.pin_vsync = GPIO_NUM_17;
            cfg.pin_hsync = GPIO_NUM_16;
            cfg.pin_pclk = GPIO_NUM_21;
            cfg.freq_write = 16000000;  // 16MHz pixel clock

            cfg.hsync_polarity = 0;    cfg.hsync_front_porch = 10;
            cfg.hsync_pulse_width = 8; cfg.hsync_back_porch = 50;
            cfg.vsync_polarity = 0;    cfg.vsync_front_porch = 10;
            cfg.vsync_pulse_width = 8; cfg.vsync_back_porch = 20;
            cfg.pclk_active_neg = 1;
            cfg.de_idle_high = 0;
            cfg.pclk_idle_high = 0;

            _bus_instance.config(cfg);
        }

        // === Panel ===
        {
            auto cfg = _panel_instance.config();
            cfg.memory_width = 480;
            cfg.memory_height = 480;
            cfg.panel_width = 480;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.readable = false;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;

            _panel_instance.config(cfg);
        }

        // === ST7701S 3-wire SPI (panel init commands) ===
        {
            auto cfg = _panel_instance.config_detail();
            cfg.pin_cs   = 39;
            cfg.pin_sclk = 48;
            cfg.pin_mosi = 47;
            cfg.use_psram = 2;  // Use PSRAM for framebuffer
            _panel_instance.config_detail(cfg);
        }

        // === Backlight (PWM) ===
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = 38;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        // === Touch (GT911) ===
        {
            auto cfg = _touch_instance.config();
            cfg.x_min = 0;  cfg.x_max = 480;
            cfg.y_min = 0;  cfg.y_max = 480;
            cfg.pin_int = -1;
            cfg.pin_rst = -1;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;
            cfg.i2c_port = 1;
            cfg.pin_sda = 19;
            cfg.pin_scl = 45;
            cfg.freq = 400000;
            cfg.i2c_addr = 0x5D;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        _panel_instance.setBus(&_bus_instance);
        setPanel(&_panel_instance);
    }
};
```

### Initialization

```cpp
LGFX_ST7701S* lgfx = new LGFX_ST7701S();
lgfx->init();
lgfx->setBrightness(75);
pinMode(38, OUTPUT);
digitalWrite(38, HIGH);  // Force backlight on
lgfx->fillScreen(TFT_BLACK);
```

## LVGL 9.4.0 Integration

### lv_conf.h Key Settings

```c
#define LV_COLOR_DEPTH 16                    // RGB565
#define LV_USE_STDLIB_MALLOC  LV_STDLIB_CLIB // Use system malloc -> PSRAM
#define LV_USE_STDLIB_STRING  LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB
#define LV_DEF_REFR_PERIOD 30               // 30ms refresh
#define LV_INDEV_DEF_READ_PERIOD 30         // 30ms input poll
#define LV_DPI_DEF 130
#define LV_USE_OS LV_OS_NONE
#define LV_FONT_DEFAULT &lv_font_montserrat_16
```

**CRITICAL**: Use `LV_STDLIB_CLIB` not `LV_STDLIB_BUILTIN`. The builtin allocator uses a static 512KB+ array in DRAM which overflows the ESP32-S3's 320KB internal RAM. `LV_STDLIB_CLIB` uses `malloc()`/`free()` which routes large allocations to PSRAM automatically.

### Tick Source

LVGL 9 removed `LV_TICK_CUSTOM`. Set the tick callback explicitly:

```cpp
static uint32_t lvgl_tick_cb(void) {
    return (uint32_t)millis();
}

// During init:
lv_tick_set_cb(lvgl_tick_cb);
```

### Display Buffer

Allocate from PSRAM for best performance:

```cpp
size_t bufferSize = sizeof(lv_color_t) * 480 * 40;  // 40 lines
lv_color_t* buf = (lv_color_t*)heap_caps_malloc(
    bufferSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

lv_display_t* disp = lv_display_create(480, 480);
lv_display_set_buffers(disp, buf, nullptr, bufferSize,
                       LV_DISPLAY_RENDER_MODE_PARTIAL);
lv_display_set_color_format(disp, LV_COLOR_FORMAT_NATIVE);
lv_display_set_flush_cb(disp, my_flush_cb);
```

### Flush Callback (CRITICAL - Color Correctness)

```cpp
void my_flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    lgfx->startWrite();
    lgfx->setAddrWindow(area->x1, area->y1, w, h);
    lgfx->writePixels((lgfx::rgb565_t*)px_map, w * h);  // <-- MUST use rgb565_t*
    lgfx->endWrite();

    lv_display_flush_ready(disp);
}
```

**WARNING**: Using `pushPixels((uint16_t*)px_map, ...)` causes wrong colors (byte-swap issue). You **must** cast to `lgfx::rgb565_t*` and use `writePixels()`. This is the single most common color bug with this board.

### Touch Input

```cpp
lv_indev_t* indev = lv_indev_create();
lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
lv_indev_set_read_cb(indev, my_touch_cb);

void my_touch_cb(lv_indev_t* indev, lv_indev_data_t* data) {
    uint16_t tx, ty;
    if (lgfx->getTouch(&tx, &ty)) {
        data->point.x = tx;
        data->point.y = ty;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
```

### Main Loop

```cpp
void loop() {
    lv_timer_handler();  // Drives rendering, input, animations
    // Your app logic here
}
```

Do NOT call `lv_tick_inc()` manually - the tick callback handles it.

## LVGL 9 API Gotchas (vs LVGL 8)

| LVGL 8 | LVGL 9 | Notes |
|--------|--------|-------|
| `LV_LABEL_LONG_DOTS` | `LV_LABEL_LONG_MODE_DOTS` | Enum renamed |
| `lv_event_get_target(e)` returns `lv_obj_t*` | Returns `void*` | Need `static_cast<lv_obj_t*>()` |
| `LV_TICK_CUSTOM` in lv_conf.h | Removed | Use `lv_tick_set_cb()` |
| `lv_spinner_create(parent, time, width)` | `lv_spinner_create(parent)` | 1 arg; use `lv_spinner_set_anim_params()` for timing |
| `lv_btn_create()` | `lv_button_create()` | Renamed |
| `lv_disp_draw_buf_init()` | `lv_display_set_buffers()` | New API |
| `lv_disp_drv_init()` + register | `lv_display_create()` + setters | Simplified |

## Build Configuration

### platformio.ini Build Flags

```ini
build_flags =
    -std=gnu++17
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DCORE_DEBUG_LEVEL=3
    -DLV_CONF_PATH=\"lv_conf.h\"
    -DLV_DRAW_SW_HELIUM=0       ; Disable ARM Helium (we're Xtensa)
    -DDISPLAY_ST7701S
    -DSCREEN_WIDTH=480
    -DSCREEN_HEIGHT=480
    -DTFT_BL=38
    -DTOUCH_SDA=19
    -DTOUCH_SCL=45
    -I.                          ; For lv_conf.h at project root
build_unflags = -std=gnu++11     ; Must remove default C++11 for C++17
```

### Post-First-Build Fix

After the first build, LVGL copies ARM Helium assembly files that won't compile on Xtensa. Delete them:

```
rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/convert/helium
rm -rf .pio/libdeps/esp32-s3-4848s040/lvgl/src/draw/sw/blend/helium
```

### lv_conf.h Location

The file must be at the **project root** (not in `include/` or `src/`). The `-I.` build flag adds the project root to the include path so LVGL finds it. Delete any copies in `.pio/libdeps/` to avoid conflicts.

## Logging

Use ESP-IDF logging, not `Serial.println`:

```cpp
#include <esp_log.h>
static const char* TAG = "MyModule";

ESP_LOGI(TAG, "Info: %s", msg);    // Info
ESP_LOGW(TAG, "Warning: %d", val); // Warning
ESP_LOGE(TAG, "Error!");           // Error
```

Serial monitor at 115200 baud. `Serial.begin(115200)` in `setup()` is still needed for ESP-IDF log output to appear.

## Common Pitfalls

1. **White/blank screen**: Wrong panel type. Must use `Panel_ST7701_guition_esp32_4848S040`, not `Panel_RGB`.
2. **Wrong colors**: Using `pushPixels((uint16_t*)...)` instead of `writePixels((lgfx::rgb565_t*)...)`.
3. **Boot crash / no PSRAM**: `memory_type` set to `opi_opi` instead of `qio_opi`.
4. **DRAM overflow at link time**: `LV_STDLIB_BUILTIN` allocating a huge static array. Switch to `LV_STDLIB_CLIB`.
5. **Backlight not on**: Missing `pinMode(38, OUTPUT); digitalWrite(38, HIGH);` after `setBrightness()`.
6. **Touch not working**: I2C port set to 0 instead of 1, or address wrong (must be 0x5D).
7. **Multiple lv_conf.h**: Stale copies in `.pio/libdeps/`. Only the root one should exist.
8. **Helium build errors**: ARM assembly files in LVGL. Delete the `helium/` directories.
9. **LSP errors about Arduino.h**: Normal for PlatformIO - doesn't affect compilation.
10. **`LV_TICK_CUSTOM` not found**: LVGL 9 removed it. Use `lv_tick_set_cb()` instead.
