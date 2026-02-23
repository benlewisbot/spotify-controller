/**
 * @file NowPlaying.hpp
 * @brief Now Playing Screen - 480x480 Square Layout
 *
 * Shows current track info with album art (JPEG), title, artist,
 * progress bar, playback controls, and volume slider.
 * Optimized for 480x480 square display.
 */

#ifndef NOW_PLAYING_HPP
#define NOW_PLAYING_HPP

#include <lvgl.h>
#include "../../spotify/SpotifyClient.hpp"

namespace ui {

/**
 * @brief Now Playing Screen Class
 *
 * Main screen showing currently playing track.
 * Layout: art top, info middle, controls bottom.
 */
class NowPlayingScreen {
public:
    NowPlayingScreen(lv_obj_t* parent);
    ~NowPlayingScreen();

    lv_obj_t* getScreen() const { return screen; }

    void updateTrackInfo(const SpotifyClient::TrackInfo& track);
    void updateProgress(int progressMs, int durationMs);
    void updatePlaybackState(bool isPlaying);
    void updateVolume(int volumePercent);
    void updateSaveState(bool saved);
    void updateShuffleState(bool shuffle);
    void updateRepeatState(int repeatMode); // 0=off, 1=context, 2=track
    void update();

    // Navigation callback - set by WindowManager
    std::function<void()> onMenuPressed;

private:
    void createUI();
    void createTopBar();
    void createAlbumArt();
    void createTrackInfo();
    void createProgressBar();
    void createControls();
    void createVolumeControl();

    void loadAlbumArt(const String& imageUrl);
    void downloadAndDisplayArt(const String& url);

    // Format milliseconds as M:SS
    static void formatTime(int ms, char* buf, size_t bufSize);

    // LVGL objects
    lv_obj_t* screen;

    // Top bar
    lv_obj_t* menuBtn;
    lv_obj_t* saveBtn;
    lv_obj_t* saveBtnLabel;
    lv_obj_t* settingsBtn;

    // Album art
    lv_obj_t* albumArtContainer;
    lv_obj_t* albumArtImg;
    lv_img_dsc_t albumArtDsc;
    uint8_t* albumArtBuffer;  // PSRAM buffer for decoded JPEG
    size_t albumArtBufferSize;

    // Track info
    lv_obj_t* trackTitleLabel;
    lv_obj_t* artistLabel;

    // Progress
    lv_obj_t* progressBar;
    lv_obj_t* progressLabelLeft;
    lv_obj_t* progressLabelRight;

    // Controls
    lv_obj_t* shuffleBtn;
    lv_obj_t* shuffleBtnLabel;
    lv_obj_t* prevBtn;
    lv_obj_t* playPauseBtn;
    lv_obj_t* playPauseBtnLabel;
    lv_obj_t* nextBtn;
    lv_obj_t* repeatBtn;
    lv_obj_t* repeatBtnLabel;

    // Volume
    lv_obj_t* volumeLabel;
    lv_obj_t* volumeSlider;
    lv_obj_t* volumePlusLabel;

    // UI state
    SpotifyClient::TrackInfo currentTrack;
    String currentArtUrl;
    bool isPlaying;
    int currentVolume;
    bool isSaved;
    bool isShuffled;
    int repeatMode;
    unsigned long lastVolumeChangeMs;
    unsigned long lastProgressUpdateMs;
    int interpolatedProgressMs;
};

} // namespace ui

#endif // NOW_PLAYING_HPP
