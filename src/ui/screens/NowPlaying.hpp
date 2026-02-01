/**
 * @file NowPlaying.hpp
 * @brief Now Playing Screen
 *
 * Shows current track info with album art, title, artist,
 * progress bar, and playback controls.
 */

#ifndef NOW_PLAYING_HPP
#define NOW_PLAYING_HPP

#include <lvgl.h>
#include "../spotify/SpotifyClient.hpp"

namespace ui {

/**
 * @brief Now Playing Screen Class
 *
 * Main screen showing currently playing track.
 */
class NowPlayingScreen {
public:
    NowPlayingScreen(lv_obj_t* parent);
    ~NowPlayingScreen();

    /**
     * @brief Get the LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

    /**
     * @brief Update UI with new track info
     */
    void updateTrackInfo(const SpotifyClient::TrackInfo& track);

    /**
     * @brief Update progress bar
     */
    void updateProgress(int progressMs, int durationMs);

    /**
     * @brief Update playback state
     */
    void updatePlaybackState(bool isPlaying);

    /**
     * @brief Update volume
     */
    void updateVolume(int volumePercent);

    /**
     * @brief Update screen (called periodically)
     */
    void update();

private:
    /**
     * @brief Create UI elements
     */
    void createUI();

    /**
     * @brief Create album art widget
     */
    void createAlbumArt();

    /**
     * @brief Create track info labels
     */
    void createTrackInfo();

    /**
     * @brief Create progress bar
     */
    void createProgressBar();

    /**
     * @brief Create control buttons
     */
    void createControls();

    /**
     * @brief Create volume slider
     */
    void createVolumeControl();

    /**
     * @brief Load album art image
     */
    void loadAlbumArt(const String& imageUrl);

    // LVGL objects
    lv_obj_t* screen;
    lv_obj_t* albumArt;
    lv_obj_t* trackTitleLabel;
    lv_obj_t* artistLabel;
    lv_obj_t* progressBar;
    lv_obj_t* progressLabelLeft;
    lv_obj_t* progressLabelRight;
    lv_obj_t* playPauseBtn;
    lv_obj_t* prevBtn;
    lv_obj_t* nextBtn;
    lv_obj_t* saveBtn;
    lv_obj_t* volumeSlider;
    lv_obj_t* menuBtn;

    // UI state
    SpotifyClient::TrackInfo currentTrack;
    bool isPlaying;
    int currentVolume;
};

} // namespace ui

#endif // NOW_PLAYING_HPP
