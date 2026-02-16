# Phase 6 Plan: Playlist Browser & Search

## Overview

**Phase 6:** Playlist Browser & Search Features
**Timeline:** 3-4 Days
**Priority:** MEDIUM (Enhanced Features)
**Dependencies:** Phase 5 Complete, Spotify API Integration

---

## Objectives

1. Implement Playlist Browser with full navigation
2. Add Track List view for individual playlists
3. Implement Search functionality with on-screen keyboard
4. Add search results display (tracks, artists, albums)
5. Implement swipe gestures for navigation
6. Add playlist creation (optional - Phase 6+)

---

## Architecture Overview

### New Files to Create

```
include/
├── PlaylistBrowserScreen.h    # Main playlist list
├── TrackListScreen.h          # Individual playlist tracks
├── SearchScreen.h             # Search UI + keyboard
├── OnScreenKeyboard.h        # Virtual keyboard component
├── SwipeGestures.h            # Gesture recognition
└── PlaylistManager.h          # Playlist data management

src/
├── screens/
│   ├── PlaylistBrowserScreen.cpp
│   ├── TrackListScreen.cpp
│   └── SearchScreen.cpp
├── ui/
│   ├── OnScreenKeyboard.cpp
│   └── SwipeGestures.cpp
└── spotify/
    └── PlaylistManager.cpp
```

### Existing Files to Modify

```
include/spotify_manager.h     # Add playlist/track endpoints
src/spotify_manager.cpp        # Implement API calls
include/MenuManager.h          # Add new screen types
include/models.h               # Add playlist/track structures
```

---

## Screen Flow Diagram

```
Now Playing Screen
    ↓ (Menu → Playlists)
Playlist Browser
    ↓ (Tap playlist)
Track List
    ↓ (Tap track)              → Playback starts
    ↓ (Back)                   → Playlist Browser
    ↓ (Back)                   → Now Playing
    
Playlist Browser
    ↓ (Search icon)
Search Screen
    ↓ (Type query)             → Keyboard appears
    ↓ (Enter/Search)           → Results appear
    ↓ (Tap track)              → Playback starts
    ↓ (Tap artist)             → Artist view (future)
    ↓ (Back)                   → Playlist Browser
```

---

## Playlist Browser Screen

### Layout

```
┌─────────────────────────────┐
│ ← Playlists            🔍   │  Header
├─────────────────────────────┤
│  ┌───────────────────────┐  │
│  │ [IMG] Liked Songs     │  │  Playlist item 1
│  │       234 songs       │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ [IMG] Discover Weekly │  │  Playlist item 2
│  │       Spotify         │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ [IMG] 2026 Favorites  │  │  Playlist item 3
│  │       Ben             │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ [IMG] Rock Classics   │  │  Playlist item 4
│  │       156 songs       │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ [IMG] Focus Music    │  │  Playlist item 5
│  │       89 songs        │  │
│  └───────────────────────┘  │
│                             │
│         ▼ Scroll ▼          │  Scroll indicator
└─────────────────────────────┘
```

### Data Structure

```cpp
// In models.h

struct Playlist {
    String id;
    String name;
    String description;
    String owner;
    String imageUrl;
    bool isPublic;
    int trackCount;
    bool isCollaborative;
    
    // Local cache
    bool tracksLoaded;
    std::vector<Track> tracks;
};

struct PlaylistList {
    std::vector<Playlist> playlists;
    int total;
    int offset;
    int limit;
    bool hasMore;
};
```

### Playlist Browser Implementation

```cpp
#ifndef PLAYLIST_BROWSER_SCREEN_H
#define PLAYLIST_BROWSER_SCREEN_H

#include <Arduino.h>
#include <vector>
#include "spotify_gui.h"
#include "models.h"

class PlaylistBrowserScreen {
private:
    static const int PLAYLISTS_PER_PAGE = 6;
    static const int ITEM_HEIGHT = 80;
    static const int ITEM_MARGIN = 10;
    
    std::vector<Playlist> playlists;
    int selectedPlaylist;
    int scrollOffset;
    bool isLoading;
    String errorMessage;
    
    // Touch areas
    struct {
        int x, y, width, height;
    } backBtn, searchBtn;
    
    struct {
        int x, y, width, height;
    } playlistAreas[PLAYLISTS_PER_PAGE];
    
public:
    PlaylistBrowserScreen(TFT_eSPI& tft, DisplayManager& display);
    
    void init();
    void show();
    void hide();
    void update();
    
    void draw();
    void drawHeader();
    void drawPlaylists();
    void drawPlaylistItem(const Playlist& playlist, int x, int y, bool selected);
    void drawLoading();
    void drawError();
    
    bool handleTouch(int16_t x, int16_t y);
    
    // Data management
    void loadPlaylists();
    void loadMorePlaylists();
    void refreshPlaylists();
    
    // Navigation
    void selectPlaylist(int index);
    void openPlaylist(const Playlist& playlist);
    void goToSearch();
    
private:
    void updateTouchAreas();
    void drawThumbnail(const String& imageUrl, int x, int y, int size);
    void scrollToPlaylist(int index);
    void drawEmptyState();
};

#endif // PLAYLIST_BROWSER_SCREEN_H
```

### Playlist Browser Implementation

