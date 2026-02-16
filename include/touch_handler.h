/**
 * touch_handler.h - FT6236 Touch Controller Handler
 * LilyGo T-Display S3 Touch
 */

#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "spotify_gui.h"

// Touch Event Typen
enum TouchEventType {
    TOUCH_NONE,
    TOUCH_PRESS,
    TOUCH_RELEASE,
    TOUCH_TAP,
    TOUCH_SWIPE_UP,
    TOUCH_SWIPE_DOWN,
    TOUCH_SWIPE_LEFT,
    TOUCH_SWIPE_RIGHT
};

// Touch Event
struct TouchEvent {
    TouchEventType type = TOUCH_TAP;
    int x = 0;
    int y = 0;
    unsigned long timestamp = 0;
};

// Touch Callback
typedef void (*TouchCallback)(const TouchEvent& event);

class TouchHandler {
public:
    TouchHandler();
    
    void begin();
    void update();
    void setGUI(SpotifyGUI* gui) { this->gui = gui; }
    
    // Event Registration
    void onPlayPause(TouchCallback callback) { onPlayPauseCb = callback; }
    void onNext(TouchCallback callback) { onNextCb = callback; }
    void onPrevious(TouchCallback callback) { onPreviousCb = callback; }
    void onVolumeChange(TouchCallback callback) { onVolumeChangeCb = callback; }
    void onSeek(TouchCallback callback) { onSeekCb = callback; }
    void onSave(TouchCallback callback) { onSaveCb = callback; }
    
    // Status
    bool isTouching() { return touchCount > 0; }
    
private:
    SpotifyGUI* gui;
    
    // Touch State
    int touchCount;
    int lastX, lastY;
    bool wasTouching;
    unsigned long touchStartTime;
    unsigned long lastTouchTime;
    
    // Debounce
    static constexpr unsigned long DEBOUNCE_MS = 50;
    static constexpr unsigned long TAP_TIMEOUT_MS = 300;
    
    // Callbacks
    TouchCallback onPlayPauseCb;
    TouchCallback onNextCb;
    TouchCallback onPreviousCb;
    TouchCallback onVolumeChangeCb;
    TouchCallback onSeekCb;
    TouchCallback onSaveCb;
    
    // Hardware
    bool readTouch(int& x, int& y);
    void processTouch(int x, int y);
    void handleButtonPress(int x, int y);
    void handleSliderTouch(int x, int y);
    
    // Gesture Detection
    TouchEventType detectGesture(int x, int y);
    bool isSwipe(int deltaX, int deltaY);
};

#endif // TOUCH_HANDLER_H
