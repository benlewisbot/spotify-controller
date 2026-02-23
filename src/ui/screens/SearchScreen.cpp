/**
 * @file SearchScreen.cpp
 * @brief Search Screen Implementation
 *
 * Provides debounced Spotify search with on-screen keyboard
 * and tappable track results.
 */

#include "SearchScreen.hpp"
#include "MenuManager.hpp"
#include "../../spotify/SpotifyClient.hpp"
#include "../../app/App.hpp"

#define MARGIN 16
#define HEADER_HEIGHT 60
#define TEXTAREA_HEIGHT 40
#define RESULT_ITEM_HEIGHT 56
#define KEYBOARD_HEIGHT 200
#define DEBOUNCE_MS 500

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

SearchScreen::SearchScreen(lv_obj_t* parent)
    : screen(nullptr)
    , header(nullptr)
    , textarea(nullptr)
    , keyboard(nullptr)
    , resultsList(nullptr)
    , debounceTimer(nullptr) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

SearchScreen::~SearchScreen() {
    if (debounceTimer) {
        lv_timer_delete(debounceTimer);
        debounceTimer = nullptr;
    }
    if (screen) {
        lv_obj_del(screen);
    }
}

// ---------------------------------------------------------------------------
// Public helpers
// ---------------------------------------------------------------------------