```cpp
#include "PlaylistBrowserScreen.h"
#include "PlaylistManager.h"
#include "SpotifyClient.h"

PlaylistBrowserScreen::PlaylistBrowserScreen(TFT_eSPI& tft, DisplayManager& display)
    : tft(tft), display(display), selectedPlaylist(-1), scrollOffset(0), 
      isLoading(false) {
}

void PlaylistBrowserScreen::init() {
    Serial.println("📚 PlaylistBrowser initialisiert");
    
    // Set up touch areas
    backBtn = {10, 10, 80, 40};
    searchBtn = {DISPLAY_WIDTH - 90, 10, 80, 40};
}

void PlaylistBrowserScreen::show() {
    Serial.println("📚 Zeige Playlists");
    
    // Load playlists if empty
    if (playlists.empty()) {
        loadPlaylists();
    }
    
    draw();
}

void PlaylistBrowserScreen::loadPlaylists() {
    isLoading = true;
    draw();
    
    Serial.println("📚 Lade Playlists von Spotify...");
    
    // Get playlists from Spotify API
    PlaylistList playlistList;
    bool success = PlaylistManager::getUserPlaylists(playlistList);
    
    if (success) {
        playlists = playlistList.playlists;
        scrollOffset = 0;
        Serial.printf("📚 %d Playlists geladen\n", playlists.size());
    } else {
        errorMessage = "Fehler beim Laden";
        Serial.println("❌ Fehler beim Laden der Playlists");
    }
    
    isLoading = false;
    draw();
}

void PlaylistBrowserScreen::draw() {
    tft.fillScreen(COLOR_BG);
    
    if (isLoading) {
        drawLoading();
        return;
    }
    
    if (!errorMessage.isEmpty()) {
        drawError();
        return;
    }
    
    if (playlists.empty()) {
        drawEmptyState();
        return;
    }
    
    drawHeader();
    drawPlaylists();
}

void PlaylistBrowserScreen::drawHeader() {
    // Back button
    tft.fillRoundRect(backBtn.x, backBtn.y, backBtn.width, backBtn.height, 5, COLOR_SURFACE);
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(backBtn.x + 25, backBtn.y + 10);
    tft.print("←");
    
    // Title
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(90, backBtn.y + 10);
    tft.print("Playlists");
    
    // Search button
    tft.fillRoundRect(searchBtn.x, searchBtn.y, searchBtn.width, searchBtn.height, 5, COLOR_SPOTIFY_GREEN);
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(searchBtn.x + 25, searchBtn.y + 10);
    tft.print("🔍");
}

void PlaylistBrowserScreen::drawPlaylists() {
    int startY = 60;
    int visibleCount = min(PLAYLISTS_PER_PAGE, (int)playlists.size() - scrollOffset);
    
    for (int i = 0; i < visibleCount; i++) {
        int index = scrollOffset + i;
        const Playlist& playlist = playlists[index];
        int y = startY + i * (ITEM_HEIGHT + ITEM_MARGIN);
        
        bool selected = (index == selectedPlaylist);
        drawPlaylistItem(playlist, 10, y, selected);
    }
    
    updateTouchAreas();
}

void PlaylistBrowserScreen::drawPlaylistItem(const Playlist& playlist, int x, int y, bool selected) {
    // Background
    uint16_t bgColor = selected ? COLOR_SPOTIFY_GREEN : COLOR_SURFACE;
    tft.fillRoundRect(x, y, DISPLAY_WIDTH - 20, ITEM_HEIGHT, 10, bgColor);
    
    // Thumbnail placeholder
    int thumbSize = 60;
    int thumbX = x + 10;
    int thumbY = y + 10;
    
    if (!playlist.imageUrl.isEmpty()) {
        drawThumbnail(playlist.imageUrl, thumbX, thumbY, thumbSize);
    } else {
        // Draw placeholder
        tft.fillRoundRect(thumbX, thumbY, thumbSize, thumbSize, 5, COLOR_TEXT_SECONDARY);
        tft.setTextColor(COLOR_BG);
        tft.setTextSize(3);
        tft.setCursor(thumbX + 15, thumbY + 15);
        tft.print("♪");
    }
    
    // Playlist name
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(thumbX + thumbSize + 15, y + 15);
    String displayName = playlist.name;
    if (displayName.length() > 20) {
        displayName = displayName.substring(0, 17) + "...";
    }
    tft.print(displayName);
    
    // Owner/Description
    tft.setTextColor(COLOR_TEXT_SECONDARY);
    tft.setTextSize(1);
    tft.setCursor(thumbX + thumbSize + 15, y + 40);
    String info = playlist.owner;
    if (!info.isEmpty()) {
        info += " • ";
    }
    info += String(playlist.trackCount) + " songs";
    tft.print(info);
}

void PlaylistBrowserScreen::drawThumbnail(const String& imageUrl, int x, int y, int size) {
    // In a real implementation, this would:
    // 1. Check if image is cached
    // 2. Download if not cached
    // 3. Scale and display
    
    // For now, draw placeholder
    tft.fillRoundRect(x, y, size, size, 5, COLOR_TEXT_SECONDARY);
    tft.setTextColor(COLOR_BG);
    tft.setTextSize(3);
    tft.setCursor(x + 15, y + 15);
    tft.print("♪");
}

void PlaylistBrowserScreen::drawLoading() {
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(100, DISPLAY_HEIGHT / 2 - 10);
    tft.print("Laden...");
}

void PlaylistBrowserScreen::drawError() {
    tft.setTextColor(COLOR_ERROR);
    tft.setTextSize(2);
    tft.setCursor(50, DISPLAY_HEIGHT / 2 - 10);
    tft.print(errorMessage);
    
    tft.setTextColor(COLOR_TEXT_SECONDARY);
    tft.setTextSize(1);
    tft.setCursor(80, DISPLAY_HEIGHT / 2 + 20);
    tft.print("Tippe zum Neuladen");
}

void PlaylistBrowserScreen::drawEmptyState() {
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(100, DISPLAY_HEIGHT / 2 - 20);
    tft.print("Keine Playlists");
    
    tft.setTextColor(COLOR_TEXT_SECONDARY);
    tft.setTextSize(1);
    tft.setCursor(50, DISPLAY_HEIGHT / 2 + 20);
    tft.print("Erstelle Playlists in Spotify");
}

bool PlaylistBrowserScreen::handleTouch(int16_t x, int16_t y) {
    // Check back button
    if (x >= backBtn.x && x <= backBtn.x + backBtn.width &&
        y >= backBtn.y && y <= backBtn.y + backBtn.height) {
        menuManager->goBack();
        return true;
    }
    
    // Check search button
    if (x >= searchBtn.x && x <= searchBtn.x + searchBtn.width &&
        y >= searchBtn.y && y <= searchBtn.y + searchBtn.height) {
        goToSearch();
        return true;
    }
    
    // Check playlist items
    for (int i = 0; i < PLAYLISTS_PER_PAGE; i++) {
        int index = scrollOffset + i;
        if (index >= playlists.size()) break;
        
        if (x >= playlistAreas[i].x && x <= playlistAreas[i].x + playlistAreas[i].width &&
            y >= playlistAreas[i].y && y <= playlistAreas[i].y + playlistAreas[i].height) {
            
            selectedPlaylist = index;
            draw();
            
            // Open playlist on tap
            openPlaylist(playlists[index]);
            return true;
        }
    }
    
    // Check if empty state was tapped
    if (playlists.empty() && !isLoading && errorMessage.isEmpty()) {
        loadPlaylists();
        return true;
    }
    
    // Check if error state was tapped
    if (!errorMessage.isEmpty()) {
        loadPlaylists();
        return true;
    }
    
    return false;
}

void PlaylistBrowserScreen::openPlaylist(const Playlist& playlist) {
    Serial.printf("📚 Öffne Playlist: %s\n", playlist.name.c_str());
    
    // Navigate to track list screen
    menuManager->navigateTo(SCREEN_TRACK_LIST);
    
    // Load playlist tracks
    TrackListScreen* trackScreen = menuManager->getTrackListScreen();
    if (trackScreen) {
        trackScreen->loadPlaylist(playlist);
    }
}

void PlaylistBrowserScreen::goToSearch() {
    Serial.println("📚 Gehe zu Suche");
    menuManager->navigateTo(SCREEN_SEARCH);
}

void PlaylistBrowserScreen::updateTouchAreas() {
    int startY = 60;
    int visibleCount = min(PLAYLISTS_PER_PAGE, (int)playlists.size() - scrollOffset);
    
    for (int i = 0; i < visibleCount; i++) {
        playlistAreas[i] = {
            10,
            startY + i * (ITEM_HEIGHT + ITEM_MARGIN),
            DISPLAY_WIDTH - 20,
            ITEM_HEIGHT
        };
    }
}
```

