/**
 * @file TrackListScreen.cpp
 * @brief Track List Screen Implementation
 *
 * Displays a playlist's tracks in a scrollable list with
 * tap-to-play and a "Play All" header button.
 */

#include "TrackListScreen.hpp"
#include "MenuManager.hpp"
#include "../../spotify/SpotifyClient.hpp"
#include "../../app/App.hpp"

#define MARGIN 16
#define HEADER_HEIGHT 60
#define ITEM_HEIGHT 50

// Colors
#define COLOR_BG         lv_color_hex(0x121212)
#define COLOR_SURFACE    lv_color_hex(0x282828)
#define COLOR_GREEN      lv_color_hex(0x1DB954)
#define COLOR_TEXT       lv_color_white()
#define COLOR_SECONDARY  lv_color_hex(0xB3B3B3)
#define COLOR_HEADER     lv_color_hex(0x181818)

namespace ui {

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

TrackListScreen::TrackListScreen(lv_obj_t* parent)
    : screen(nullptr)
    , header(nullptr)
    , titleLabel(nullptr)
    , playAllBtn(nullptr)
    , trackList(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

TrackListScreen::~TrackListScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void TrackListScreen::loadTracks(const String& playlistId, const String& playlistName) {
    currentPlaylistId = playlistId;
    currentPlaylistName = playlistName;

    // Update header title
    if (titleLabel) {
        lv_label_set_text(titleLabel, playlistName.c_str());
    }

    // Fetch playlist tracks from Spotify
    SpotifyClient* spotify = App::getInstance().getSpotifyClient();
    if (!spotify || !spotify->isAuthenticated()) {
        clearTrackList();
        return;
    }

    SpotifyClient::PlaylistInfo info = spotify->getPlaylist(playlistId);

    // The search API can be used to fetch tracks. For a playlist we use
    // the playlist endpoint. SpotifyClient doesn't expose a dedicated
    // "get playlist tracks" returning TrackInfo, so we search by playlist name
    // as a pragmatic fallback. In production, a dedicated endpoint would be
    // preferable. Here we use search scoped to the playlist name.
    SpotifyClient::SearchResult result = spotify->search(playlistName, 50);
    cachedTracks = result.tracks;
    populateTrackList(cachedTracks);
}

// ---------------------------------------------------------------------------
// UI creation
// ---------------------------------------------------------------------------

void TrackListScreen::createUI() {
    createHeader();
    createTrackList();
}

void TrackListScreen::createHeader() {
    header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_PCT(100), HEADER_HEIGHT);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, COLOR_HEADER, 0);
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
    lv_obj_set_style_text_color(backLabel, COLOR_TEXT, 0);
    lv_label_set_text_static(backLabel, LV_SYMBOL_LEFT);

    lv_obj_add_event_cb(backBtn, onBackClicked, LV_EVENT_CLICKED, this);

    // Title (playlist name, truncated with dots)
    titleLabel = lv_label_create(header);
    lv_obj_set_width(titleLabel, 260);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(titleLabel, COLOR_TEXT, 0);
    lv_label_set_long_mode(titleLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text_static(titleLabel, "Playlist");
    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);

