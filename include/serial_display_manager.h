#ifndef SERIAL_DISPLAY_MANAGER_H
#define SERIAL_DISPLAY_MANAGER_H

#include <Arduino.h>
#include "config.h"

// Serial Display Protocol Commands
// Basiert auf ST7796U Serial Display Commands

enum SerialDisplayCommand {
  CMD_CLEAR_SCREEN = 0x01,
  CMD_FILL_RECT = 0x02,
  CMD_DRAW_LINE = 0x03,
  CMD_DRAW_CIRCLE = 0x04,
  CMD_DRAW_TEXT = 0x05,
  CMD_SET_COLOR = 0x06,
  CMD_SET_FONT_SIZE = 0x07,
  CMD_SET_CURSOR = 0x08,
  CMD_DRAW_PIXEL = 0x09,
  CMD_DRAW_ROUND_RECT = 0x0A,
  CMD_TOUCH_DATA = 0x20,
  CMD_TOUCH_ENABLE = 0x21,
  CMD_TOUCH_DISABLE = 0x22
};

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

class SerialDisplayManager {
private:
  HardwareSerial* serial;
  bool initialized;
  uint8_t displayTx;
  uint8_t displayRx;
  uint32_t baudRate;

  Color currentColor;
  uint8_t currentFontSize;

public:
  SerialDisplayManager() : serial(nullptr), initialized(false), currentColor({18, 18, 18}), currentFontSize(16) {
  }

  void init(HardwareSerial& serialRef, uint8_t txPin, uint8_t rxPin, uint32_t baud) {
    serial = &serialRef;
    displayTx = txPin;
    displayRx = rxPin;
    baudRate = baud;

    // Serial initialisieren
    serial->begin(baudRate, SERIAL_8N1, rxPin, txPin);

    // Display initialisieren
    clearScreen();

    #if DEBUG_DISPLAY
      Serial.println("\n🖥 Serial Display initialisiert:");
      Serial.printf("  TX Pin: %d\n", displayTx);
      Serial.printf("  RX Pin: %d\n", displayRx);
      Serial.printf("  Baud Rate: %d\n", baudRate);
    #endif

    initialized = true;
  }

  void clearScreen() {
    if (!initialized || !serial) return;

    sendCommand(CMD_CLEAR_SCREEN);
  }

  void setFillColor(uint8_t r, uint8_t g, uint8_t b) {
    if (!initialized || !serial) return;

    currentColor = {r, g, b};
    sendCommand(CMD_SET_COLOR);
    serial->write(r);
    serial->write(g);
    serial->write(b);
  }

  void setFillColor(uint16_t color) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    setFillColor(r, g, b);
  }

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    if (!initialized || !serial) return;

    sendCommand(CMD_FILL_RECT);
    serial->write((x >> 8) & 0xFF);
    serial->write(x & 0xFF);
    serial->write((y >> 8) & 0xFF);
    serial->write(y & 0xFF);
    serial->write((w >> 8) & 0xFF);
    serial->write(w & 0xFF);
    serial->write((h >> 8) & 0xFF);
    serial->write(h & 0xFF);
  }

  void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius) {
    if (!initialized || !serial) return;

    sendCommand(CMD_DRAW_ROUND_RECT);
    serial->write((x >> 8) & 0xFF);
    serial->write(x & 0xFF);
    serial->write((y >> 8) & 0xFF);
    serial->write(y & 0xFF);
    serial->write((w >> 8) & 0xFF);
    serial->write(w & 0xFF);
    serial->write((h >> 8) & 0xFF);
    serial->write(h & 0xFF);
    serial->write((radius >> 8) & 0xFF);
    serial->write(radius & 0xFF);
  }

  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    if (!initialized || !serial) return;

    sendCommand(CMD_DRAW_LINE);
    serial->write((x0 >> 8) & 0xFF);
    serial->write(x0 & 0xFF);
    serial->write((y0 >> 8) & 0xFF);
    serial->write(y0 & 0xFF);
    serial->write((x1 >> 8) & 0xFF);
    serial->write(x1 & 0xFF);
    serial->write((y1 >> 8) & 0xFF);
    serial->write(y1 & 0xFF);
  }

  void drawCircle(int16_t x, int16_t y, int16_t r) {
    if (!initialized || !serial) return;

    sendCommand(CMD_DRAW_CIRCLE);
    serial->write((x >> 8) & 0xFF);
    serial->write(x & 0xFF);
    serial->write((y >> 8) & 0xFF);
    serial->write(y & 0xFF);
    serial->write((r >> 8) & 0xFF);
    serial->write(r & 0xFF);
  }

  void drawPixel(int16_t x, int16_t y) {
    if (!initialized || !serial) return;

    sendCommand(CMD_DRAW_PIXEL);
    serial->write((x >> 8) & 0xFF);
    serial->write(x & 0xFF);
    serial->write((y >> 8) & 0xFF);
    serial->write(y & 0xFF);
  }

  void setFontSize(uint8_t size) {
    if (!initialized || !serial) return;

    currentFontSize = size;
    sendCommand(CMD_SET_FONT_SIZE);
    serial->write(size);
  }

  void setCursor(int16_t x, int16_t y) {
    if (!initialized || !serial) return;

    sendCommand(CMD_SET_CURSOR);
    serial->write((x >> 8) & 0xFF);
    serial->write(x & 0xFF);
    serial->write((y >> 8) & 0xFF);
    serial->write(y & 0xFF);
  }

  void print(const String& text) {
    if (!initialized || !serial) return;

    serial->print(text);
  }

  void println(const String& text) {
    if (!initialized || !serial) return;

    serial->println(text);
  }

  // Spotify Colors
  void setSpotifyBackground() {
    setFillColor(0x12, 0x12, 0x12);
  }

  void setSpotifySurface() {
    setFillColor(0x18, 0x18, 0x18);
  }

  void setSpotifyPrimary() {
    setFillColor(0x1D, 0xB9, 0x54); // Spotify Green
  }

  void setSpotifyText() {
    setFillColor(0xFF, 0xFF, 0xFF);
  }

  void setSpotifyTextSecondary() {
    setFillColor(0xB3, 0xB3, 0xB3);
  }

  bool isInitialized() {
    return initialized;
  }

  void sendCommand(uint8_t cmd) {
    serial->write(0xFF); // Start Byte
    serial->write(cmd);  // Command
    serial->write(0x00); // Padding (für Alignment)
  }

  // Touch-Handling für Serial Display
  void enableTouch() {
    if (!initialized || !serial) return;

    sendCommand(CMD_TOUCH_ENABLE);
  }

  void disableTouch() {
    if (!initialized || !serial) return;

    sendCommand(CMD_TOUCH_DISABLE);
  }

  // Touch-Daten lesen (über Serial)
  struct TouchData {
    uint16_t x;
    uint16_t y;
    bool pressed;
  };

  TouchData getTouchData() {
    TouchData data = {0, 0, false};

    if (!initialized || !serial) {
      return data;
    }

    // Warte auf Touch-Daten
    if (serial->available() >= 5) {
      uint8_t header = serial->read();
      if (header == 0xFF) {
        uint8_t cmd = serial->read();
        if (cmd == CMD_TOUCH_DATA) {
          data.x = (serial->read() << 8) | serial->read();
          data.y = (serial->read() << 8) | serial->read();
          data.pressed = true;
        }
      }
    }

    return data;
  }
};

#endif // SERIAL_DISPLAY_MANAGER_H