---

## Track List Screen

### Layout

```
┌─────────────────────────────┐
│ ← Discover Weekly      ⚙️   │  Header
├─────────────────────────────┤
│  ┌───────────────────────┐  │
│  │ 1. Song Title        │  │  Track 1
│  │    Artist Name       │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ 2. Another Song      │  │  Track 2
│  │    Another Artist    │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ 3. Third Song        │  │  Track 3
│  │    Third Artist      │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ 4. Fourth Song       │  │  Track 4 (selected)
│  │    Fourth Artist     │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ 5. Fifth Song        │  │  Track 5
│  │    Fifth Artist      │  │
│  └───────────────────────┘  │
│                             │
│         ▼ Scroll ▼          │
└─────────────────────────────┘
```

### Track List Implementation

```cpp
#ifndef TRACK_LIST_SCREEN_H
#define TRACK_LIST_SCREEN_H

#include <Arduino.h>
#include <vector>
#include "spotify_gui.h"
#include "models.h"

class TrackListScreen {
private:
    static const int TRACKS_PER_PAGE = 7;
    static const int ITEM_HEIGHT = 60;
    
    Playlist currentPlaylist;
    std::vector<Track> tracks;
    int selectedTrack;
    int scrollOffset;
    bool isLoading;
    String errorMessage;
    
    // Touch areas
    struct {
        int x, y, width, height;
    } backBtn, shuffleBtn, playAllBtn;
    
    struct {
        int x, y, width, height;
    } trackAreas[TRACKS_PER_PAGE];
    
public:
    TrackListScreen(TFT_eSPI& tft, DisplayManager& display);
    
    void init();
    void show();
    void hide();
    void update();
    
    void draw();
    void drawHeader();
    void drawTracks();
    void drawTrackItem(const Track& track, int index, int x, int y, bool selected);
    void drawLoading();
    void drawError();
    
    bool handleTouch(int16_t x, int16_t y);
    
    // Data management
    void loadPlaylist(const Playlist& playlist);
    void loadTracks(const String& playlistId);
    void refreshTracks();
    
    // Playback
    void playTrack(int index);
    void playAll();
    void shufflePlay();
    
private:
    void updateTouchAreas();
    void drawTrackNumber(int number, int x, int y);
    void drawEmptyState();
};

#endif // TRACK_LIST_SCREEN_H
```

### Track List Implementation