void SearchScreen::reset() {
    if (textarea) {
        lv_textarea_set_text(textarea, "");
    }
    clearResults();
    cachedResults.clear();
    if (keyboard) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

// ---------------------------------------------------------------------------
// UI creation
// ---------------------------------------------------------------------------

void SearchScreen::createUI() {
    createHeader();
    createSearchInput();
    createResultsList();
}

void SearchScreen::createHeader() {
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

    // Title
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(titleLabel, COLOR_TEXT, 0);
    lv_label_set_text_static(titleLabel, "Search");
    lv_obj_align_to(titleLabel, backBtn, LV_ALIGN_OUT_RIGHT_MID, MARGIN, 0);
}

void SearchScreen::createSearchInput() {
    // Textarea for search query
    textarea = lv_textarea_create(screen);
    lv_obj_set_size(textarea, 480 - (MARGIN * 2), TEXTAREA_HEIGHT);
    lv_obj_set_pos(textarea, MARGIN, HEADER_HEIGHT + MARGIN);
    lv_textarea_set_placeholder_text(textarea, "Search for tracks...");
    lv_textarea_set_one_line(textarea, true);
    lv_obj_set_style_bg_color(textarea, COLOR_SURFACE, 0);
    lv_obj_set_style_bg_opa(textarea, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(textarea, COLOR_GREEN, LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(textarea, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x404040), 0);
    lv_obj_set_style_border_width(textarea, 1, 0);
    lv_obj_set_style_radius(textarea, 8, 0);
    lv_obj_set_style_text_color(textarea, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(textarea, &lv_font_montserrat_14, 0);
    lv_obj_set_style_pad_left(textarea, 12, 0);
    lv_obj_set_style_pad_right(textarea, 12, 0);

    // Keyboard (hidden until textarea is focused)
    keyboard = lv_keyboard_create(screen);
    lv_obj_set_size(keyboard, 480, KEYBOARD_HEIGHT);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_textarea(keyboard, textarea);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    // Events
    lv_obj_add_event_cb(textarea, onTextChanged, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(textarea, onTextareaFocused, LV_EVENT_FOCUSED, this);
    lv_obj_add_event_cb(textarea, onTextareaDefocused, LV_EVENT_DEFOCUSED, this);
}

void SearchScreen::createResultsList() {
    // The results list sits between the textarea and keyboard area.
    // When keyboard is hidden it uses the full remaining height.
    int topY = HEADER_HEIGHT + MARGIN + TEXTAREA_HEIGHT + MARGIN;

    resultsList = lv_obj_create(screen);
    lv_obj_set_size(resultsList, LV_PCT(100), 480 - topY);
    lv_obj_set_pos(resultsList, 0, topY);
    lv_obj_set_style_bg_opa(resultsList, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(resultsList, 0, 0);
    lv_obj_set_style_pad_all(resultsList, 0, 0);
    lv_obj_set_style_pad_row(resultsList, 0, 0);
    lv_obj_set_flex_flow(resultsList, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(resultsList, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(resultsList, LV_DIR_VER);
}

// ---------------------------------------------------------------------------
// Search logic
// ---------------------------------------------------------------------------

void SearchScreen::performSearch(const char* query) {
    if (!query || strlen(query) == 0) {
        clearResults();
        cachedResults.clear();
        return;
    }

    SpotifyClient* spotify = App::getInstance().getSpotifyClient();
    if (!spotify || !spotify->isAuthenticated()) {
        return;
    }

    SpotifyClient::SearchResult result = spotify->search(String(query), 20);
    cachedResults = result.tracks;
    populateResults(cachedResults);
}

void SearchScreen::populateResults(const std::vector<SpotifyClient::TrackInfo>& tracks) {
    clearResults();

    for (size_t i = 0; i < tracks.size(); i++) {
        const auto& track = tracks[i];

        // Row container (acts as a tappable button)
        lv_obj_t* row = lv_obj_create(resultsList);
        lv_obj_set_size(row, LV_PCT(100), RESULT_ITEM_HEIGHT);
        lv_obj_set_style_bg_color(row, COLOR_BG, 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(row, COLOR_SURFACE, LV_STATE_PRESSED);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_radius(row, 0, 0);
        lv_obj_set_style_pad_left(row, MARGIN, 0);
        lv_obj_set_style_pad_right(row, MARGIN, 0);
        lv_obj_set_style_pad_top(row, 6, 0);
        lv_obj_set_style_pad_bottom(row, 6, 0);
        lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);

        // Track name
        lv_obj_t* nameLabel = lv_label_create(row);
        lv_obj_set_width(nameLabel, 480 - (MARGIN * 4));
        lv_obj_align(nameLabel, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_set_style_text_font(nameLabel, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(nameLabel, COLOR_TEXT, 0);
        lv_label_set_long_mode(nameLabel, LV_LABEL_LONG_MODE_DOTS);
        lv_label_set_text(nameLabel, track.title.c_str());

        // Artist name
        lv_obj_t* artistLabel = lv_label_create(row);
        lv_obj_set_width(artistLabel, 480 - (MARGIN * 4));
        lv_obj_align(artistLabel, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(artistLabel, COLOR_SECONDARY, 0);
        lv_label_set_long_mode(artistLabel, LV_LABEL_LONG_MODE_DOTS);
        lv_label_set_text(artistLabel, track.artist.c_str());

        // Store index as user data for tap handler
        lv_obj_add_event_cb(row, onResultTapped,
                            LV_EVENT_CLICKED, this);
        // Encode index into the row's user_data via a style property trick:
        // We store the index in the row's layout property that we can read back.
        // Simpler: just use lv_obj_set_user_data.
        lv_obj_set_user_data(row, reinterpret_cast<void*>(i));
    }
}

void SearchScreen::clearResults() {
    if (!resultsList) return;
    lv_obj_clean(resultsList);
}

// ---------------------------------------------------------------------------
// Static callbacks
// ---------------------------------------------------------------------------

void SearchScreen::debounceTimerCb(lv_timer_t* timer) {
    SearchScreen* self = static_cast<SearchScreen*>(lv_timer_get_user_data(timer));
    if (!self || !self->textarea) return;

    const char* text = lv_textarea_get_text(self->textarea);
    self->performSearch(text);

    // Single-shot: delete after firing
    lv_timer_delete(self->debounceTimer);
    self->debounceTimer = nullptr;
}

void SearchScreen::onTextChanged(lv_event_t* e) {
    SearchScreen* self = static_cast<SearchScreen*>(lv_event_get_user_data(e));
    if (!self) return;

    // Reset the debounce timer on every keystroke
    if (self->debounceTimer) {
        lv_timer_delete(self->debounceTimer);
        self->debounceTimer = nullptr;
    }

    const char* text = lv_textarea_get_text(self->textarea);
    if (!text || strlen(text) == 0) {
        self->clearResults();
        self->cachedResults.clear();
        return;
    }

    // Start a new debounce timer (single-shot)
    self->debounceTimer = lv_timer_create(debounceTimerCb, DEBOUNCE_MS, self);
    lv_timer_set_repeat_count(self->debounceTimer, 1);
}

void SearchScreen::onResultTapped(lv_event_t* e) {
    SearchScreen* self = static_cast<SearchScreen*>(lv_event_get_user_data(e));
    if (!self) return;

    lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
    size_t index = reinterpret_cast<size_t>(lv_obj_get_user_data(target));

    if (index >= self->cachedResults.size()) return;

    const auto& track = self->cachedResults[index];

    // Play the selected track
    SpotifyClient* spotify = App::getInstance().getSpotifyClient();
    if (spotify && !track.uri.isEmpty()) {
        spotify->playTrack(track.uri);
    }

    // Navigate to Now Playing
    auto& menuManager = MenuManager::getInstance();
    menuManager.goBack();
}

void SearchScreen::onBackClicked(lv_event_t* e) {
    (void)e;
    auto& menuManager = MenuManager::getInstance();
    menuManager.goBack();
}

void SearchScreen::onTextareaFocused(lv_event_t* e) {
    SearchScreen* self = static_cast<SearchScreen*>(lv_event_get_user_data(e));
    if (!self || !self->keyboard || !self->resultsList) return;

    lv_obj_clear_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);

    // Shrink results list to make room for the keyboard
    int topY = HEADER_HEIGHT + MARGIN + TEXTAREA_HEIGHT + MARGIN;
    int availableHeight = 480 - topY - KEYBOARD_HEIGHT;
    lv_obj_set_height(self->resultsList, availableHeight);
}

void SearchScreen::onTextareaDefocused(lv_event_t* e) {
    SearchScreen* self = static_cast<SearchScreen*>(lv_event_get_user_data(e));
    if (!self || !self->keyboard || !self->resultsList) return;

    lv_obj_add_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);

    // Restore full results list height
    int topY = HEADER_HEIGHT + MARGIN + TEXTAREA_HEIGHT + MARGIN;
    lv_obj_set_height(self->resultsList, 480 - topY);
}

} // namespace ui
