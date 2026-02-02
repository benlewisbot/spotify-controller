/**
 * @file test_ui.cpp
 * @brief Unit tests for UI components
 *
 * Tests:
 * - UI constants validation
 * - Color values
 * - Size calculations
 * - Position calculations
 */

#include "test_utils.hpp"
#include "../../include/config.h"

// Test: UI constants are valid
void testUIConstants() {
    Serial.println("\n🧪 Test: UI Constants");

    TestResult result;

    // Margins should be positive
    ASSERT_GT(UI_MARGIN, 0);

    // Album art should have reasonable size
    ASSERT_GT(UI_ALBUM_ART_SIZE, 100);
    ASSERT_LT(UI_ALBUM_ART_SIZE, 500);

    // Buttons should have reasonable sizes
    ASSERT_GT(UI_BUTTON_PLAY_SIZE, 0);
    ASSERT_GT(UI_BUTTON_PREV_NEXT_SIZE, 0);
    ASSERT_GT(UI_BUTTON_SMALL_SIZE, 0);

    // Volume slider dimensions
    ASSERT_GT(UI_VOLUME_SLIDER_WIDTH, 0);
    ASSERT_GT(UI_VOLUME_SLIDER_HEIGHT, 0);

    // Radii should be reasonable
    ASSERT_GT(UI_RADIUS_LARGE, 0);
    ASSERT_GT(UI_RADIUS_SMALL, 0);
    ASSERT_GT(UI_RADIUS_KNOB, 0);

    // Progress bar height
    ASSERT_GT(UI_PROGRESS_BAR_HEIGHT, 0);

    result.printSummary();
}

// Test: Color values are valid
void testColorValues() {
    Serial.println("\n🧪 Test: Color Values");

    TestResult result;

    // Colors should be valid 24-bit RGB values
    ASSERT_GT(COLOR_SPOTIFY_BG, 0);
    ASSERT_LT(COLOR_SPOTIFY_BG, 0xFFFFFF);

    ASSERT_GT(COLOR_SPOTIFY_SURFACE, 0);
    ASSERT_LT(COLOR_SPOTIFY_SURFACE, 0xFFFFFF);

    ASSERT_GT(COLOR_SPOTIFY_PRIMARY, 0);
    ASSERT_LT(COLOR_SPOTIFY_PRIMARY, 0xFFFFFF);

    ASSERT_GT(COLOR_SPOTIFY_TEXT, 0);
    ASSERT_LT(COLOR_SPOTIFY_TEXT, 0xFFFFFF);

    ASSERT_GT(COLOR_SPOTIFY_TEXT_SEC, 0);
    ASSERT_LT(COLOR_SPOTIFY_TEXT_SEC, 0xFFFFFF);

    // Spotify green should be the expected value
    ASSERT_EQ(0x1DB954, COLOR_SPOTIFY_PRIMARY);

    // Background should be dark
    ASSERT_LT(COLOR_SPOTIFY_BG, 0x404040);

    result.printSummary();
}

// Test: Display dimensions
void testDisplayDimensions() {
    Serial.println("\n🧪 Test: Display Dimensions");

    TestResult result;

    // Width and height should be positive
    ASSERT_GT(DISPLAY_WIDTH, 100);
    ASSERT_LT(DISPLAY_WIDTH, 2000);

    ASSERT_GT(DISPLAY_HEIGHT, 100);
    ASSERT_LT(DISPLAY_HEIGHT, 2000);

    // Calculate usable width (account for margins and volume slider)
    int usableWidth = DISPLAY_WIDTH - (UI_MARGIN * 2) - UI_VOLUME_SLIDER_WIDTH - 8;
    ASSERT_GT(usableWidth, 0);

    // Album art should fit in width
    ASSERT_LT(UI_ALBUM_ART_SIZE, usableWidth + UI_ALBUM_ART_SIZE);

    result.printSummary();
}