```cpp
#include "TrackListScreen.h"
#include "PlaylistManager.h"
#include "SpotifyClient.h"
#include "PlaybackController.h"

TrackListScreen::TrackListScreen(TFT_eSPI& tft, DisplayManager& display)
    : tft(tft), display(display), selectedTrack(-1), scrollOffset(0), 
      isLoading(false) {
}

void TrackListScreen::init() {
    Serial.println("🎵 TrackList initialisiert");
    
    // Set up touch areas
    backBtn = {10, 10, 80, 40};
    shuffleBtn = {DISPLAY_WIDTH - 180, 10, 80, 40};
    playAllBtn = {DISPLAY_WIDTH - 90, 10, 80, 40};
}

void TrackListScreen::show() {
    Serial.printf("🎵 Zeige Tracks: %s\n", currentPlaylist.name.c_str());
    draw();
}

void TrackListScreen::loadPlaylist(const Playlist& playlist) {
    currentPlaylist = playlist;
    tracks.clear();
    selectedTrack = -1;
    scrollOffset = 0;
    
    if (!playlist.tracksLoaded) {
        loadTracks(playlist.id);
    } else {
        tracks = playlist.tracks;
        draw();
    }
}

void TrackListScreen::loadTracks(const String& playlistId) {
    isLoading = true;
    draw();
    
    Serial.printf("🎵 Lade Tracks für Playlist: %s\n", playlistId.c_str());
    
    // Get tracks from Spotify API
    bool success = PlaylistManager::getPlaylistTracks(playlistId, tracks);
    
    if (success) {
        Serial.printf("🎵 %d Tracks geladen\n", tracks.size());
    } else {
        errorMessage = "Fehler beim Laden";
        Serial.println("❌ Fehler beim Laden der Tracks");
    }
    
    isLoading = false;
    draw();
}

void TrackListScreen::draw() {
    tft.fillScreen(COLOR_BG);
    
    if (isLoading) {
        drawLoading();
        return;
    }
    
    if (!errorMessage.isEmpty()) {
        drawError();
        return;
    }
    
    if (tracks.empty()) {
        drawEmptyState();
        return;
    }
    
    drawHeader();
    drawTracks();
}

void TrackListScreen::drawHeader() {
    // Back button
    tft.fillRoundRect(backBtn.x, backBtn.y, backBtn.width, backBtn.height, 5, COLOR_SURFACE);
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(backBtn.x + 25, backBtn.y + 10);
    tft.print("←");
    
    // Playlist name (truncated)
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(100, backBtn.y + 10);
    String displayName = currentPlaylist.name;
    if (displayName.length() > 15) {
        displayName = displayName.substring(0, 12) + "...";
    }
    tft.print(displayName);
    
    // Shuffle button
    tft.fillRoundRect(shuffleBtn.x, shuffleBtn.y, shuffleBtn.width, shuffleBtn.height, 5, COLOR_SURFACE);
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(shuffleBtn.x + 20, shuffleBtn.y + 10);
    tft.print("🔀");
    
    // Play All button
    tft.fillRoundRect(playAllBtn.x, playAllBtn.y, playAllBtn.width, playAllBtn.height, 5, COLOR_SPOTIFY_GREEN);
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(playAllBtn.x + 15, playAllBtn.y + 10);
    tft.print("▶️");
}

void TrackListScreen::drawTracks() {
    int startY = 60;
    int visibleCount = min(TRACKS_PER_PAGE, (int)tracks.size() - scrollOffset);
    
    for (int i = 0; i < visibleCount; i++) {
        int index = scrollOffset + i;
        const Track& track = tracks[index];
        int y = startY + i * (ITEM_HEIGHT + 5);
        
        bool selected = (index == selectedTrack);
        drawTrackItem(track, index, 10, y, selected);
    }
    
    updateTouchAreas();
}

void TrackListScreen::drawTrackItem(const Track& track, int index, int x, int y, bool selected) {
    // Background
    uint16_t bgColor = selected ? COLOR_SPOTIFY_GREEN : COLOR_BG;
    tft.fillRoundRect(x, y, DISPLAY_WIDTH - 20, ITEM_HEIGHT, 5, bgColor);
    
    // Track number
    drawTrackNumber(index + 1, x + 10, y + 20);
    
    // Track title
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(x + 50, y + 10);
    String title = track.title;
    if (title.length() > 25) {
        title = title.substring(0, 22) + "...";
    }
    tft.print(title);
    
    // Artist name
    tft.setTextColor(COLOR_TEXT_SECONDARY);
    tft.setTextSize(1);
    tft.setCursor(x + 50, y + 35);
    String artist = track.artist;
    if (artist.length() > 30) {
        artist = artist.substring(0, 27) + "...";
    }
    tft.print(artist);
}

void TrackListScreen::drawTrackNumber(int number, int x, int y) {
    tft.setTextColor(COLOR_TEXT_SECONDARY);
    tft.setTextSize(2);
    tft.setCursor(x, y);
    tft.print(number);
    tft.print(".");
}

void TrackListScreen::drawLoading() {
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(100, DISPLAY_HEIGHT / 2 - 10);
    tft.print("Laden...");
}

void TrackListScreen::drawError() {
    tft.setTextColor(COLOR_ERROR);
    tft.setTextSize(2);
    tft.setCursor(50, DISPLAY_HEIGHT / 2 - 10);
    tft.print(errorMessage);
    
    tft.setTextColor(COLOR_TEXT_SECONDARY);
    tft.setTextSize(1);
    tft.setCursor(80, DISPLAY_HEIGHT / 2 + 20);
    tft.print("Tippe zum Neuladen");
}

void TrackListScreen::drawEmptyState() {
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(100, DISPLAY_HEIGHT / 2 - 20);
    tft.print("Keine Tracks");
}

bool TrackListScreen::handleTouch(int16_t x, int16_t y) {
    // Check back button
    if (x >= backBtn.x && x <= backBtn.x + backBtn.width &&
        y >= backBtn.y && y <= backBtn.y + backBtn.height) {
        menuManager->goBack();
        return true;
    }
    
    // Check shuffle button
    if (x >= shuffleBtn.x && x <= shuffleBtn.x + shuffleBtn.width &&
        y >= shuffleBtn.y && y <= shuffleBtn.y + shuffleBtn.height) {
        shufflePlay();
        return true;
    }
    
    // Check play all button
    if (x >= playAllBtn.x && x <= playAllBtn.x + playAllBtn.width &&
        y >= playAllBtn.y && y <= playAllBtn.y + playAllBtn.height) {
        playAll();
        return true;
    }
    
    // Check track items
    for (int i = 0; i < TRACKS_PER_PAGE; i++) {
        int index = scrollOffset + i;
        if (index >= tracks.size()) break;
        
        if (x >= trackAreas[i].x && x <= trackAreas[i].x + trackAreas[i].width &&
            y >= trackAreas[i].y && y <= trackAreas[i].y + trackAreas[i].height) {
            
            selectedTrack = index;
            draw();
            
            // Play track
            playTrack(index);
            return true;
        }
    }
    
    return false;
}

void TrackListScreen::playTrack(int index) {
    if (index < 0 || index >= tracks.size()) return;
    
    const Track& track = tracks[index];
    Serial.printf("🎵 Spiele Track: %s - %s\n", track.title.c_str(), track.artist.c_str());
    
    // Play via Spotify API
    PlaybackController::playTrack(track.uri, currentPlaylist.uri, index);
    
    // Navigate to Now Playing
    menuManager->navigateTo(SCREEN_NOW_PLAYING);
}

void TrackListScreen::playAll() {
    Serial.println("🎵 Spiele alle Tracks");
    
    if (tracks.empty()) return;
    
    // Play playlist via Spotify API
    PlaybackController::playPlaylist(currentPlaylist.uri, 0);
    
    // Navigate to Now Playing
    menuManager->navigateTo(SCREEN_NOW_PLAYING);
}

void TrackListScreen::shufflePlay() {
    Serial.println("🎵 Shuffle Play");
    
    if (tracks.empty()) return;
    
    // Shuffle and play via Spotify API
    PlaybackController::playPlaylist(currentPlaylist.uri, 0, true);
    
    // Navigate to Now Playing
    menuManager->navigateTo(SCREEN_NOW_PLAYING);
}

void TrackListScreen::updateTouchAreas() {
    int startY = 60;
    int visibleCount = min(TRACKS_PER_PAGE, (int)tracks.size() - scrollOffset);
    
    for (int i = 0; i < visibleCount; i++) {
        trackAreas[i] = {
            10,
            startY + i * (ITEM_HEIGHT + 5),
            DISPLAY_WIDTH - 20,
            ITEM_HEIGHT
        };
    }
}
```

