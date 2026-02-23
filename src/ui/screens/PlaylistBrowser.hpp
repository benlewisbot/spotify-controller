/**
 * @file PlaylistBrowser.hpp
 * @brief Playlist Browser Screen
 *
 * Scrollable list of user's Spotify playlists.
 * Tap a playlist to start playback and return to Now Playing.
 */

#ifndef PLAYLIST_BROWSER_HPP
#define PLAYLIST_BROWSER_HPP

#include <lvgl.h>
#include <vector>
#include "../../spotify/SpotifyClient.hpp"
#include "../../app/App.hpp"
#include "MenuManager.hpp"

namespace ui {

/**
 * @brief Playlist Browser Screen Class
 *
 * Displays a scrollable list of the user's Spotify playlists.
 * Each item shows name, owner, and track count.
 * Tapping an item plays the playlist and navigates to Now Playing.
 */
class PlaylistBrowserScreen {
public:
    PlaylistBrowserScreen(lv_obj_t* parent);
    ~PlaylistBrowserScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

    /**
     * @brief Load playlists from Spotify API and populate the list
     */
    void loadPlaylists();

    /**
     * @brief Clear and reload playlists
     */
    void refreshPlaylists();

private:
    void createUI();
    void createHeader();
    void createListContainer();
    void createLoadingSpinner();
    void showLoading();
    void hideLoading();
    void showError(const String& message);
    void clearList();
    void populateList(const std::vector<SpotifyClient::PlaylistInfo>& playlists);
    void createPlaylistItem(const SpotifyClient::PlaylistInfo& playlist, int index);

    // LVGL objects
    lv_obj_t* screen;
    lv_obj_t* listContainer;
    lv_obj_t* loadingSpinner;
    lv_obj_t* loadingLabel;
    lv_obj_t* errorLabel;

    // Cached playlist data for event callbacks
    std::vector<SpotifyClient::PlaylistInfo> cachedPlaylists;
};

} // namespace ui

#endif // PLAYLIST_BROWSER_HPP
