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
  uint16_t syncLostCount;

  // Touch protocol constants
  static constexpr uint8_t TOUCH_HEADER_BYTE = 0xAA;
  static constexpr uint8_t TOUCH_PACKET_SIZE = 7;

public:
  struct TouchPoint {
    uint16_t x;
    uint16_t y;
    bool pressed;
    uint8_t gesture; // Swipe, Tap, etc.
  };

  TouchManagerUART() : serial(nullptr), initialized(false), syncLostCount(0) {
  }

  void init(HardwareSerial& serialRef, uint8_t rxPin, uint8_t txPin) {
    serial = &serialRef;
    touchRx = rxPin;
    touchTx = txPin;
    syncLostCount = 0;

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
    return serial->available() >= TOUCH_PACKET_SIZE;
  }

  TouchPoint getTouchPoint() {
    TouchPoint point = {0, 0, false, 0};

    if (!initialized || !serial) {
      return point;
    }

    // Robust sync: Search for header byte
    while (serial->available() > 0) {
      uint8_t peekedByte = serial->peek();  // Look ahead without reading

      if (peekedByte == TOUCH_HEADER_BYTE) {
        break;  // Header found!
      }

      // Wrong byte -> discard and continue searching
      serial->read();
      syncLostCount++;

      if (syncLostCount > 100) {
        Serial.println("⚠️ UART sync lost, flushing buffer");
        while (serial->available()) {
          serial->read();
        }
        syncLostCount = 0;
        return point;
      }
    }

    // Need at least 7 bytes: Header + X(2) + Y(2) + Pressed(1) + Gesture(1)
    if (serial->available() < TOUCH_PACKET_SIZE) {
      return point;  // Not enough data yet
    }

    // Read header
    uint8_t header = serial->read();
    if (header != TOUCH_HEADER_BYTE) {
      return point;  // Should not happen after sync check above
    }

    // Read data bytes
    uint8_t x_high = serial->read();
    uint8_t x_low = serial->read();
    uint8_t y_high = serial->read();
    uint8_t y_low = serial->read();
    point.pressed = (serial->read() == 0x01);
    point.gesture = serial->read();

    // Parse coordinates
    point.x = (x_high << 8) | x_low;
    point.y = (y_high << 8) | y_low;

    // Bounds check (Bug #15)
    if (point.x >= DISPLAY_WIDTH || point.y >= DISPLAY_HEIGHT) {
      #if DEBUG_TOUCH
        Serial.printf("⚠️ Touch out of bounds: %d,%d\n", point.x, point.y);
      #endif
      point.pressed = false;
    }

    syncLostCount = 0;
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
