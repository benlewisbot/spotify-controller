#ifndef TOUCH_MANAGER_UART_H
#define TOUCH_MANAGER_UART_H

#include <Arduino.h>
#include "config.h"

class TouchManagerUART {
private:
  HardwareSerial* serial;
  uint8_t touchRx;
  uint8_t touchTx;
  bool initialized;

public:
  struct TouchPoint {
    uint16_t x;
    uint16_t y;
    bool pressed;
    uint8_t gesture; // Swipe, Tap, etc.
  };

  TouchManagerUART() : serial(nullptr), initialized(false) {
  }

  void init(HardwareSerial& serialRef, uint8_t rxPin, uint8_t txPin) {
    serial = &serialRef;
    touchRx = rxPin;
    touchTx = txPin;

    // Serial für Touch initialisieren
    serial->begin(115200, SERIAL_8N1, rxPin, txPin);

    #if DEBUG_TOUCH
      Serial.println("\n👆 UART Touch-Manager initialisiert:");
      Serial.printf("  RX Pin: %d\n", touchRx);
      Serial.printf("  TX Pin: %d\n", touchTx);
      Serial.printf("  Baud Rate: 115200\n");
    #endif

    initialized = true;
  }

  bool isPressed() {
    if (!initialized || !serial) {
      return false;
    }

    // Prüfen ob Touch-Daten verfügbar
    return serial->available() >= 7;
  }

  TouchPoint getTouchPoint() {
    TouchPoint point = {0, 0, false, 0};

    if (!initialized || !serial) {
      return point;
    }

    // Warte auf Touch-Daten
    if (serial->available() >= 7) {
      uint8_t header = serial->read();
      if (header == 0xAA) { // Touch-Header
        point.x = (serial->read() << 8) | serial->read();
        point.y = (serial->read() << 8) | serial->read();
        point.pressed = serial->read() == 0x01;
        point.gesture = serial->read();
      }
    }

    return point;
  }

  bool isPressedArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    TouchPoint point = getTouchPoint();

    if (!point.pressed) {
      return false;
    }

    // Prüfen ob Touch im Bereich ist
    return (point.x >= x1 && point.x <= x2 && point.y >= y1 && point.y <= y2);
  }

  // Touch-Gestures erkennen
  enum Gesture {
    GESTURE_NONE = 0,
    GESTURE_TAP = 1,
    GESTURE_SWIPE_LEFT = 2,
    GESTURE_SWIPE_RIGHT = 3,
    GESTURE_SWIPE_UP = 4,
    GESTURE_SWIPE_DOWN = 5
  };

  Gesture getGesture() {
    TouchPoint point = getTouchPoint();
    return (Gesture)point.gesture;
  }
};

#endif // TOUCH_MANAGER_UART_H
