#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"

enum DisplayDriver {
  ILI9341,
  ILI9488,
  ST7789,
  ST7735,
  ST7796U,
  UNKNOWN
};

enum TouchController {
  XPT2046,     // Resistiver Touch
  FT6236,      // Capacitive Touch (LilyGo T-Display S3 Touch)
  FT6336U,     // Capacitive Touch
  CST_SELF,    // Capacitive Touch
  UNKNOWN_TOUCH
};

class DisplayManager {
private:
  TFT_eSPI* tft;
  bool initialized;
  DisplayDriver currentDriver;
  TouchController currentTouch;
  uint16_t width;
  uint16_t height;

public:
  DisplayManager() : tft(nullptr), initialized(false), width(DISPLAY_WIDTH), height(DISPLAY_HEIGHT) {
  }

  void init(TFT_eSPI& tftRef) {
    tft = &tftRef;
    
    // Display initialisieren
    tft->begin();
    
    // Rotation setzen
    tft->setRotation(DISPLAY_ORIENTATION_DEFAULT);
    
    // Display löschen
    tft->fillScreen(TFT_BLACK);
    
    // Display Info ausgeben
    #if DEBUG_DISPLAY
      Serial.println("\n🖥 Display initialisiert:");
      Serial.printf("  Typ: %s\n", DISPLAY_NAME);
      Serial.printf("  Größe: %dx%d\n", width, height);
      Serial.printf("  Treiber: %s\n", getDriverName(DISPLAY_DRIVER));
      Serial.printf("  Touch: %s\n", getTouchName(TOUCH_CONTROLLER));
    #endif
    
    initialized = true;
  }

  void setRotation(uint8_t rotation) {
    if (initialized && tft) {
      tft->setRotation(rotation);
    }
  }

  void fillScreen(uint16_t color) {
    if (initialized && tft) {
      tft->fillScreen(color);
    }
  }

  void drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (initialized && tft) {
      tft->drawPixel(x, y, color);
    }
  }

  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (initialized && tft) {
      tft->drawRect(x, y, w, h, color);
    }
  }

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (initialized && tft) {
      tft->fillRect(x, y, w, h, color);
    }
  }

  void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
    if (initialized && tft) {
      tft->drawCircle(x, y, r, color);
    }
  }

  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (initialized && tft) {
      tft->drawLine(x0, y0, x1, y1, color);
    }
  }

  void setTextColor(uint16_t color) {
    if (initialized && tft) {
      tft->setTextColor(color);
    }
  }

  void setTextSize(uint8_t size) {
    if (initialized && tft) {
      tft->setTextSize(size);
    }
  }

  void setCursor(int16_t x, int16_t y) {
    if (initialized && tft) {
      tft->setCursor(x, y);
    }
  }

  void print(const String& text) {
    if (initialized && tft) {
      tft->print(text);
    }
  }

  void println(const String& text) {
    if (initialized && tft) {
      tft->println(text);
    }
  }

  uint16_t getWidth() {
    return width;
  }

  uint16_t getHeight() {
    return height;
  }

  bool isInitialized() {
    return initialized;
  }

  const char* getDriverName(int driver) {
    switch (driver) {
      case ILI9341: return "ILI9341";
      case ILI9488: return "ILI9488";
      case ST7789: return "ST7789";
      case ST7796U: return "ST7796U";
      case ST7735: return "ST7735";
      default: return "UNKNOWN";
    }
  }

  const char* getTouchName(TouchController controller) {
    switch (controller) {
      case XPT2046: return "XPT2046 (Resistive)";
      case FT6236: return "FT6236 (Capacitive)";
      case FT6336U: return "FT6336U (Capacitive)";
      case CST_SELF: return "CST_SELF (Capacitive)";
      default: return "UNKNOWN";
    }
  }

  void drawTestPattern() {
    if (!initialized || !tft) return;

    tft->fillScreen(TFT_BLACK);

    // Test-Pattern zeichnen
    tft->fillRect(0, 0, width / 3, height, TFT_RED);
    tft->fillRect(width / 3, 0, width / 3, height, TFT_GREEN);
    tft->fillRect(2 * width / 3, 0, width / 3, height, TFT_BLUE);

    tft->setTextColor(TFT_WHITE);
    tft->setTextSize(2);
    tft->setCursor(10, height / 2 - 10);
    tft->print("TEST OK");

    delay(2000);
  }
};

#endif // DISPLAY_MANAGER_H
