/**
 * @file SearchScreen.hpp
 * @brief Search Screen for Spotify track search
 *
 * Provides text input with on-screen keyboard, debounced search,
 * and a results list for track selection. 480x480 square layout.
 */

#ifndef SEARCH_SCREEN_HPP
#define SEARCH_SCREEN_HPP

#include <lvgl.h>
#include <Arduino.h>
#include <vector>
#include "MenuManager.hpp"
#include "../../spotify/SpotifyClient.hpp"
#include "../../app/App.hpp"

namespace ui {

/**
 * @brief Search Screen Class
 *
 * Full-screen search with keyboard input, debounced API queries,
 * and selectable track results.
 */
class SearchScreen {
public:
    SearchScreen(lv_obj_t* parent);
    ~SearchScreen();

    /**
     * @brief Get LVGL screen object
     */
    lv_obj_t* getScreen() const { return screen; }

    /**
     * @brief Clear search field and results
     */
    void reset();

private:
    void createUI();
    void createHeader();
    void createSearchInput();
    void createResultsList();

    /**
     * @brief Execute the search query via SpotifyClient
     */
    void performSearch(const char* query);

    /**
     * @brief Populate the results list with search results
     */
    void populateResults(const std::vector<SpotifyClient::TrackInfo>& tracks);

    /**
     * @brief Clear all result items from the list
     */
    void clearResults();

    // Debounce timer callback (static for LVGL C API)
    static void debounceTimerCb(lv_timer_t* timer);

    // Event callbacks
    static void onTextChanged(lv_event_t* e);
    static void onResultTapped(lv_event_t* e);
    static void onBackClicked(lv_event_t* e);
    static void onTextareaFocused(lv_event_t* e);
    static void onTextareaDefocused(lv_event_t* e);

    // LVGL objects
    lv_obj_t* screen;
    lv_obj_t* header;
    lv_obj_t* textarea;
    lv_obj_t* keyboard;
    lv_obj_t* resultsList;

    // Debounce timer
    lv_timer_t* debounceTimer;

    // Cached search results for tap handling
    std::vector<SpotifyClient::TrackInfo> cachedResults;
};

} // namespace ui

#endif // SEARCH_SCREEN_HPP
