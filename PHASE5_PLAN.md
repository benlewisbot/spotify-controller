# Phase 5 Plan: Settings Screen & Menu System

## Overview

**Phase 5:** Settings Screen Implementation
**Timeline:** 2-3 Days
**Priority:** HIGH (Required for usability)
**Dependencies:** Phase 1-4 Complete

---

## Objectives

1. Create a flexible menu system for screen navigation
2. Implement Settings Screen with multiple configuration options
3. Add runtime configuration support (brightness, rotation, etc.)
4. Implement Spotify Token Management
5. Add WiFi Configuration UI
6. Create About Screen with device info

---

## Architecture Overview

### New Files to Create

```
include/
├── MenuManager.h          # Screen navigation system
├── SettingsScreen.h       # Settings UI screen
├── WiFiConfigScreen.h     # WiFi configuration
├── SpotifyTokenScreen.h   # Token management
└── AboutScreen.h          # Device info

src/
├── screens/
│   ├── SettingsScreen.cpp
│   ├── WiFiConfigScreen.cpp
│   ├── SpotifyTokenScreen.cpp
│   └── AboutScreen.cpp
└── menu/
    ├── MenuManager.cpp
    └── MenuItem.cpp
```

### Existing Files to Modify

```
include/spotify_gui.h      # Add screen switching support
src/spotify_gui.cpp        # Implement screen manager
include/config.h            # Add runtime config struct
src/app/App.cpp             # Initialize menu system
```

---

## Menu System Design

### Screen Types

```cpp
enum ScreenType {
    SCREEN_NOW_PLAYING,    // Main player screen
    SCREEN_PLAYLISTS,      // Playlist browser (Phase 6)
    SCREEN_SEARCH,         // Search (Phase 6)
    SCREEN_SETTINGS,       // Settings menu
    SCREEN_WIFI_CONFIG,    // WiFi configuration
    SCREEN_SPOTIFY_TOKEN,  // Token management
    SCREEN_ABOUT,          // About screen
    SCREEN_COUNT
};
```

### Menu Manager Interface

```cpp
#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <Arduino.h>
#include "spotify_gui.h"
#include "config.h"

// Forward declarations
class SettingsScreen;
class WiFiConfigScreen;
class SpotifyTokenScreen;
class AboutScreen;

enum NavigationAction {
    NAV_BACK,
    NAV_UP,
    NAV_DOWN,
    NAV_SELECT,
    NAV_MENU
};

class MenuManager {
private:
    ScreenType currentScreen;
    ScreenType previousScreen;
    bool inMenuMode;
    
    // Screen instances
    SettingsScreen* settingsScreen;
    WiFiConfigScreen* wifiScreen;
    SpotifyTokenScreen* tokenScreen;
    AboutScreen* aboutScreen;
    
public:
    MenuManager(SpotifyGUI& gui);
    
    void init();
    void update();
    
    // Navigation
    void navigateTo(ScreenType screen);
    void goBack();
    void handleTouch(int16_t x, int16_t y);
    
    // Screen Management
    void showScreen(ScreenType screen);
    void hideScreen(ScreenType screen);
    ScreenType getCurrentScreen() const;
    bool isInMenuMode() const;
    
    // Screen drawing delegates
    void drawCurrentScreen();
    void drawMenuButton();
    
    // Menu Button (top-left corner)
    bool isMenuButtonPressed(int16_t x, int16_t y);
    
private:
    void initScreens();
    void cleanupScreens();
};

#endif // MENU_MANAGER_H
```

---

## Settings Screen Design

### Layout

```
┌─────────────────────────────┐
│ ☰ Settings              ←   │  Header
├─────────────────────────────┤
│                             │
│  WiFi Configuration   >     │  Navigate to WiFi
│                             │
│  Spotify Account       >     │  Navigate to Token
│                             │
│  ─────────────────────────  │  Separator
│                             │
│  Display                     │
│  Brightness          ●○○○○   │  Slider
│  Rotation            ○●○○○   │  Toggle
│  Screensaver          [ON]   │  Switch
│  Timeout             5 min   │  Select
│                             │
│  ─────────────────────────  │  Separator
│                             │
│  About                >     │  Navigate to About
│                             │
│  Reboot              [OK]   │  Button
│                             │
└─────────────────────────────┘
```

