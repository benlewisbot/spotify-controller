/**
 * @file main.cpp
 * @brief ESP32 Spotify Controller - Main Entry Point
 *
 * Phase 1 MVP Implementation:
 * - WiFi Connection with Auto-Reconnect
 * - Spotify OAuth 2.0 Authentication
 * - Now Playing Screen with Album Art, Title, Artist
 * - Playback Controls (Play/Pause, Next, Previous, Volume)
 * - Touch Input with Button Feedback
 * - Modular Display Interface (for various displays)
 * - Modern UI (Spotify-inspired + Apple Liquid Glass)
 */

#include <Arduino.h>
#include "app/App.hpp"
#include "config/Config.hpp"

// Global app instance
App* app = nullptr;

/**
 * @brief Arduino Setup
 */
void setup() {
    // Initialize serial
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n========================================");
    Serial.println("  🎵 Spotify Controller ESP32");
    Serial.println("  Version: 1.0.0 (Phase 1 MVP)");
    Serial.println("========================================\n");

    // Get app instance and initialize
    app = &App::getInstance();

    if (!app->init()) {
        Serial.println("❌ Failed to initialize application!");
        Serial.println("   System will restart in 5 seconds...");

        // Blink LED to indicate error
        pinMode(LED_BUILTIN, OUTPUT);
        for (int i = 0; i < 10; i++) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(500);
        }

        ESP.restart();
    }

    Serial.println("\n✅ Setup complete!");
}

/**
 * @brief Arduino Main Loop
 */
void loop() {
    // Update application
    if (app) {
        app->loop();
    }

    // Small delay to prevent watchdog issues
    delay(1);
}
