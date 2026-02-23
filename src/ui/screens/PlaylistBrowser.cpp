/**
 * @file PlaylistBrowser.cpp
 * @brief Playlist Browser Screen Implementation
 *
 * Fetches and displays user's Spotify playlists in a scrollable list.
 * Tapping a playlist starts playback and returns to Now Playing.
 */

#include "PlaylistBrowser.hpp"
#include "MenuManager.hpp"
#include "../../app/App.hpp"

#define MARGIN 16
#define HEADER_HEIGHT 60
#define ITEM_HEIGHT 60
#define ITEM_SPACING 8
#define ITEM_RADIUS 12

namespace ui {

PlaylistBrowserScreen::PlaylistBrowserScreen(lv_obj_t* parent)
    : screen(nullptr)
    , listContainer(nullptr)
    , loadingSpinner(nullptr)
    , loadingLabel(nullptr)
    , errorLabel(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

PlaylistBrowserScreen::~PlaylistBrowserScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void PlaylistBrowserScreen::createUI() {
    createHeader();
    createListContainer();
    createLoadingSpinner();
}

void PlaylistBrowserScreen::createHeader() {
    lv_obj_t* header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_PCT(100), HEADER_HEIGHT);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x181818), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, MARGIN, 0);

    // Back button
    lv_obj_t* backBtn = lv_button_create(header);
    lv_obj_set_size(backBtn, 40, 40);
    lv_obj_set_style_bg_opa(backBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(backBtn, 0, 0);
    lv_obj_set_style_radius(backBtn, 20, 0);

    lv_obj_t* backLabel = lv_label_create(backBtn);
    lv_obj_center(backLabel);
    lv_obj_set_style_text_font(backLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(backLabel, lv_color_white(), 0);
    lv_label_set_text_static(backLabel, LV_SYMBOL_LEFT);

    lv_obj_add_event_cb(backBtn, [](lv_event_t* e) {
        auto& menuManager = MenuManager::getInstance();
        menuManager.goBack();
    }, LV_EVENT_CLICKED, nullptr);

    // Title
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_label_set_text_static(titleLabel, "Playlists");
    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);
}