---

## Search Screen with On-Screen Keyboard

### Layout (Search Entry)

```
┌─────────────────────────────┐
│ ← Search                ×   │  Header
├─────────────────────────────┤
│                             │
│  ┌───────────────────────┐  │
│  │ Type to search...     │  │  Search bar
│  └───────────────────────┘  │
│                             │
│  Recent Searches            │
│  • rock classics            │
│  • discover weekly          │
│                             │
│  Top Categories             │
│  [Pop]  [Rock]  [Hip-Hop]   │
│  [Jazz] [Electronic] [R&B]  │
│                             │
└─────────────────────────────┘
```

### Layout (Search Results)

```
┌─────────────────────────────┐
│ ← "rock"              ×   │  Header
├─────────────────────────────┤
│  Tracks                    │
│  ┌───────────────────────┐  │
│  │ 1. Bohemian Rhapsody   │  │  Track result
│  │    Queen              │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ 2. Stairway to Heaven  │  │  Track result
│  │    Led Zeppelin       │  │
│  └───────────────────────┘  │
│                             │
│  Artists                   │
│  ┌───────────────────────┐  │
│  │ [IMG] Queen          │  │  Artist result
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │
│  │ [IMG] Led Zeppelin   │  │  Artist result
│  └───────────────────────┘  │
│                             │
│  Albums                    │
│  ┌───────────────────────┐  │
│  │ [IMG] A Night at...  │  │  Album result
│  └───────────────────────┘  │
│                             │
└─────────────────────────────┘
```

### Layout (Keyboard Active)

```
┌─────────────────────────────┐
│ ← "rock"              ×   │  Header
├─────────────────────────────┤
│  ┌───────────────────────┐  │  Search bar (editing)
│  │ rock|                 │  │  (cursor shown)
│  └───────────────────────┘  │
│                             │
│  ┌───────────────────────┐  │  Keyboard row 1
│  │  Q  W  E  R  T  Y  U  │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │  Keyboard row 2
│  │  A  S  D  F  G  H  J  │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │  Keyboard row 3
│  │  Z  X  C  V  B  N  M  │  │
│  └───────────────────────┘  │
│  ┌───────────────────────┐  │  Keyboard row 4
│  │  ⌫  Space  . ⏎     │  │
│  └───────────────────────┘  │
│                             │
└─────────────────────────────┘
```

