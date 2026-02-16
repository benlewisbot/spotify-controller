# Phase 5: Settings Screen Implementation

## Overview

Phase 5 implements a comprehensive settings system for the Spotify Controller ESP32 device. The implementation includes:

- **MenuManager**: Hierarchical screen navigation system
- **RuntimeConfig**: Persistent configuration storage using LittleFS
- **Settings Screens**: WiFi, Display, Spotify, and About screens
- **User Feedback**: Loading indicators, success/error messages, confirmation dialogs

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      WindowManager                          │
│  ┌─────────────────────────────────────────────────────────┤
│  │                    MenuManager                           │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┤
│  │  │ Settings  │ │   WiFi    │ │  Display  │ │  Spotify  │
│  │  │   Main    │ │ Settings  │ │ Settings  │ │   Token   │
│  │  └───────────┘ └───────────┘ └───────────┘ └───────────┤
│  │  ┌───────────┐                                          │
│  │  │   About   │                                          │
│  │  └───────────┘                                          │
│  └─────────────────────────────────────────────────────────┤
└─────────────────────────────────────────────────────────────┘
```

## File Structure

```
include/
├── RuntimeConfig.hpp           # Configuration structures & manager

src/
├── RuntimeConfig.cpp           # Configuration implementation
├── ui/
│   ├── WindowManager.hpp       # Updated with settings screens
│   ├── WindowManager.cpp       # Updated with MenuManager integration
│   └── screens/
│       ├── MenuManager.hpp     # Navigation system header
│       ├── MenuManager.cpp     # Navigation implementation
│       ├── Settings.hpp        # Main settings screen header
│       ├── Settings.cpp        # Main settings screen implementation
│       ├── WiFiSettingsScreen.hpp
│       ├── WiFiSettingsScreen.cpp
│       ├── SpotifyTokenScreen.hpp
│       ├── SpotifyTokenScreen.cpp
│       ├── DisplaySettingsScreen.hpp
│       ├── DisplaySettingsScreen.cpp
│       ├── AboutScreen.hpp
│       └── AboutScreen.cpp
```

## Components

### 1. RuntimeConfig (RuntimeConfig.hpp)

Persistent configuration management using LittleFS.

**Features:**
- JSON-based configuration storage
- Type-safe configuration structures
- Automatic save/load on startup
- Boot count tracking

**Configuration Sections:**
- `Display`: brightness, rotation, screensaver settings
- `WiFi`: auto-reconnect, timeout, last SSID
- `Spotify`: auto-play, device name, user info
- `Interface`: touch sound, haptic feedback, UI scale
- `System`: firmware version, build date, boot count

**Usage:**
```cpp
// Get configuration
auto& config = RuntimeConfigManager::getInstance().getConfig();
uint8_t brightness = config.display.brightness;

// Update configuration
config.display.brightness = 80;
RuntimeConfigManager::getInstance().save();
```

### 2. MenuManager (MenuManager.hpp/cpp)

Handles screen navigation with history support.

**Features:**
- Screen stack navigation (push/pop)
- Loading overlay with spinner
- Success/Error message dialogs
- Confirmation dialogs with callbacks
- Screen registration system

**Screen Types:**
```cpp
enum class ScreenType {
    SETTINGS_MAIN,
    WIFI_SETTINGS,
    WIFI_SCAN,
    WIFI_CONNECT,
    SPOTIFY_TOKEN,
    SPOTIFY_REAUTH,
    DISPLAY_SETTINGS,
    BRIGHTNESS,
    ORIENTATION,
    SCREENSAVER,
    ABOUT,
    DEVICE_INFO,
    FIRMWARE_INFO
};
```

**Usage:**
```cpp
auto& menuManager = MenuManager::getInstance();
menuManager.navigateTo(ScreenType::WIFI_SETTINGS);
menuManager.goBack();
menuManager.showLoading("Connecting...");
menuManager.showSuccess("Connected!");
```

### 3. SettingsScreen (Settings.cpp)

Main settings menu with navigation to sub-screens.

**Sections:**
- Network → WiFi Settings
- Display → Display Settings
- Spotify → Spotify Account
- System → About

**UI Elements:**
- Back button (returns to Now Playing)
- Section headers with gray labels
- Navigation items with subtitle and arrow icon

### 4. WiFiSettingsScreen

WiFi network configuration.

**Features:**
- Connection status card (icon, SSID, IP)
- Network scan button
- Available networks list
- Signal strength indicators (color-coded)
- Security icon (lock for secured networks)
- Connect buttons for each network

**Status Colors:**
- Green (>-50 dBm): Excellent signal
- Light Green (>-60 dBm): Good signal
- Yellow (>-70 dBm): Fair signal
- Red (<-70 dBm): Poor signal

### 5. SpotifyTokenScreen

Spotify OAuth2 token management.

**Features:**
- Account info (email, user ID)
- Token status (valid/expired)
- Time remaining until expiration
- Refresh Token button
- Re-authenticate button
- View Token Details button

### 6. DisplaySettingsScreen

Display configuration.

**Features:**
- Brightness slider (0-100%)
- Orientation selector (Portrait/Landscape)
- Screensaver enable toggle
- Screensaver timeout selector

**Settings:**
- Real-time brightness preview
- Automatic configuration save
- Multiple orientation options

### 7. AboutScreen

Device information and system stats.

**Information Displayed:**
- App logo and name
- Firmware version
- Device ID
- Build date
- System Information button (heap, uptime, chip model)
- Legal & Licenses button

## Integration

### WindowManager Updates

```cpp
// WindowManager.hpp
#include "screens/MenuManager.hpp"
#include "screens/WiFiSettingsScreen.hpp"
#include "screens/SpotifyTokenScreen.hpp"
#include "screens/DisplaySettingsScreen.hpp"
#include "screens/AboutScreen.hpp"