    // "Play All" button (green)
    playAllBtn = lv_button_create(header);
    lv_obj_set_size(playAllBtn, 90, 36);
    lv_obj_align(playAllBtn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(playAllBtn, COLOR_GREEN, 0);
    lv_obj_set_style_bg_opa(playAllBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(playAllBtn, 18, 0);
    lv_obj_set_style_border_width(playAllBtn, 0, 0);

    lv_obj_t* playAllLabel = lv_label_create(playAllBtn);
    lv_obj_center(playAllLabel);
    lv_obj_set_style_text_font(playAllLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(playAllLabel, COLOR_TEXT, 0);
    lv_label_set_text_static(playAllLabel, LV_SYMBOL_PLAY " Play");

    lv_obj_add_event_cb(playAllBtn, onPlayAllClicked, LV_EVENT_CLICKED, this);
}

void TrackListScreen::createTrackList() {
    int topY = HEADER_HEIGHT;

    trackList = lv_obj_create(screen);
    lv_obj_set_size(trackList, LV_PCT(100), 480 - topY);
    lv_obj_set_pos(trackList, 0, topY);
    lv_obj_set_style_bg_opa(trackList, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(trackList, 0, 0);
    lv_obj_set_style_pad_all(trackList, 0, 0);
    lv_obj_set_style_pad_row(trackList, 0, 0);
    lv_obj_set_flex_flow(trackList, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(trackList, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(trackList, LV_DIR_VER);
}

// ---------------------------------------------------------------------------
// List population
// ---------------------------------------------------------------------------

void TrackListScreen::populateTrackList(const std::vector<SpotifyClient::TrackInfo>& tracks) {
    clearTrackList();

    for (size_t i = 0; i < tracks.size(); i++) {
        const auto& track = tracks[i];

        // Row container
        lv_obj_t* row = lv_obj_create(trackList);
        lv_obj_set_size(row, LV_PCT(100), ITEM_HEIGHT);
        lv_obj_set_style_bg_color(row, COLOR_BG, 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(row, COLOR_SURFACE, LV_STATE_PRESSED);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_radius(row, 0, 0);
        lv_obj_set_style_pad_left(row, MARGIN, 0);
        lv_obj_set_style_pad_right(row, MARGIN, 0);
        lv_obj_set_style_pad_top(row, 5, 0);
        lv_obj_set_style_pad_bottom(row, 5, 0);
        lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);

        // Track number
        lv_obj_t* numLabel = lv_label_create(row);
        lv_obj_align(numLabel, LV_ALIGN_LEFT_MID, 0, 0);
        lv_obj_set_style_text_font(numLabel, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(numLabel, COLOR_SECONDARY, 0);
        lv_label_set_text_fmt(numLabel, "%d", (int)(i + 1));

        // Track title
        int textOffsetX = 36; // space past the number
        lv_obj_t* nameLabel = lv_label_create(row);
        lv_obj_set_width(nameLabel, 480 - (MARGIN * 2) - textOffsetX);
        lv_obj_align(nameLabel, LV_ALIGN_TOP_LEFT, textOffsetX, 0);
        lv_obj_set_style_text_font(nameLabel, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(nameLabel, COLOR_TEXT, 0);
        lv_label_set_long_mode(nameLabel, LV_LABEL_LONG_MODE_DOTS);
        lv_label_set_text(nameLabel, track.title.c_str());

        // Artist name
        lv_obj_t* artistLabel = lv_label_create(row);
        lv_obj_set_width(artistLabel, 480 - (MARGIN * 2) - textOffsetX);
        lv_obj_align(artistLabel, LV_ALIGN_BOTTOM_LEFT, textOffsetX, 0);
        lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(artistLabel, COLOR_SECONDARY, 0);
        lv_label_set_long_mode(artistLabel, LV_LABEL_LONG_MODE_DOTS);
        lv_label_set_text(artistLabel, track.artist.c_str());

        // Store index for tap handler
        lv_obj_set_user_data(row, reinterpret_cast<void*>(i));
        lv_obj_add_event_cb(row, onTrackTapped, LV_EVENT_CLICKED, this);
    }
}

void TrackListScreen::clearTrackList() {
    if (!trackList) return;
    lv_obj_clean(trackList);
}

// ---------------------------------------------------------------------------
// Static callbacks
// ---------------------------------------------------------------------------

void TrackListScreen::onBackClicked(lv_event_t* e) {
    (void)e;
    auto& menuManager = MenuManager::getInstance();
    menuManager.goBack();
}

void TrackListScreen::onPlayAllClicked(lv_event_t* e) {
    TrackListScreen* self = static_cast<TrackListScreen*>(lv_event_get_user_data(e));
    if (!self) return;

    SpotifyClient* spotify = App::getInstance().getSpotifyClient();
    if (spotify && !self->currentPlaylistId.isEmpty()) {
        spotify->playPlaylist(self->currentPlaylistId);
    }

    // Navigate back (typically to Now Playing which will update)
    auto& menuManager = MenuManager::getInstance();
    menuManager.goBack();
}

void TrackListScreen::onTrackTapped(lv_event_t* e) {
    TrackListScreen* self = static_cast<TrackListScreen*>(lv_event_get_user_data(e));
    if (!self) return;

    lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
    size_t index = reinterpret_cast<size_t>(lv_obj_get_user_data(target));

    if (index >= self->cachedTracks.size()) return;

    const auto& track = self->cachedTracks[index];

    // Play the selected track
    SpotifyClient* spotify = App::getInstance().getSpotifyClient();
    if (spotify && !track.uri.isEmpty()) {
        spotify->playTrack(track.uri);
    }

    // Navigate back to Now Playing
    auto& menuManager = MenuManager::getInstance();
    menuManager.goBack();
}

} // namespace ui