### Search Screen Implementation

```cpp
#ifndef SEARCH_SCREEN_H
#define SEARCH_SCREEN_H

#include <Arduino.h>
#include "spotify_gui.h"
#include "models.h"
#include "OnScreenKeyboard.h"

enum SearchTab {
    SEARCH_ALL,
    SEARCH_TRACKS,
    SEARCH_ARTISTS,
    SEARCH_ALBUMS
};

class SearchScreen {
private:
    String searchQuery;
    bool showingKeyboard;
    SearchTab currentTab;
    bool isLoading;
    
    // Search results
    std::vector<Track> trackResults;
    std::vector<Artist> artistResults;
    std::vector<Album> albumResults;
    
    // Recent searches
    std::vector<String> recentSearches;
    
    // Top categories
    std::vector<String> topCategories;
    
    // Touch areas
    struct {
        int x, y, width, height;
    } backBtn, closeBtn, searchBar;
    
    // Keyboard
    OnScreenKeyboard keyboard;
    
public:
    SearchScreen(TFT_eSPI& tft, DisplayManager& display);
    
    void init();
    void show();
    void hide();
    void update();
    
    void draw();
    void drawHeader();
    void drawSearchBar();
    void drawRecentSearches();
    void drawTopCategories();
    void drawResults();
    void drawKeyboard();
    
    bool handleTouch(int16_t x, int16_t y);
    
    // Search
    void performSearch(const String& query);
    void searchTracks(const String& query);
    void searchArtists(const String& query);
    void searchAlbums(const String& query);
    
    // Keyboard
    void showKeyboard();
    void hideKeyboard();
    void handleKeyInput(char key);
    
    // Results
    void playTrack(const Track& track);
    void viewArtist(const Artist& artist);
    void viewAlbum(const Album& album);
    
private:
    void loadRecentSearches();
    void saveRecentSearches();
    void loadTopCategories();
    void updateTouchAreas();
};

#endif // SEARCH_SCREEN_H
```

### On-Screen Keyboard Component

```cpp
#ifndef ON_SCREEN_KEYBOARD_H
#define ON_SCREEN_KEYBOARD_H

#include <Arduino.h>
#include "spotify_gui.h"

class OnScreenKeyboard {
private:
    TFT_eSPI& tft;
    
    // Keyboard layout (rows of keys)
    static const int ROWS = 4;
    static const int COLS = 10;
    
    // Key layout
    const char* layout[ROWS] = {
        "QWERTYUIOP",
        "ASDFGHJKL;",
        "ZXCVBNM,./",
        "⌫   SPACE.⏎"
    };
    
    // Touch areas for keys
    struct KeyArea {
        int x, y, width, height;
        char key;
    } keyAreas[ROWS][COLS];
    
    bool isVisible;
    int selectedRow;
    int selectedCol;
    
public:
    OnScreenKeyboard(TFT_eSPI& tft);
    
    void init();
    void show(int y);
    void hide();
    
    void draw();
    void drawKey(int row, int col, bool selected);
    
    char getKeyAt(int x, int y);
    bool handleTouch(int16_t x, int16_t y);
    
    // Callbacks
    void (*onKeyInput)(char key) = nullptr;
    void (*onBackspace)() = nullptr;
    void (*onEnter)() = nullptr;
    void (*onSpace)() = nullptr;
    
private:
    void updateKeyAreas(int startY);
    int getKeyWidth(char key);
    int getKeyHeight();
};

#endif // ON_SCREEN_KEYBOARD_H
```

### Keyboard Implementation

```cpp
#include "OnScreenKeyboard.h"

OnScreenKeyboard::OnScreenKeyboard(TFT_eSPI& tft) 
    : tft(tft), isVisible(false), selectedRow(-1), selectedCol(-1) {
}

void OnScreenKeyboard::init() {
    Serial.println("⌨️ On-Screen-Keyboard initialisiert");
}

void OnScreenKeyboard::show(int y) {
    isVisible = true;
    updateKeyAreas(y);
    draw();
}

void OnScreenKeyboard::hide() {
    isVisible = false;
    
    // Clear keyboard area
    tft.fillRect(0, keyAreas[0][0].y, DISPLAY_WIDTH, 
                 keyAreas[ROWS-1][COLS-1].y + getKeyHeight() - keyAreas[0][0].y, 
                 COLOR_BG);
}

void OnScreenKeyboard::updateKeyAreas(int startY) {
    int keyHeight = getKeyHeight();
    int keyWidth = 35;
    int gap = 5;
    int startX = 10;
    
    for (int row = 0; row < ROWS; row++) {
        int currentX = startX;
        
        for (int col = 0; col < COLS; col++) {
            char key = layout[row][col];
            
            // Special keys have different widths
            int width = getKeyWidth(key);
            
            keyAreas[row][col] = {
                currentX,
                startY + row * (keyHeight + gap),
                width,
                keyHeight,
                key
            };
            
            currentX += width + gap;
        }
    }
}

int OnScreenKeyboard::getKeyWidth(char key) {
    if (key == '⌫') return 80;
    if (key == ' ') return 200;
    if (key == '.') return 50;
    if (key == '⏎') return 80;
    return 35;
}

int OnScreenKeyboard::getKeyHeight() {
    return 50;
}

void OnScreenKeyboard::draw() {
    if (!isVisible) return;
    
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            bool selected = (row == selectedRow && col == selectedCol);
            drawKey(row, col, selected);
        }
    }
}

void OnScreenKeyboard::drawKey(int row, int col, bool selected) {
    KeyArea& key = keyAreas[row][col];
    
    // Background
    uint16_t bgColor = selected ? COLOR_SPOTIFY_GREEN : COLOR_SURFACE;
    tft.fillRoundRect(key.x, key.y, key.width, key.height, 5, bgColor);
    
    // Key text
    tft.setTextColor(COLOR_TEXT_PRIMARY);
    tft.setTextSize(2);
    
    // Center text in key
    int textX = key.x + (key.width - 20) / 2;
    int textY = key.y + (key.height - 20) / 2;
    tft.setCursor(textX, textY);
    
    if (key.key == '⌫') {
        tft.print("⌫");
    } else if (key.key == ' ') {
        tft.print("SPACE");
    } else if (key.key == '⏎') {
        tft.print("⏎");
    } else {
        tft.print(key.key);
    }
}

char OnScreenKeyboard::getKeyAt(int x, int y) {
    if (!isVisible) return '\0';
    
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            KeyArea& key = keyAreas[row][col];
            
            if (x >= key.x && x <= key.x + key.width &&
                y >= key.y && y <= key.y + key.height) {
                return key.key;
            }
        }
    }
    
    return '\0';
}

bool OnScreenKeyboard::handleTouch(int16_t x, int16_t y) {
    char key = getKeyAt(x, y);
    
    if (key == '\0') {
        // Deselect
        if (selectedRow != -1) {
            int oldRow = selectedRow;
            int oldCol = selectedCol;
            selectedRow = -1;
            selectedCol = -1;
            drawKey(oldRow, oldCol, false);
        }
        return false;
    }
    
    // Handle key press
    if (key == '⌫') {
        if (onBackspace) onBackspace();
    } else if (key == ' ') {
        if (onSpace) onSpace();
    } else if (key == '⏎') {
        if (onEnter) onEnter();
    } else {
        if (onKeyInput) onKeyInput(key);
    }
    
    return true;
}
```