### Settings Items Structure

```cpp
enum SettingsItemType {
    SETTINGS_ITEM_NAVIGATION,   // Navigate to submenu
    SETTINGS_ITEM_SLIDER,       // Slider control
    SETTINGS_ITEM_TOGGLE,       // Toggle switch
    SETTINGS_ITEM_SELECT,       // Select from list
    SETTINGS_ITEM_BUTTON,       // Button action
    SETTINGS_ITEM_SEPARATOR     // Visual separator
};

struct SettingsItem {
    const char* label;
    SettingsItemType type;
    
    union {
        struct {
            ScreenType targetScreen;
        } navigation;
        
        struct {
            int value;
            int min;
            int max;
            const char* unit;
        } slider;
        
        struct {
            bool enabled;
        } toggle;
        
        struct {
            int selectedIndex;
            const char* options[];
            int optionCount;
        } select;
        
        struct {
            void (*action)();
            const char* buttonText;
        } button;
    };
};

class SettingsScreen {
private:
    static const int MAX_ITEMS = 20;
    SettingsItem items[MAX_ITEMS];
    int itemCount;
    int selectedItem;
    int scrollOffset;
    
    // Touch areas
    struct {
        int x, y, width, height;
    } touchAreas[MAX_ITEMS];
    
public:
    SettingsScreen(TFT_eSPI& tft, DisplayManager& display);
    
    void init();
    void show();
    void hide();
    void update();
    
    void draw();
    void drawHeader();
    void drawItems();
    void drawBackButton();
    
    bool handleTouch(int16_t x, int16_t y);
    
    // Settings management
    void loadSettings();
    void saveSettings();
    
private:
    void setupItems();
    void updateTouchAreas();
    void drawItem(int index, int y);
    void drawSlider(const SettingsItem& item, int x, int y, bool selected);
    void drawToggle(const SettingsItem& item, int x, int y, bool selected);
    void drawSelect(const SettingsItem& item, int x, int y, bool selected);
    void drawButton(const SettingsItem& item, int x, int y, bool selected);
    void scrollToSelected();
};
```

### Settings Implementation

```cpp
void SettingsScreen::setupItems() {
    itemCount = 0;
    
    // WiFi Configuration
    items[itemCount++] = {
        .label = "WiFi Configuration",
        .type = SETTINGS_ITEM_NAVIGATION,
        .navigation = { .targetScreen = SCREEN_WIFI_CONFIG }
    };
    
    // Spotify Account
    items[itemCount++] = {
        .label = "Spotify Account",
        .type = SETTINGS_ITEM_NAVIGATION,
        .navigation = { .targetScreen = SCREEN_SPOTIFY_TOKEN }
    };
    
    // Separator
    items[itemCount++] = {
        .label = "─",
        .type = SETTINGS_ITEM_SEPARATOR
    };
    
    // Display Section Header (drawn as disabled item)
    items[itemCount++] = {
        .label = "Display",
        .type = SETTINGS_ITEM_SEPARATOR
    };
    
    // Brightness
    items[itemCount++] = {
        .label = "Brightness",
        .type = SETTINGS_ITEM_SLIDER,
        .slider = {
            .value = runtimeConfig.brightness,
            .min = 0,
            .max = 100,
            .unit = "%"
        }
    };
    
    // Rotation
    items[itemCount++] = {
        .label = "Rotation",
        .type = SETTINGS_ITEM_SELECT,
        .select = {
            .selectedIndex = runtimeConfig.rotation,
            .options = {"Portrait", "Landscape", "Portrait (Rev)", "Landscape (Rev)"},
            .optionCount = 4
        }
    };
    
    // Screensaver
    items[itemCount++] = {
        .label = "Screensaver",
        .type = SETTINGS_ITEM_TOGGLE,
        .toggle = { .enabled = runtimeConfig.screensaverEnabled }
    };
    
    // Screensaver Timeout
    if (runtimeConfig.screensaverEnabled) {
        items[itemCount++] = {
            .label = "Timeout",
            .type = SETTINGS_ITEM_SELECT,
            .select = {
                .selectedIndex = 0, // Calculate from runtimeConfig.screensaverTimeout
                .options = {"1 min", "2 min", "5 min", "10 min", "15 min", "30 min"},
                .optionCount = 6
            }
        };
    }
    
    // Separator
    items[itemCount++] = {
        .label = "─",
        .type = SETTINGS_ITEM_SEPARATOR
    };
    
    // About
    items[itemCount++] = {
        .label = "About",
        .type = SETTINGS_ITEM_NAVIGATION,
        .navigation = { .targetScreen = SCREEN_ABOUT }
    };
    
    // Reboot
    items[itemCount++] = {
        .label = "Reboot",
        .type = SETTINGS_ITEM_BUTTON,
        .button = {
            .action = rebootDevice,
            .buttonText = "REBOOT"
        }
    };
    
    selectedItem = 0;
}
```

