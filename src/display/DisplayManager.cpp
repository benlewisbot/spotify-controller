/**
 * @file DisplayManager.cpp
 * @brief Display Manager Implementation
 */

#include "DisplayManager.hpp"
#include "../config/Config.hpp"

// Include display drivers
#include "drivers/ILI9341Display.hpp"
#include "drivers/ILI9488Display.hpp"
#include "drivers/ST7789Display.hpp"
#include "drivers/ST7796UDisplay.hpp"

// Include touch drivers
#include "drivers/FT6236Touch.hpp"
#include "drivers/XPT2046Touch.hpp"

// Logging
#define LOG_TAG "DisplayMgr"

#include <Arduino.h>

DisplayManager::DisplayManager()
    : initialized(false)
    , displayImpl(nullptr)
    , touchImpl(nullptr)
    , display(nullptr)
    , indev(nullptr)
    , width(DEFAULT_DISPLAY_WIDTH)
    , height(DEFAULT_DISPLAY_HEIGHT)
    , lastTouchX(0)
    , lastTouchY(0)
    , lastTouchPressed(false)
    , lastUpdateMs(millis()) {
}

DisplayManager::~DisplayManager() {
    if (display) {
        lv_display_delete(display);
    }
    if (indev) {
        lv_indev_delete(indev);
    }
    delete displayImpl;
    delete touchImpl;
}

bool DisplayManager::init() {
    Serial.printf("\n🖥  Initializing DisplayManager...\n");

    // Create display driver
    if (!createDisplayDriver()) {
        Serial.println("❌ Failed to create display driver");
        return false;
    }

    Serial.printf("✅ Display: %s (%dx%d)\n",
                  displayImpl->getName(),
                  displayImpl->getWidth(),
                  displayImpl->getHeight());

    // Create touch driver
    if (!createTouchDriver()) {
        Serial.println("⚠️  No touch controller detected");
    }

    // Initialize LVGL
    if (!initLVGL()) {
        Serial.println("❌ Failed to initialize LVGL");
        return false;
    }

    Serial.println("✅ LVGL initialized");
    Serial.println("✅ DisplayManager ready\n");

    initialized = true;
    return true;
}

bool DisplayManager::createDisplayDriver() {
    // Auto-detect or use specified type
    switch (DISPLAY_TYPE) {
        case DISPLAY_TYPE_ILI9341:
            displayImpl = new ILI9341Display();
            break;

        case DISPLAY_TYPE_ILI9488:
            displayImpl = new ILI9488Display();
            break;

        case DISPLAY_TYPE_ST7789:
            displayImpl = new ST7789Display();
            break;

        case DISPLAY_TYPE_ST7796U:
            displayImpl = new ST7796UDisplay();
            break;

        case DISPLAY_TYPE_AUTO:
        default:
            // Try to auto-detect
            // Start with ILI9341 (most common)
            displayImpl = new ILI9341Display();
            if (!displayImpl->init()) {
                delete displayImpl;
                displayImpl = new ILI9488Display();
                if (!displayImpl->init()) {
                    delete displayImpl;
                    displayImpl = new ST7789Display();
                    if (!displayImpl->init()) {
                        delete displayImpl;
                        displayImpl = new ST7796UDisplay();
                        if (!displayImpl->init()) {
                            delete displayImpl;
                            displayImpl = nullptr;
                            return false;
                        }
                    }
                }
            }
            break;
    }

    // Initialize the display
    if (!displayImpl->init()) {
        delete displayImpl;
        displayImpl = nullptr;
        return false;
    }

    // Get dimensions
    width = displayImpl->getWidth();
    height = displayImpl->getHeight();

    // Set orientation from config
    auto& config = ConfigManager::getInstance();
    displayImpl->setOrientation(config.getDisplayOrientation() == 1);

    // Set brightness from config
    displayImpl->setBrightness(config.getBrightness());

    return true;
}

bool DisplayManager::createTouchDriver() {
    // Try FT6236 first (capacitive, common on newer displays)
    touchImpl = new FT6236Touch();
    if (touchImpl->init()) {
        Serial.printf("✅ Touch: FT6236 Capacitive\n");
        return true;
    }
    delete touchImpl;

    // Try XPT2046 (resistive, common on ILI9341 displays)
    touchImpl = new XPT2046Touch();
    if (touchImpl->init()) {
        Serial.printf("✅ Touch: XPT2046 Resistive\n");
        return true;
    }
    delete touchImpl;

    touchImpl = nullptr;
    return false;
}

bool DisplayManager::initLVGL() {
    // Initialize LVGL
    lv_init();

    // Allocate display buffer from PSRAM if available
    size_t bufferSize = sizeof(lv_color_t) * width * 40;
    lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc(bufferSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!buf1) {
        Serial.println("⚠️  PSRAM not available, using regular RAM");
        buf1 = (lv_color_t*)malloc(bufferSize);
        if (!buf1) {
            Serial.println("❌ Failed to allocate display buffer");
            return false;
        }
    }

    // Create display driver
    display = lv_display_create(width, height);
    if (!display) {
        free(buf1);
        return false;
    }

    lv_display_set_flush_cb(display, flushCallback);
    lv_display_set_flush_ready_cb(display, flushReadyCallback);
    lv_display_set_buffers(display, buf1, nullptr, bufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Create input device driver
    if (touchImpl) {
        indev = lv_indev_create();
        if (indev) {
            lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
            lv_indev_set_read_cb(indev, touchCallback);
        }
    }

    return true;
}

void DisplayManager::update() {
    if (!initialized) {
        return;
    }

    // Increase LVGL tick
    lv_tick_inc(millis() - lastUpdateMs);
    lastUpdateMs = millis();

    // Handle LVGL tasks
    lv_timer_handler();
}

void DisplayManager::flushCallback(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    DisplayManager* dm = (DisplayManager*)lv_display_get_user_data(disp);

    if (dm && dm->displayImpl) {
        // TODO: Implement actual pixel rendering to display
        // This depends on the specific display driver implementation
    }

    lv_display_flush_ready(disp);
}

void DisplayManager::touchCallback(lv_indev_t* indev, lv_indev_data_t* data) {
    DisplayManager* dm = (DisplayManager*)lv_indev_get_user_data(indev);

    if (dm && dm->touchImpl) {
        int16_t x, y;
        if (dm->touchImpl->read(x, y)) {
            data->point.x = x;
            data->point.y = y;
            data->state = LV_INDEV_STATE_PRESSED;
            dm->lastTouchPressed = true;
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
            dm->lastTouchPressed = false;
        }
    }
}

void DisplayManager::flushReadyCallback(lv_display_t* disp) {
    lv_display_flush_ready(disp);
}

void DisplayManager::renderCallback(lv_timer_t* timer) {
    // This is called by LVGL timer system
    DisplayManager* dm = (DisplayManager*)lv_timer_get_user_data(timer);
    if (dm) {
        dm->update();
    }
}
