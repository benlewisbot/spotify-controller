/**
 * @file State.hpp
 * @brief Application State Management
 */

#ifndef STATE_HPP
#define STATE_HPP

/**
 * @brief Application States
 */
enum class AppState {
    INIT,              // Initialization in progress
    WIFI_CONNECTING,   // Connecting to WiFi
    AUTH_REQUIRED,     // Spotify authentication needed
    AUTHENTICATING,    // Authenticating with Spotify
    READY,             // Ready to use
    NOW_PLAYING,       // Displaying now playing info
    ERROR,             // Error state
    SETTINGS           // Settings screen active
};

/**
 * @brief Event Types
 */
enum class EventType {
    // System events
    STATE_CHANGED,
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,

    // Authentication events
    AUTH_REQUIRED,
    AUTH_COMPLETED,
    AUTH_ERROR,
    SPOTIFY_AUTHENTICATED,
    SPOTIFY_AUTH_ERROR,
    TOKEN_REFRESHED,

    // Playback events
    PLAYBACK_CHANGED,
    TRACK_CHANGED,
    VOLUME_CHANGED,

    // UI events
    UI_REFRESH,
    SCREEN_CHANGED,
    BUTTON_PRESSED,
    TOUCH_GESTURE,

    // Config events
    CONFIG_LOADED,
    CONFIG_SAVED,
    CONFIG_ERROR
};

/**
 * @brief Event Data Structure
 */
struct Event {
    EventType type;
    int intValue;
    float floatValue;
    String stringValue;
    void* userData;

    // Constructors
    explicit Event(EventType t, int i = 0)
        : type(t), intValue(i), floatValue(0.0f), userData(nullptr) {}

    Event(EventType t, const String& s)
        : type(t), intValue(0), floatValue(0.0f), stringValue(s), userData(nullptr) {}

    Event(EventType t, float f)
        : type(t), intValue(0), floatValue(f), userData(nullptr) {}

    Event(EventType t, void* data)
        : type(t), intValue(0), floatValue(0.0f), userData(data) {}
};

/**
 * @brief Event Callback Function Type
 */
using EventCallback = std::function<void(const Event&)>;

#endif // STATE_HPP