---

## WiFi Configuration Screen

### Layout

```
┌─────────────────────────────┐
│ ← WiFi Settings            ✕ │  Header
├─────────────────────────────┤
│                             │
│  Status: Connected          │
│  Network: HomeWiFi_5G       │
│  IP: 192.168.1.100         │
│  Signal: Excellent (-42)    │
│                             │
│  ─────────────────────────  │
│                             │
│  Change Network             │
│  [Scan Networks]            │
│                             │
│  ─────────────────────────  │
│                             │
│  Static IP Settings         │
│  Use Static IP        [OFF] │
│  IP Address          [...]  │
│  Gateway             [...]  │
│  Subnet              255... │
│                             │
│  [Save & Reconnect]         │
│                             │
└─────────────────────────────┘
```

### WiFi Configuration Implementation

```cpp
class WiFiConfigScreen {
private:
    enum WiFiState {
        WIFI_STATUS,
        WIFI_SCAN,
        WIFI_CONNECTING,
        WIFI_EDIT_STATIC
    };
    
    WiFiState currentState;
    String scannedNetworks[20];
    int networkCount;
    int selectedNetwork;
    
    // Static IP fields
    char ipAddress[16];
    char gateway[16];
    char subnet[16];
    
public:
    WiFiConfigScreen(TFT_eSPI& tft, DisplayManager& display);
    
    void init();
    void show();
    void hide();
    void update();
    
    void draw();
    void drawStatus();
    void drawNetworkList();
    void drawStaticIPConfig();
    
    bool handleTouch(int16_t x, int16_t y);
    
    void scanNetworks();
    void connectToNetwork(const char* ssid, const char* password);
    void saveConfig();
    
private:
    void drawNetworkItem(int index, int y, bool selected);
    int getSignalStrength(int rssi);
    const char* getSignalDescription(int rssi);
};
```

---

## Spotify Token Screen

### Layout

```
┌─────────────────────────────┐
│ ← Spotify Token           ✕ │  Header
├─────────────────────────────┤
│                             │
│  Account Status             │
│  ● Connected                │
│  User: ben@email.com        │
│  Premium: Yes               │
│                             │
│  ─────────────────────────  │
│                             │
│  Token Info                 │
│  Access Token: Valid        │
│  Expires in: 58 min         │
│  Refresh Token: Valid       │
│                             │
│  ─────────────────────────  │
│                             │
│  Re-authenticate            │
│  [Login with Spotify]       │
│                             │
│  [Clear Credentials]        │
│                             │
└─────────────────────────────┘
```

### Spotify Token Implementation

```cpp
class SpotifyTokenScreen {
private:
    enum TokenState {
        TOKEN_STATUS,
        TOKEN_LOGIN,
        TOKEN_CONFIRM_CLEAR
    };
    
    TokenState currentState;
    
    // Login flow
    String authUrl;
    bool waitingForCallback;
    
public:
    SpotifyTokenScreen(TFT_eSPI& tft, DisplayManager& display);
    
    void init();
    void show();
    void hide();
    void update();
    
    void draw();
    void drawStatus();
    void drawLogin();
    void drawConfirmClear();
    
    bool handleTouch(int16_t x, int16_t y);
    
    void startAuthentication();
    void checkAuthCallback();
    void clearCredentials();
    void refreshToken();
    
private:
    bool isTokenValid();
    String getTokenExpiryTime();
    void showQRCode(const String& url);
};
```

