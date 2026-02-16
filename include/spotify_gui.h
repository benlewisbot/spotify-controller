/**
 * spotify_gui.h - GUI Manager für Spotify Controller
 * Basiert auf Spotify Dark Theme (#121212, #1DB954)
 */

#ifndef SPOTIFY_GUI_H
#define SPOTIFY_GUI_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "config.h"
#include "spotify_manager.h"

// Button Bereiche
struct ButtonArea {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

// Slider Bereiche
struct SliderArea {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool vertical = false;
};

class SpotifyGUI {
public:
    SpotifyGUI(TFT_eSPI& tft);
    
    void begin();
    void update(const TrackInfo& track);
    
    // Main Screen
    void drawNowPlaying(const TrackInfo& track);
    void drawProgressBar(int progress, int duration, bool isPlaying);
    void drawCover(int x, int y, int size);
    void drawTrackInfo(const TrackInfo& track);
    void drawControls(bool isPlaying);
    void drawVolumeSlider(int volume);
    void drawSaveButton(bool isSaved);
    
    // Touch Hit Testing
    bool isPlayPausePressed(int x, int y);
    bool isNextPressed(int x, int y);
    bool isPreviousPressed(int x, int y);
    bool isVolumeSlider(int x, int y);
    bool isProgressBar(int x, int y);
    bool isSaveButton(int x, int y);
    
    // Update Methods
    int getVolumeFromTouch(int x, int y);
    int getSeekPosition(int x, int y, int duration);
    
    // Utility
    void clear();
    void drawRoundedRect(int x, int y, int w, int h, int r, uint32_t color);
    void fillRoundedRect(int x, int y, int w, int h, int r, uint32_t color);
    
private:
    TFT_eSPI& tft;
    
    // Layout
    static constexpr int COVER_SIZE = 280;
    static constexpr int COVER_X = (DISPLAY_WIDTH - COVER_SIZE) / 2;
    static constexpr int COVER_Y = 20;
    
    static constexpr int TITLE_Y = 320;
    static constexpr int ARTIST_Y = 355;
    
    static constexpr int PROGRESS_Y = 390;
    static constexpr int PROGRESS_HEIGHT = 8;
    static constexpr int PROGRESS_WIDTH = 400;
    static constexpr int PROGRESS_X = (DISPLAY_WIDTH - PROGRESS_WIDTH) / 2;
    
    static constexpr int CONTROLS_Y = 425;
    static constexpr int BUTTON_SIZE = 50;
    static constexpr int BUTTON_SPACING = 30;
    
    // Button Areas
    ButtonArea btnPlayPause;
    ButtonArea btnNext;
    ButtonArea btnPrevious;
    ButtonArea btnSave;
    
    // Slider Areas
    SliderArea sliderVolume;
    SliderArea sliderProgress;
    
    void initLayout();
    uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b);
    void drawCenteredText(const String& text, int x, int y, int fontSize, 
                         uint32_t color, const char* font = nullptr);
    void drawTruncatedText(const String& text, int x, int y, int maxWidth, 
                          int fontSize, uint32_t color);
};

#endif // SPOTIFY_GUI_H
