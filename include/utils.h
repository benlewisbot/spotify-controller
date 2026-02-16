/**
 * utils.h - Hilfsfunktionen für Spotify Controller
 */

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// String Utilities
String urlEncode(const String& str);
String urlDecode(const String& str);
String extractQueryValue(const String& url, const String& key);
String generateRandomString(int length);

// HTTP Utilities
String httpGet(WiFiClientSecure& client, const String& url);
String httpPost(WiFiClientSecure& client, const String& url, 
                const String& body, const String& contentType = "application/json");

// JSON Utilities - Non-template versions for ArduinoJson v6
bool loadJson(const String& path, DynamicJsonDocument& doc);
bool saveJson(const String& path, DynamicJsonDocument& doc);
String jsonToString(DynamicJsonDocument& doc);

// Time Utilities
unsigned long getCurrentTime();
String formatTime(int seconds);
String formatMsTime(int milliseconds);

// Display Utilities
int centerTextWidth(const String& text, int fontSize);

// Debug Utilities
#ifdef DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(fmt, ...)
#endif

#endif // UTILS_H