---

## About Screen

### Layout

```
┌─────────────────────────────┐
│ ← About                  ✕ │  Header
├─────────────────────────────┤
│                             │
│         SPOTIFY             │
│         CONTROLLER          │
│                             │
│  Version: 1.0.0             │
│  Build: 2026-02-12          │
│                             │
│  ─────────────────────────  │
│                             │
│  Hardware                   │
│  Board: ESP32-WROVER       │
│  CPU: Dual-core 240MHz     │
│  RAM: 520KB + 8MB PSRAM    │
│                             │
│  Display                    │
│  Driver: ST7789             │
│  Size: 320x480             │
│  Touch: FT6236 (Capacitive)│
│                             │
│  ─────────────────────────  │
│                             │
│  Network                    │
│  WiFi: Connected            │
│  IP: 192.168.1.100         │
│  MAC: AA:BB:CC:DD:EE:FF    │
│                             │
│  ─────────────────────────  │
│                             │
│  GitHub:                    │
│  BenLewisbot/               │
│  spotify-controller-esp32   │
│                             │
└─────────────────────────────┘
```

### About Screen Implementation

```cpp
class AboutScreen {
private:
    struct SystemInfo {
        String version;
        String buildDate;
        String board;
        String cpu;
        String ram;
        String displayDriver;
        String displaySize;
        String touchController;
        String wifiStatus;
        String ipAddress;
        String macAddress;
        String gitUrl;
    } info;
    
public:
    AboutScreen(TFT_eSPI& tft, DisplayManager& display);
    
    void init();
    void show();
    void hide();
    void update();
    
    void draw();
    void drawLogo();
    void drawInfo();
    
    bool handleTouch(int16_t x, int16_t y);
    
private:
    void gatherSystemInfo();
    void drawInfoRow(const String& label, const String& value, int y);
    void drawSeparator(int y);
};
```

---

## Runtime Configuration

### Runtime Config Structure

```cpp
// In config.h

struct RuntimeConfig {
    // Display
    int brightness;              // 0-100
    int rotation;               // 0-3
    bool screensaverEnabled;    // true/false
    int screensaverTimeout;     // minutes
    
    // Audio
    int volumeLimit;            // 0-100 (optional)
    
    // WiFi
    bool useStaticIP;
    String staticIP;
    String gateway;
    String subnet;
    
    // UI
    bool animationsEnabled;    // true/false
    int touchSensitivity;      // 1-10
};

// Global instance
extern RuntimeConfig runtimeConfig;

// Functions to manage runtime config
void loadRuntimeConfig();
void saveRuntimeConfig();
void applyRuntimeConfig();
```

### Runtime Config Implementation