// New members:
ui::WiFiSettingsScreen* wifiSettingsScreen;
ui::SpotifyTokenScreen* spotifyTokenScreen;
ui::DisplaySettingsScreen* displaySettingsScreen;
ui::AboutScreen* aboutScreen;
MenuManager* menuManager;
```

### App.cpp Updates

```cpp
#include "RuntimeConfig.hpp"

bool App::initConfig() {
    configManager = new ConfigManager();
    bool configOk = configManager->init();
    
    // Initialize RuntimeConfig manager
    bool runtimeConfigOk = RuntimeConfigManager::getInstance().begin();
    
    return configOk && runtimeConfigOk;
}
```

## UI Design

### Color Scheme (Spotify Dark Theme)
- Background: `#121212`
- Card Background: `#282828`
- Header: `#181818`
- Accent (Green): `#1DB954`
- Text Primary: `#FFFFFF`
- Text Secondary: `#B3B3B3`
- Text Muted: `#727272`
- Error: `#E91E63`
- Warning: `#FFC107`

### Layout Constants
- Margin: 16px
- Card Radius: 12px
- Item Height: 60px
- Header Height: 60px

### Fonts
- Title: Montserrat 22
- Subtitle: Montserrat 16
- Body: Montserrat 14
- Small: Montserrat 12
- Icons: Montserrat 28

## Event Handling

### Touch Events
All screens use LVGL event callbacks:

```cpp
lv_obj_add_event_cb(button, [](lv_obj_t* obj, lv_event_t e) {
    if (e == LV_EVENT_CLICKED) {
        // Handle click
    }
}, LV_EVENT_CLICKED, userData);
```

### Back Navigation
Back button is present on all sub-screens:
- Main Settings → Now Playing
- Sub-screens → Previous screen (via MenuManager history)

## Build Configuration

### platformio.ini additions
```ini
lib_deps = 
    ArduinoJson@^6.21.0
    LittleFS_esp32@^1.0.6
```

### Memory Considerations
- Each screen is created lazily (on first access)
- Screens are cached after creation
- RuntimeConfig uses ~2KB of flash storage

## Testing Checklist

- [ ] Main settings screen displays all sections
- [ ] Navigation to each sub-screen works
- [ ] Back button navigates correctly
- [ ] WiFi scan finds networks
- [ ] Brightness slider updates display
- [ ] Screensaver toggle saves to config
- [ ] Spotify token refresh works
- [ ] About screen shows correct info
- [ ] Config persists across reboots

## Future Enhancements

1. **WiFi Password Entry**: On-screen keyboard for entering WiFi passwords
2. **Static IP Configuration**: Manual IP/subnet/gateway entry
3. **OTA Updates**: Firmware update from About screen
4. **Factory Reset**: Reset all settings from About screen
5. **Theme Selection**: Dark/Light theme toggle
6. **Export/Import Config**: Backup settings to/from SD card

## Changelog

### v1.0.0 (Phase 5)
- Initial settings screen implementation
- MenuManager navigation system
- RuntimeConfig persistent storage
- WiFi, Display, Spotify, About screens
- Loading/Success/Error overlays