---

## Swipe Gestures

### Gesture Detection

```cpp
#ifndef SWIPE_GESTURES_H
#define SWIPE_GESTURES_H

#include <Arduino.h>
#include "touch_manager.h"

enum SwipeDirection {
    SWIPE_NONE,
    SWIPE_UP,
    SWIPE_DOWN,
    SWIPE_LEFT,
    SWIPE_RIGHT
};

struct SwipeGesture {
    SwipeDirection direction;
    int startX;
    int startY;
    int endX;
    int endY;
    int distance;
    unsigned long duration;
};

class SwipeGestures {
private:
    static const int MIN_SWIPE_DISTANCE = 50;
    static const int MAX_SWIPE_TIME = 500;
    
    int startX;
    int startY;
    bool isDragging;
    unsigned long dragStartTime;
    
    void (*onSwipe)(SwipeDirection direction) = nullptr;
    
public:
    SwipeGestures();
    
    void onTouchStart(int x, int y);
    void onTouchMove(int x, int y);
    void onTouchEnd(int x, int y);
    
    bool detectSwipe(int startX, int startY, int endX, int endY, 
                     unsigned long duration);
    
    void setSwipeCallback(void (*callback)(SwipeDirection));
    
private:
    SwipeDirection getSwipeDirection(int dx, int dy);
    int getDistance(int x1, int y1, int x2, int y2);
};

#endif // SWIPE_GESTURES_H
```

### Swipe Gestures Implementation

```cpp
#include "SwipeGestures.h"

SwipeGestures::SwipeGestures() 
    : startX(0), startY(0), isDragging(false), dragStartTime(0) {
}

void SwipeGestures::onTouchStart(int x, int y) {
    startX = x;
    startY = y;
    isDragging = true;
    dragStartTime = millis();
}

void SwipeGestures::onTouchMove(int x, int y) {
    // Could implement live drag feedback here
}

void SwipeGestures::onTouchEnd(int x, int y) {
    if (!isDragging) return;
    
    isDragging = false;
    unsigned long duration = millis() - dragStartTime;
    
    if (detectSwipe(startX, startY, x, y, duration)) {
        SwipeDirection direction = getSwipeDirection(x - startX, y - startY);
        if (onSwipe) {
            onSwipe(direction);
        }
    }
}

bool SwipeGestures::detectSwipe(int startX, int startY, int endX, int endY, 
                                 unsigned long duration) {
    if (duration > MAX_SWIPE_TIME) return false;
    
    int distance = getDistance(startX, startY, endX, endY);
    return distance >= MIN_SWIPE_DISTANCE;
}

SwipeDirection SwipeGestures::getSwipeDirection(int dx, int dy) {
    int absDx = abs(dx);
    int absDy = abs(dy);
    
    if (absDx > absDy) {
        return (dx > 0) ? SWIPE_RIGHT : SWIPE_LEFT;
    } else {
        return (dy > 0) ? SWIPE_DOWN : SWIPE_UP;
    }
}

int SwipeGestures::getDistance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

void SwipeGestures::setSwipeCallback(void (*callback)(SwipeDirection)) {
    onSwipe = callback;
}
```

---

## Playlist Manager (API Layer)