```cpp
// In src/app/Config.cpp

RuntimeConfig runtimeConfig;

void loadRuntimeConfig() {
    File file = LittleFS.open("/runtime_config.json", "r");
    if (!file) {
        // Set defaults
        runtimeConfig.brightness = 75;
        runtimeConfig.rotation = 1; // Landscape
        runtimeConfig.screensaverEnabled = true;
        runtimeConfig.screensaverTimeout = 5;
        runtimeConfig.volumeLimit = 100;
        runtimeConfig.useStaticIP = false;
        runtimeConfig.animationsEnabled = true;
        runtimeConfig.touchSensitivity = 5;
        return;
    }
    
    // Parse JSON
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, file);
    file.close();
    
    runtimeConfig.brightness = doc["brightness"] | 75;
    runtimeConfig.rotation = doc["rotation"] | 1;
    runtimeConfig.screensaverEnabled = doc["screensaverEnabled"] | true;
    runtimeConfig.screensaverTimeout = doc["screensaverTimeout"] | 5;
    runtimeConfig.volumeLimit = doc["volumeLimit"] | 100;
    runtimeConfig.useStaticIP = doc["useStaticIP"] | false;
    runtimeConfig.animationsEnabled = doc["animationsEnabled"] | true;
    runtimeConfig.touchSensitivity = doc["touchSensitivity"] | 5;
}

void saveRuntimeConfig() {
    DynamicJsonDocument doc(1024);
    
    doc["brightness"] = runtimeConfig.brightness;
    doc["rotation"] = runtimeConfig.rotation;
    doc["screensaverEnabled"] = runtimeConfig.screensaverEnabled;
    doc["screensaverTimeout"] = runtimeConfig.screensaverTimeout;
    doc["volumeLimit"] = runtimeConfig.volumeLimit;
    doc["useStaticIP"] = runtimeConfig.useStaticIP;
    doc["staticIP"] = runtimeConfig.staticIP;
    doc["gateway"] = runtimeConfig.gateway;
    doc["subnet"] = runtimeConfig.subnet;
    doc["animationsEnabled"] = runtimeConfig.animationsEnabled;
    doc["touchSensitivity"] = runtimeConfig.touchSensitivity;
    
    File file = LittleFS.open("/runtime_config.json", "w");
    serializeJson(doc, file);
    file.close();
}

void applyRuntimeConfig() {
    // Apply brightness
    analogWrite(PIN_BACKLIGHT, map(runtimeConfig.brightness, 0, 100, 0, 255));
    
    // Apply rotation
    displayManager.setRotation(runtimeConfig.rotation);
    
    // Apply WiFi static IP
    if (runtimeConfig.useStaticIP) {
        // Configure static IP
        WiFi.config(
            IPAddress(runtimeConfig.staticIP),
            IPAddress(runtimeConfig.gateway),
            IPAddress(runtimeConfig.subnet)
        );
    }
}
```

---

## Integration with Existing Code

### Modify spotify_gui.h

```cpp
class SpotifyGUI {
private:
    // ... existing members ...
    
    MenuManager* menuManager;
    bool inSettingsMode;
    
public:
    // ... existing methods ...
    
    // Menu integration
    void setMenuManager(MenuManager* manager);
    void showMenuButton();
    void hideMenuButton();
    void enterSettingsMode();
    void exitSettingsMode();
    void drawMenuOverlay();
};
```

### Modify App.cpp

```cpp
void App::setup() {
    // ... existing setup ...
    
    // Initialize runtime config
    loadRuntimeConfig();
    applyRuntimeConfig();
    
    // Initialize menu manager
    menuManager = new MenuManager(gui);
    menuManager->init();
    gui.setMenuManager(menuManager);
}

void App::loop() {
    // Handle touch
    if (touchManager.isPressed()) {
        TouchPoint point = touchManager.getTouchPoint();
        
        // First check menu button
        if (menuManager && menuManager.isMenuButtonPressed(point.x, point.y)) {
            menuManager.navigateTo(SCREEN_SETTINGS);
            return;
        }
        
        // Then handle current screen
        if (menuManager && menuManager.isInMenuMode()) {
            menuManager.handleTouch(point.x, point.y);
        } else {
            // Handle now playing screen touch
            // ... existing touch handling ...
        }
    }
    
    // Update screens
    if (menuManager && menuManager.isInMenuMode()) {
        menuManager.update();
    } else {
        // ... existing now playing update ...
    }
    
    // ... existing loop ...
}
```

---

## Touch Interaction Design

### Settings Screen Touch Areas

```
┌─────────────────────────────┐
│☰ [MENU BTN]              ← │  [x:0-50, y:0-40]
├─────────────────────────────┤
│ [ITEM 1]           >        │  [x:10-470, y:50-90]
│ [ITEM 2]           >        │  [x:10-470, y:95-135]
│ [ITEM 3]           ●○○○○    │  [x:10-470, y:140-180]
│ [ITEM 4]           [ON]     │  [x:10-470, y:185-225]
│                             │
│ [REBOOT]           [BTN]    │  [x:10-470, y:400-440]
└─────────────────────────────┘
```

### Touch Handling Flow