void PlaylistBrowserScreen::createListContainer() {
    listContainer = lv_obj_create(screen);
    lv_obj_set_size(listContainer, LV_PCT(100), 480 - HEADER_HEIGHT);
    lv_obj_set_pos(listContainer, 0, HEADER_HEIGHT);
    lv_obj_set_style_bg_opa(listContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(listContainer, 0, 0);
    lv_obj_set_style_pad_left(listContainer, MARGIN, 0);
    lv_obj_set_style_pad_right(listContainer, MARGIN, 0);
    lv_obj_set_style_pad_top(listContainer, MARGIN, 0);
    lv_obj_set_style_pad_bottom(listContainer, MARGIN, 0);
    lv_obj_set_style_pad_row(listContainer, ITEM_SPACING, 0);
    lv_obj_set_flex_flow(listContainer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(listContainer, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(listContainer, LV_DIR_VER);
}

void PlaylistBrowserScreen::createLoadingSpinner() {
    // Spinner centered on the list area
    loadingSpinner = lv_spinner_create(screen);
    lv_obj_set_size(loadingSpinner, 50, 50);
    lv_obj_align(loadingSpinner, LV_ALIGN_CENTER, 0, -10);
    lv_spinner_set_anim_params(loadingSpinner, 1000, 200);
    lv_obj_set_style_arc_color(loadingSpinner, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_arc_color(loadingSpinner, lv_color_hex(0x282828), LV_PART_MAIN);

    // Loading text below spinner
    loadingLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(loadingLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(loadingLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(loadingLabel, "Loading playlists...");
    lv_obj_align(loadingLabel, LV_ALIGN_CENTER, 0, 30);

    // Error label (hidden by default)
    errorLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(errorLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(errorLabel, lv_color_hex(0xB3B3B3), 0);
    lv_obj_set_style_text_align(errorLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(errorLabel, 480 - (MARGIN * 4));
    lv_obj_align(errorLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(errorLabel, LV_OBJ_FLAG_HIDDEN);

    // Initially hide loading indicators
    hideLoading();
}

void PlaylistBrowserScreen::showLoading() {
    lv_obj_clear_flag(loadingSpinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(errorLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(listContainer, LV_OBJ_FLAG_HIDDEN);
}

void PlaylistBrowserScreen::hideLoading() {
    lv_obj_add_flag(loadingSpinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(listContainer, LV_OBJ_FLAG_HIDDEN);
}

void PlaylistBrowserScreen::showError(const String& message) {
    lv_obj_add_flag(loadingSpinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(listContainer, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(errorLabel, message.c_str());
    lv_obj_clear_flag(errorLabel, LV_OBJ_FLAG_HIDDEN);
}

void PlaylistBrowserScreen::clearList() {
    if (listContainer) {
        lv_obj_clean(listContainer);
    }
    cachedPlaylists.clear();
}

void PlaylistBrowserScreen::loadPlaylists() {
    showLoading();

    SpotifyClient* spotify = App::getInstance().getSpotifyClient();
    if (!spotify) {
        showError("Spotify client not available");
        return;
    }

    if (!spotify->isAuthenticated()) {
        showError("Not authenticated with Spotify");
        return;
    }

    std::vector<SpotifyClient::PlaylistInfo> playlists = spotify->getPlaylists();

    if (playlists.empty()) {
        showError("No playlists found");
        return;
    }

    populateList(playlists);
}

void PlaylistBrowserScreen::refreshPlaylists() {
    clearList();
    loadPlaylists();
}

void PlaylistBrowserScreen::populateList(const std::vector<SpotifyClient::PlaylistInfo>& playlists) {
    clearList();
    cachedPlaylists = playlists;

    for (int i = 0; i < (int)cachedPlaylists.size(); i++) {
        createPlaylistItem(cachedPlaylists[i], i);
    }

    hideLoading();

    // Scroll to top
    lv_obj_scroll_to_y(listContainer, 0, LV_ANIM_OFF);
}

void PlaylistBrowserScreen::createPlaylistItem(const SpotifyClient::PlaylistInfo& playlist, int index) {
    // Item container
    lv_obj_t* item = lv_obj_create(listContainer);
    lv_obj_set_size(item, LV_PCT(100), ITEM_HEIGHT);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(item, ITEM_RADIUS, 0);
    lv_obj_set_style_border_width(item, 0, 0);
    lv_obj_set_style_pad_left(item, MARGIN, 0);
    lv_obj_set_style_pad_right(item, MARGIN, 0);
    lv_obj_set_style_pad_top(item, 8, 0);
    lv_obj_set_style_pad_bottom(item, 8, 0);
    lv_obj_set_scrollbar_mode(item, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

    // Pressed style
    lv_obj_set_style_bg_color(item, lv_color_hex(0x3E3E3E), LV_STATE_PRESSED);

    // Playlist name
    lv_obj_t* nameLabel = lv_label_create(item);
    lv_obj_set_width(nameLabel, 480 - (MARGIN * 4) - (MARGIN * 2));
    lv_obj_align(nameLabel, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_font(nameLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(nameLabel, lv_color_white(), 0);
    lv_label_set_long_mode(nameLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(nameLabel, playlist.name.c_str());

    // Owner + track count subtitle
    String subtitle = playlist.owner + " - " + String(playlist.trackCount) + " tracks";
    lv_obj_t* subtitleLabel = lv_label_create(item);
    lv_obj_set_width(subtitleLabel, 480 - (MARGIN * 4) - (MARGIN * 2));
    lv_obj_align(subtitleLabel, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_text_font(subtitleLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(subtitleLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_long_mode(subtitleLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(subtitleLabel, subtitle.c_str());

    // Arrow indicator on the right
    lv_obj_t* arrow = lv_label_create(item);
    lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_font(arrow, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(arrow, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(arrow, LV_SYMBOL_PLAY);

    // Make item clickable
    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);

    // Store index as user data for the event callback
    lv_obj_set_user_data(item, this);

    // Store the playlist index in a way the callback can retrieve it.
    // We use the child index in the flex container, which matches our playlist index.
    lv_obj_add_event_cb(item, [](lv_event_t* e) {
        lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
        PlaylistBrowserScreen* self = static_cast<PlaylistBrowserScreen*>(lv_obj_get_user_data(target));
        if (!self) return;

        // Determine which playlist was tapped by finding the child index
        lv_obj_t* parent = lv_obj_get_parent(target);
        int childCount = (int)lv_obj_get_child_count(parent);
        int tappedIndex = -1;
        for (int i = 0; i < childCount; i++) {
            if (lv_obj_get_child(parent, i) == target) {
                tappedIndex = i;
                break;
            }
        }

        if (tappedIndex < 0 || tappedIndex >= (int)self->cachedPlaylists.size()) return;

        const SpotifyClient::PlaylistInfo& selected = self->cachedPlaylists[tappedIndex];

        // Start playback of the selected playlist
        SpotifyClient* spotify = App::getInstance().getSpotifyClient();
        if (spotify && spotify->isAuthenticated()) {
            spotify->playPlaylist(selected.id);
        }

        // Navigate back to Now Playing
        auto& menuManager = MenuManager::getInstance();
        menuManager.goBack();
    }, LV_EVENT_CLICKED, nullptr);
}

} // namespace ui