```cpp
#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "models.h"
#include "SpotifyClient.h"

class PlaylistManager {
public:
    // Get user playlists
    static bool getUserPlaylists(PlaylistList& result, 
                                 int limit = 20, int offset = 0);
    
    // Get playlist tracks
    static bool getPlaylistTracks(const String& playlistId, 
                                  std::vector<Track>& result);
    
    // Search
    static bool searchTracks(const String& query, 
                            std::vector<Track>& result, int limit = 20);
    static bool searchArtists(const String& query, 
                             std::vector<Artist>& result, int limit = 20);
    static bool searchAlbums(const String& query, 
                            std::vector<Album>& result, int limit = 20);
    
    // Playlist operations
    static bool createPlaylist(const String& name, const String& description);
    static bool addTrackToPlaylist(const String& playlistId, const String& trackUri);
    static bool removeTrackFromPlaylist(const String& playlistId, 
                                       const String& trackUri, int position);
    
private:
    // API endpoints
    static const char* GET_USER_PLAYLISTS_ENDPOINT;
    static const char* GET_PLAYLIST_TRACKS_ENDPOINT;
    static const char* SEARCH_ENDPOINT;
};

#endif // PLAYLIST_MANAGER_H
```

---

## Implementation Tasks

### Day 1: Playlist Browser
- [ ] Create PlaylistBrowserScreen.h/cpp
- [ ] Implement playlist list UI
- [ ] Add playlist thumbnail placeholder
- [ ] Implement playlist item touch handling
- [ ] Create PlaylistManager class
- [ ] Implement getUserPlaylists API call
- [ ] Add loading and error states
- [ ] Test navigation from Now Playing

### Day 2: Track List
- [ ] Create TrackListScreen.h/cpp
- [ ] Implement track list UI
- [ ] Add play/shuffle controls
- [ ] Implement track item touch handling
- [ ] Implement playTrack functionality
- [ ] Implement playPlaylist functionality
- [ ] Add loading and error states
- [ ] Test playback from track list

### Day 3: Search Screen
- [ ] Create SearchScreen.h/cpp
- [ ] Create OnScreenKeyboard component
- [ ] Implement search bar UI
- [ ] Add recent searches display
- [ ] Add top categories
- [ ] Implement search results UI
- [ ] Implement keyboard touch handling
- [ ] Implement search API calls

### Day 4: Polish & Integration
- [ ] Add swipe gestures
- [ ] Implement image caching for thumbnails
- [ ] Add smooth transitions
- [ ] Optimize performance
- [ ] Add error handling
- [ ] Test all navigation flows
- [ ] Document API integration

---

## Testing Checklist

### Playlist Browser
- [ ] Playlists load from Spotify API
- [ ] Playlist list displays correctly
- [ ] Playlist items are touchable
- [ ] Navigation to Track List works
- [ ] Back button returns to Playlist Browser
- [ ] Loading state displays
- [ ] Error state displays
- [ ] Scroll works if playlists exceed screen

### Track List
- [ ] Tracks load for selected playlist
- [ ] Track list displays correctly
- [ ] Track items are touchable
- [ ] Play button plays track
- [ ] Play All works
- [ ] Shuffle Play works
- [ ] Navigation to Now Playing works
- [ ] Back button returns to Playlist Browser

### Search Screen
- [ ] Search bar accepts input
- [ ] Keyboard displays and hides
- [ ] Keyboard keys work
- [ ] Backspace deletes characters
- [ ] Enter performs search
- [ ] Recent searches display
- [ ] Top categories display
- [ ] Search results display
- [ ] Track results are playable
- [ ] Artist/Album results work

### Swipe Gestures
- [ ] Swipe up scrolls down
- [ ] Swipe down scrolls up
- [ ] Swipe left navigates back
- [ ] Swipe right navigates forward

### Performance
- [ ] Playlist loading completes in < 5 seconds
- [ ] Track loading completes in < 3 seconds
- [ ] Search completes in < 2 seconds
- [ ] UI is responsive during loading
- [ ] Scrolling is smooth

---

## Known Challenges & Solutions

### Challenge 1: Large Playlist Loading
**Problem:** Loading 1000+ tracks at once blocks UI
**Solution:** Load in pages (20-50 tracks), show progress, load more on scroll

### Challenge 2: Image Download Performance
**Problem:** Downloading many thumbnails is slow
**Solution:** 
- Cache images in LittleFS
- Load images asynchronously
- Show placeholders first, update when loaded

### Challenge 3: Keyboard Size on Small Display
**Problem:** Keyboard may not fit on small displays
**Solution:** 
- Make keyboard scrollable
- Support both portrait and landscape
- Compact layout option

### Challenge 4: Search API Rate Limits
**Problem:** Too many search requests may hit rate limits
**Solution:** 
- Debounce search input
- Cache search results
- Implement exponential backoff

### Challenge 5: Swipe Gesture Conflicts
**Problem:** Swipes may conflict with button taps
**Solution:** 
- Define clear touch zones
- Use time threshold (swipe = fast, tap = slow)
- Test extensively

---

## MVP vs Full Features

### MVP (Must Have)
- ✅ Playlist Browser (view playlists)
- ✅ Track List (view tracks in playlist)
- ✅ Play track from list
- ✅ Basic search (tracks only)
- ✅ On-screen keyboard
- ✅ Navigation between screens

### Full Features (Nice to Have)
- ⏳ Search results for artists/albums
- ⏳ Recent searches
- ⏳ Top categories
- ⏳ Swipe gestures
- ⏳ Image thumbnails
- ⏳ Playlist creation
- ⏳ Add/remove tracks

---

## Next Steps (Phase 7)

After Phase 6 completion:
1. Implement Screensaver
2. Add Device Selection
3. Performance Optimization
4. Testing & Bug Fixes
5. Documentation

---

**Generated:** 2026-02-12
**Author:** Subagent (Phase 6 Planning)
**Status:** Ready for Implementation
