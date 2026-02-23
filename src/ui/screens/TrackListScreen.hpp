/**
 * @file TrackListScreen.hpp
 * @brief Track List Screen for displaying playlist tracks
 *
 * Shows a scrollable list of tracks for a given playlist with
 * a "Play All" button. 480x480 square layout.
 */

#ifndef TRACK_LIST_SCREEN_HPP
#define TRACK_LIST_SCREEN_HPP

#include <lvgl.h>
#include <Arduino.h>
#include <vector>
#include "MenuManager.hpp"
#include "../../spotify/SpotifyClient.hpp"
#include "../../app/App.hpp"

namespace ui {

/**
 * @brief Track List Screen Class
 *
 * Displays the tracks of a playlist with tap-to-play and
 * a "Play All" header button.
 */
class TrackListScreen {
public:
    TrackListScreen(lv_obj_t* parent);
    ~TrackListScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

    /**
     * @brief Load tracks for a playlist
     * @param playlistId Spotify playlist ID
     * @param playlistName Display name for the header title
     */
    void loadTracks(const String& playlistId, const String& playlistName);

private:
    void createUI();
    void createHeader();
    void createTrackList();

    /**
     * @brief Populate the list with fetched tracks
     */
    void populateTrackList(const std::vector<SpotifyClient::TrackInfo>& tracks);

    /**
     * @brief Clear all items from the list
     */
    void clearTrackList();

    // Event callbacks
    static void onBackClicked(lv_event_t* e);
    static void onPlayAllClicked(lv_event_t* e);
    static void onTrackTapped(lv_event_t* e);

    // LVGL objects
    lv_obj_t* screen;
    lv_obj_t* header;
    lv_obj_t* titleLabel;
    lv_obj_t* playAllBtn;
    lv_obj_t* trackList;

    // Current playlist context
    String currentPlaylistId;
    String currentPlaylistName;

    // Cached track list for tap handling
    std::vector<SpotifyClient::TrackInfo> cachedTracks;
};

} // namespace ui

#endif // TRACK_LIST_SCREEN_HPP