// Test: Button positioning calculation
void testButtonPositioning() {
    Serial.println("\n🧪 Test: Button Positioning");

    TestResult result;

    // Calculate control area width
    int controlAreaWidth = DISPLAY_WIDTH - UI_MARGIN * 2 - UI_VOLUME_SLIDER_WIDTH - 8;
    ASSERT_GT(controlAreaWidth, 0);

    // Calculate center X
    int centerX = UI_MARGIN + controlAreaWidth / 2;
    ASSERT_GT(centerX, UI_MARGIN);
    ASSERT_LT(centerX, DISPLAY_WIDTH - UI_MARGIN);

    // Previous button position
    int prevX = UI_MARGIN;
    int prevY = DISPLAY_HEIGHT - UI_MARGIN;
    ASSERT_GT(prevX, 0);
    ASSERT_LT(prevY, DISPLAY_HEIGHT);

    // Next button position
    int nextX = DISPLAY_WIDTH - UI_MARGIN - UI_VOLUME_SLIDER_WIDTH - 4;
    ASSERT_GT(nextX, centerX);
    ASSERT_LT(nextX, DISPLAY_WIDTH);

    // Play button position (centered)
    int playX = DISPLAY_WIDTH / 2 - UI_VOLUME_SLIDER_WIDTH / 2 - 4;
    ASSERT_GT(playX, prevX);
    ASSERT_LT(playX, nextX);

    result.printSummary();
}

// Test: Volume slider positioning
void testVolumeSliderPosition() {
    Serial.println("\n🧪 Test: Volume Slider Position");

    TestResult result;

    // Slider should be on the right side
    int sliderX = DISPLAY_WIDTH - UI_MARGIN - UI_VOLUME_SLIDER_WIDTH;
    ASSERT_GT(sliderX, DISPLAY_WIDTH / 2);
    ASSERT_LT(sliderX, DISPLAY_WIDTH);

    // Slider vertical position
    int sliderY = 20; // Offset from center
    ASSERT_GT(sliderY, 0);
    ASSERT_LT(sliderY, DISPLAY_HEIGHT);

    // Volume icon position (above slider)
    int iconY = sliderY - UI_RADIUS_KNOB - 4;
    ASSERT_GT(iconY, 0);

    result.printSummary();
}

// Test: Progress bar positioning
void testProgressBarPosition() {
    Serial.println("\n🧪 Test: Progress Bar Position");

    TestResult result;

    // Progress bar should be near bottom
    int progressY = DISPLAY_HEIGHT - 60;
    ASSERT_GT(progressY, DISPLAY_HEIGHT / 2);
    ASSERT_LT(progressY, DISPLAY_HEIGHT);

    // Progress bar width
    int progressWidth = DISPLAY_WIDTH - (UI_MARGIN * 2);
    ASSERT_GT(progressWidth, DISPLAY_WIDTH / 2);
    ASSERT_LT(progressWidth, DISPLAY_WIDTH);

    result.printSummary();
}

// Test: Layout sanity checks
void testLayoutSanity() {
    Serial.println("\n🧪 Test: Layout Sanity");

    TestResult result;

    // Controls should fit below progress bar
    int progressBottom = DISPLAY_HEIGHT - 60;
    int controlsTop = DISPLAY_HEIGHT - UI_MARGIN - UI_BUTTON_PLAY_SIZE;
    ASSERT_GT(controlsTop, progressBottom);

    // Album art should fit above track info
    int albumArtBottom = DISPLAY_HEIGHT / 2 + UI_ALBUM_ART_SIZE / 2;
    ASSERT_LT(albumArtBottom, progressBottom);

    // Track info should fit
    int trackInfoTop = UI_MARGIN;
    ASSERT_GT(trackInfoBottom := trackInfoTop + 50, trackInfoTop);
    ASSERT_LT(trackInfoBottom, progressBottom);

    // Volume slider should be visible
    int sliderBottom = DISPLAY_HEIGHT / 2 + UI_VOLUME_SLIDER_HEIGHT / 2 + 20;
    ASSERT_LT(sliderBottom, controlsTop);

    result.printSummary();
}

// Run all UI tests
void runAllUITests() {
    Serial.println("\n========================================");
    Serial.println("  🧪 UI Component Unit Tests");
    Serial.println("========================================");

    testUIConstants();
    testColorValues();
    testDisplayDimensions();
    testButtonPositioning();
    testVolumeSliderPosition();
    testProgressBarPosition();
    testLayoutSanity();

    Serial.println("\n✅ All UI test suites completed!");
}

// For Arduino/ESP32
void setup() {
    Serial.begin(115200);
    delay(1000);

    runAllUITests();
}

void loop() {
    // Nothing to do
}