```cpp
bool SettingsScreen::handleTouch(int16_t x, int16_t y) {
    // Check back button
    if (x < 50 && y < 40) {
        menuManager->goBack();
        return true;
    }
    
    // Check items
    for (int i = 0; i < itemCount; i++) {
        if (x >= touchAreas[i].x && x <= touchAreas[i].x + touchAreas[i].width &&
            y >= touchAreas[i].y && y <= touchAreas[i].y + touchAreas[i].height) {
            
            handleItemTouch(i);
            return true;
        }
    }
    
    return false;
}

void SettingsScreen::handleItemTouch(int index) {
    SettingsItem& item = items[index];
    
    switch (item.type) {
        case SETTINGS_ITEM_NAVIGATION:
            menuManager->navigateTo(item.navigation.targetScreen);
            break;
            
        case SETTINGS_ITEM_SLIDER:
            // Show slider control overlay
            showSliderControl(index);
            break;
            
        case SETTINGS_ITEM_TOGGLE:
            item.toggle.enabled = !item.toggle.enabled;
            // Update runtime config
            if (String(item.label) == "Screensaver") {
                runtimeConfig.screensaverEnabled = item.toggle.enabled;
            }
            saveRuntimeConfig();
            break;
            
        case SETTINGS_ITEM_SELECT:
            // Show selection popup
            showSelectionPopup(index);
            break;
            
        case SETTINGS_ITEM_BUTTON:
            if (item.button.action) {
                item.button.action();
            }
            break;
            
        case SETTINGS_ITEM_SEPARATOR:
            // Do nothing
            break;
    }
}
```

---

## Code Skeleton: MenuManager

```cpp
#include "MenuManager.h"
#include "SettingsScreen.h"
#include "WiFiConfigScreen.h"
#include "SpotifyTokenScreen.h"
#include "AboutScreen.h"

MenuManager::MenuManager(SpotifyGUI& gui) 
    : currentScreen(SCREEN_NOW_PLAYING), 
      previousScreen(SCREEN_NOW_PLAYING),
      inMenuMode(false),
      gui(gui) {
}

void MenuManager::init() {
    Serial.println("📋 MenuManager initialisiert");
    initScreens();
}

void MenuManager::initScreens() {
    // Initialize all screens
    settingsScreen = new SettingsScreen(gui.getTFT(), gui.getDisplayManager());
    settingsScreen->init();
    
    wifiScreen = new WiFiConfigScreen(gui.getTFT(), gui.getDisplayManager());
    wifiScreen->init();
    
    tokenScreen = new SpotifyTokenScreen(gui.getTFT(), gui.getDisplayManager());
    tokenScreen->init();
    
    aboutScreen = new AboutScreen(gui.getTFT(), gui.getDisplayManager());
    aboutScreen->init();
}

void MenuManager::navigateTo(ScreenType screen) {
    previousScreen = currentScreen;
    currentScreen = screen;
    
    if (screen != SCREEN_NOW_PLAYING) {
        inMenuMode = true;
    } else {
        inMenuMode = false;
    }
    
    Serial.printf("📋 Navigation: %s → %s\n", 
        getScreenName(previousScreen),
        getScreenName(currentScreen));
    
    showScreen(screen);
}

void MenuManager::goBack() {
    if (previousScreen != SCREEN_NOW_PLAYING) {
        navigateTo(previousScreen);
    } else {
        navigateTo(SCREEN_NOW_PLAYING);
    }
}

void MenuManager::showScreen(ScreenType screen) {
    // Hide all screens
    if (settingsScreen) settingsScreen->hide();
    if (wifiScreen) wifiScreen->hide();
    if (tokenScreen) tokenScreen->hide();
    if (aboutScreen) aboutScreen->hide();
    
    // Show target screen
    switch (screen) {
        case SCREEN_SETTINGS:
            gui.exitSettingsMode();
            settingsScreen->show();
            break;
        case SCREEN_WIFI_CONFIG:
            wifiScreen->show();
            break;
        case SCREEN_SPOTIFY_TOKEN:
            tokenScreen->show();
            break;
        case SCREEN_ABOUT:
            aboutScreen->show();
            break;
        case SCREEN_NOW_PLAYING:
            gui.enterSettingsMode();
            gui.showMenuButton();
            break;
        default:
            break;
    }
}

void MenuManager::handleTouch(int16_t x, int16_t y) {
    switch (currentScreen) {
        case SCREEN_SETTINGS:
            settingsScreen->handleTouch(x, y);
            break;
        case SCREEN_WIFI_CONFIG:
            wifiScreen->handleTouch(x, y);
            break;
        case SCREEN_SPOTIFY_TOKEN:
            tokenScreen->handleTouch(x, y);
            break;
        case SCREEN_ABOUT:
            aboutScreen->handleTouch(x, y);
            break;
        default:
            break;
    }
}

const char* MenuManager::getScreenName(ScreenType screen) {
    switch (screen) {
        case SCREEN_NOW_PLAYING: return "Now Playing";
        case SCREEN_SETTINGS: return "Settings";
        case SCREEN_WIFI_CONFIG: return "WiFi Config";
        case SCREEN_SPOTIFY_TOKEN: return "Spotify Token";
        case SCREEN_ABOUT: return "About";
        default: return "Unknown";
    }
}
```

