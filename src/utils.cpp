/**
 * utils.cpp - Hilfsfunktionen Implementierung
 */

#include "utils.h"
#include <Arduino.h>

// URL Encoding
String urlEncode(const String& str) {
    String encoded = "";
    char c;
    char code0;
    char code1;
    for (unsigned int i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (c == ' ') {
            encoded += '+';
        } else if (isalnum(c)) {
            encoded += c;
        } else {
            code1 = (c & 0xf) + '0';
            if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
            c = (c >> 4) & 0xf;
            code0 = c + '0';
            if (c > 9) code0 = c - 10 + 'A';
            encoded += '%';
            encoded += code0;
            encoded += code1;
        }
    }
    return encoded;
}

// URL Decoding
String urlDecode(const String& str) {
    String decoded = "";
    char c;
    for (unsigned int i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (c == '+') {
            decoded += ' ';
        } else if (c == '%' && i + 2 < str.length()) {
            String hex = str.substring(i + 1, i + 3);
            decoded += (char)strtol(hex.c_str(), NULL, 16);
            i += 2;
        } else {
            decoded += c;
        }
    }
    return decoded;
}

// Extract Query Value
String extractQueryValue(const String& url, const String& key) {
    int keyIndex = url.indexOf(key + "=");
    if (keyIndex == -1) return "";
    
    int valueStart = keyIndex + key.length() + 1;
    int valueEnd = url.indexOf("&", valueStart);
    
    if (valueEnd == -1) {
        valueEnd = url.length();
    }
    
    return urlDecode(url.substring(valueStart, valueEnd));
}

// Generate Random String
String generateRandomString(int length) {
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    String result = "";
    for (int i = 0; i < length; i++) {
        result += chars[random(0, sizeof(chars) - 1)];
    }
    return result;
}

// HTTP GET
String httpGet(WiFiClientSecure& client, const String& url) {
    String response = "";
    
    int httpPort = 443;
    if (!url.startsWith("https://")) {
        return "";
    }
    
    String host = url.substring(8);
    int pathIndex = host.indexOf('/');
    String path = pathIndex == -1 ? "/" : host.substring(pathIndex);
    if (pathIndex != -1) host = host.substring(0, pathIndex);
    
    if (!client.connect(host.c_str(), httpPort)) {
        DEBUG_PRINTLN("HTTP GET connection failed");
        return "";
    }
    
    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    
    unsigned long timeout = millis() + 10000;
    while (client.available() == 0) {
        if (millis() > timeout) {
            client.stop();
            return "";
        }
    }
    
    // Skip headers
    while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r\n") break;
    }
    
    while (client.available()) {
        response += client.readString();
    }
    
    client.stop();
    return response;
}

// HTTP POST
String httpPost(WiFiClientSecure& client, const String& url,
                const String& body, const String& contentType) {
    String response = "";
    
    int httpPort = 443;
    if (!url.startsWith("https://")) {
        return "";
    }
    
    String host = url.substring(8);
    int pathIndex = host.indexOf('/');
    String path = pathIndex == -1 ? "/" : host.substring(pathIndex);
    if (pathIndex != -1) host = host.substring(0, pathIndex);
    
    if (!client.connect(host.c_str(), httpPort)) {
        DEBUG_PRINTLN("HTTP POST connection failed");
        return "";
    }
    
    client.print(String("POST ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Content-Type: " + contentType + "\r\n" +
                 "Content-Length: " + String(body.length()) + "\r\n" +
                 "Connection: close\r\n\r\n" +
                 body);
    
    unsigned long timeout = millis() + 10000;
    while (client.available() == 0) {
        if (millis() > timeout) {
            client.stop();
            return "";
        }
    }
    
    // Skip headers
    while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r\n") break;
    }
    
    while (client.available()) {
        response += client.readString();
    }
    
    client.stop();
    return response;
}

// Get Current Time
unsigned long getCurrentTime() {
    return millis();
}

// Format Time (mm:ss)
String formatTime(int seconds) {
    int mins = seconds / 60;
    int secs = seconds % 60;
    return String(mins) + ":" + (secs < 10 ? "0" : "") + String(secs);
}

// Format Milliseconds Time (mm:ss)
String formatMsTime(int milliseconds) {
    return formatTime(milliseconds / 1000);
}

// Center Text Width
int centerTextWidth(const String& text, int fontSize) {
    // Approximate width based on font size
    // Adjust based on actual font metrics if available
    return (SCREEN_WIDTH - text.length() * fontSize * 6) / 2;
}
