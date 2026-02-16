#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include <Arduino.h>
#include "config.h"

enum TouchController {
  XPT2046,     // Resistiver Touch
  FT6236,      // Capacitive Touch (LilyGo T-Display S3 Touch)
  FT6336U,     // Capacitive Touch
  CST_SELF,    // Capacitive Touch
  UNKNOWN_TOUCH
};

struct TouchPoint {
  uint16_t x = 0;
  uint16_t y = 0;
  bool pressed = false;
};

class TouchManager {
private:
  TouchController controller;
  uint8_t touchIntPin;
  bool initialized;

  // ISR-safe flag for interrupt-based touch detection (volatile for ISR access)
  volatile bool touchDetected = false;
  volatile unsigned long lastTouchIRQTime = 0;

public:
  TouchManager() : controller(TOUCH_CONTROLLER), touchIntPin(TOUCH_INT), initialized(false) {
  }

  // Call from ISR when touch interrupt fires
  void IRAM_ATTR onTouchInterrupt() {
    touchDetected = true;
    lastTouchIRQTime = millis();
  }

  // Clear touch flag (call after processing touch)
  void clearTouchFlag() {
    touchDetected = false;
  }

  // Check if touch was detected via interrupt
  bool hasPendingTouch() const {
    return touchDetected;
  }

  void init() {
    Serial.println("\n👆 Touch-Manager wird initialisiert...");

    // Touch-Controller aus config.h
    controller = TOUCH_CONTROLLER;

    // Pin konfigurieren
    if (TOUCH_INT != 255) {
      pinMode(TOUCH_INT, INPUT);
      touchIntPin = TOUCH_INT;
    }

    switch (controller) {
      case FT6236:
        initFT6236();
        break;
      case XPT2046:
        initXPT2046();
        break;
      default:
        Serial.println("⚠️ Kein Touch-Controller konfiguriert");
        break;
    }

    initialized = true;
    Serial.println("✅ Touch-Manager initialisiert");
  }

  bool isPressed() {
    if (!initialized) return false;

    switch (controller) {
      case FT6236:
        return checkFT6236Pressed();
      case XPT2046:
        return checkXPT2046Pressed();
      default:
        return false;
    }
  }

  TouchPoint getTouchPoint() {
    TouchPoint point = {0, 0, false};

    if (!initialized || !isPressed()) {
      return point;
    }

    switch (controller) {
      case FT6236:
        return readFT6236();
      case XPT2046:
        return readXPT2046();
      default:
        return point;
    }
  }

  const char* getControllerName() {
    switch (controller) {
      case FT6236: return "FT6236 (Capacitive)";
      case XPT2046: return "XPT2046 (Resistive)";
      default: return "UNKNOWN";
    }
  }

private:
  // ===== FT6236 (Capacitive Touch) =====
  void initFT6236() {
    Serial.println("  - FT6236 Touch-Controller");
    // TODO: FT6236 Library initialisieren
    // Für jetzt: Placeholder
  }

  bool checkFT6236Pressed() {
    // TODO: FT6236 Library einbinden
    // Für jetzt: Placeholder
    return false;
  }

  TouchPoint readFT6236() {
    TouchPoint point = {0, 0, false};
    // TODO: FT6236 Library einbinden
    // Für jetzt: Placeholder
    return point;
  }

  // ===== XPT2046 (Resistiver Touch) =====
  void initXPT2046() {
    Serial.println("  - XPT2046 Touch-Controller");
    // TODO: XPT2046 Library initialisieren
    // Für jetzt: Placeholder
  }

  bool checkXPT2046Pressed() {
    // TODO: XPT2046 Library einbinden
    // Für jetzt: Placeholder
    return false;
  }

  TouchPoint readXPT2046() {
    TouchPoint point = {0, 0, false};
    // TODO: XPT2046 Library einbinden
    // Für jetzt: Placeholder
    return point;
  }
};

#endif // TOUCH_MANAGER_H