---

## Implementation Tasks

### Day 1: Menu System & Basic Settings
- [ ] Create MenuManager.h and MenuManager.cpp
- [ ] Create SettingsScreen.h and SettingsScreen.cpp
- [ ] Implement basic navigation (Now Playing ↔ Settings)
- [ ] Draw Settings screen header and menu button
- [ ] Implement Settings item structure
- [ ] Add Settings items (WiFi, Spotify, Display separator)

### Day 2: Settings Functionality
- [ ] Implement Brightness slider
- [ ] Implement Rotation selector
- [ ] Implement Screensaver toggle
- [ ] Implement Screensaver timeout selector
- [ ] Create RuntimeConfig structure
- [ ] Implement load/save config functions
- [ ] Add About screen
- [ ] Implement device info display

### Day 3: WiFi & Token Screens
- [ ] Create WiFiConfigScreen
- [ ] Implement WiFi status display
- [ ] Implement network scanning
- [ ] Implement static IP configuration
- [ ] Create SpotifyTokenScreen
- [ ] Implement token status display
- [ ] Implement authentication flow
- [ ] Add clear credentials option

---

## Testing Checklist

### Menu System
- [ ] Navigate from Now Playing to Settings
- [ ] Navigate back from Settings to Now Playing
- [ ] Navigate between sub-settings screens
- [ ] Test back button functionality
- [ ] Test menu button in Now Playing screen

### Settings Screen
- [ ] Brightness slider works and saves
- [ ] Rotation selector works and applies
- [ ] Screensaver toggle works
- [ ] Screensaver timeout selector works
- [ ] All items are touchable
- [ ] Scrollable list works if items exceed screen

### WiFi Configuration
- [ ] WiFi status displays correctly
- [ ] Network scanning works
- [ ] Connect to new network works
- [ ] Static IP configuration saves
- [ ] Reconnect after config change works

### Spotify Token
- [ ] Token status displays correctly
- [ ] Authentication flow starts
- [ ] Token refresh works
- [ ] Clear credentials works
- [ ] Re-authentication after clear works

### About Screen
- [ ] All system info displays correctly
- [ ] Version number is correct
- [ ] Hardware info is accurate
- [ ] Network info is current

### Runtime Config
- [ ] Settings save to file
- [ ] Settings load on boot
- [ ] Brightness applies immediately
- [ ] Rotation applies immediately
- [ ] Changes persist after reboot

---

## Known Challenges & Solutions

### Challenge 1: Screen Memory Management
**Problem:** Multiple screens in memory may exceed RAM
**Solution:** Only keep active screen in memory, or use PSRAM for screen buffers

### Challenge 2: Touch Area Overlaps
**Problem:** Menu button and list items may overlap
**Solution:** Define clear touch zones, test boundaries

### Challenge 3: Settings Persistence
**Problem:** LittleFS may fail to write
**Solution:** Add error handling, show error message to user

### Challenge 4: WiFi Scan Performance
**Problem:** Scanning blocks UI
**Solution:** Scan in background, show progress indicator

### Challenge 5: Spotify Auth Flow
**Problem:** Need to redirect to Spotify web page
**Solution:** Show QR code, start local web server for callback

---

## Next Steps (Phase 6)

After Phase 5 completion:
1. Implement Playlist Browser screen
2. Implement Track List screen
3. Implement Search screen with keyboard
4. Add offline cache (optional)

---

**Generated:** 2026-02-12
**Author:** Subagent (Phase 5 Planning)
**Status:** Ready for Implementation
