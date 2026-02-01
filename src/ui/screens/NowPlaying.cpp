/**
 * @file NowPlaying.cpp
 * @brief Now Playing Screen Implementation
 */

#include "NowPlaying.hpp"
#include "../display/themes/SpotifyTheme.hpp"
#include "../spotify/SpotifyClient.hpp"

// UI Layout constants (for 320x480 landscape)
#define MARGIN 16
#define ALBUM_ART_SIZE 220
#define CONTROLS_Y 400
#define PROGRESS_Y 360

NowPlayingScreen::NowPlayingScreen(lv_obj_t* parent)
    : screen(nullptr)
    , albumArt(nullptr)
    , trackTitleLabel(nullptr)
    , artistLabel(nullptr)
    , progressBar(nullptr)
    , progressLabelLeft(nullptr)
    , progressLabelRight(nullptr)
    , playPauseBtn(nullptr)
    , prevBtn(nullptr)
    , nextBtn(nullptr)
    , saveBtn(nullptr)
    , volumeSlider(nullptr)
    , menuBtn(nullptr)
    , isPlaying(false)
    , currentVolume(50) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    createUI();
}

NowPlayingScreen::~NowPlayingScreen() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void NowPlayingScreen::createUI() {
    // Album Art
    createAlbumArt();

    // Track Info
    createTrackInfo();

    // Progress Bar
    createProgressBar();

    // Controls
    createControls();

    // Volume
    createVolumeControl();
}

void NowPlayingScreen::createAlbumArt() {
    // Create album art image widget
    albumArt = lv_image_create(screen);

    // Position on the left side
    lv_obj_align(albumArt, LV_ALIGN_LEFT_MID, MARGIN, -20);
    lv_obj_set_size(albumArt, ALBUM_ART_SIZE, ALBUM_ART_SIZE);

    // Style
    lv_obj_set_style_radius(albumArt, 12, 0);
    lv_obj_set_style_border_width(albumArt, 0, 0);
    lv_obj_set_style_shadow_width(albumArt, 0, 0);

    // Placeholder gradient
    static uint8_t gradient_map[] = {0x12, 0x12, 0x12, 0x28, 0x28, 0x28};
    lv_image_set_src(albumArt, gradient_map);
}

void NowPlayingScreen::createTrackInfo() {
    // Track Title
    trackTitleLabel = lv_label_create(screen);
    lv_obj_align(trackTitleLabel, LV_ALIGN_TOP_LEFT, MARGIN, MARGIN);
    lv_obj_set_style_text_font(trackTitleLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(trackTitleLabel, lv_color_white(), 0);
    lv_label_set_long_mode(trackTitleLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_width(trackTitleLabel, LV_PCT(50));
    lv_label_set_text(trackTitleLabel, "No Track");

    // Artist Name
    artistLabel = lv_label_create(screen);
    lv_obj_align_to(artistLabel, trackTitleLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
    lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(artistLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_long_mode(artistLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_width(artistLabel, LV_PCT(50));
    lv_label_set_text(artistLabel, "Not Playing");
}

void NowPlayingScreen::createProgressBar() {
    // Progress Bar
    progressBar = lv_bar_create(screen);
    lv_obj_set_size(progressBar, LV_PCT(100) - (MARGIN * 2), 4);
    lv_obj_align(progressBar, LV_ALIGN_BOTTOM_LEFT, MARGIN, -60);

    // Style
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(progressBar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(progressBar, 2, 0);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR);

    // Progress Labels
    progressLabelLeft = lv_label_create(screen);
    lv_obj_align_to(progressLabelLeft, progressBar, LV_ALIGN_OUT_TOP_LEFT, 0, -4);
    lv_obj_set_style_text_font(progressLabelLeft, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(progressLabelLeft, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text(progressLabelLeft, "0:00");

    progressLabelRight = lv_label_create(screen);
    lv_obj_align_to(progressLabelRight, progressBar, LV_ALIGN_OUT_TOP_RIGHT, 0, -4);
    lv_obj_set_style_text_font(progressLabelRight, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(progressLabelRight, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text(progressLabelRight, "0:00");
}

void NowPlayingScreen::createControls() {
    // Previous Button
    prevBtn = lv_button_create(screen);
    lv_obj_set_size(prevBtn, 44, 44);
    lv_obj_align(prevBtn, LV_ALIGN_BOTTOM_LEFT, MARGIN, -MARGIN);
    lv_obj_set_style_bg_color(prevBtn, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(prevBtn, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_set_style_radius(prevBtn, 22, 0);
    lv_obj_set_style_border_width(prevBtn, 0, 0);

    lv_obj_t* prevLabel = lv_label_create(prevBtn);
    lv_obj_center(prevLabel);
    lv_obj_set_style_text_font(prevLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(prevLabel, lv_color_white(), 0);
    lv_label_set_text_static(prevLabel, LV_SYMBOL_SKIP_BACKWARD);

    // Play/Pause Button (larger)
    playPauseBtn = lv_button_create(screen);
    lv_obj_set_size(playPauseBtn, 56, 56);
    lv_obj_align(playPauseBtn, LV_ALIGN_BOTTOM_MID, 0, -MARGIN);
    lv_obj_set_style_bg_color(playPauseBtn, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_bg_opa(playPauseBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(playPauseBtn, lv_color_hex(0x1ED760), LV_STATE_PRESSED);
    lv_obj_set_style_radius(playPauseBtn, 28, 0);
    lv_obj_set_style_border_width(playPauseBtn, 0, 0);

    lv_obj_t* playPauseLabel = lv_label_create(playPauseBtn);
    lv_obj_center(playPauseLabel);
    lv_obj_set_style_text_font(playPauseLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(playPauseLabel, lv_color_white(), 0);
    lv_label_set_text_static(playPauseLabel, LV_SYMBOL_PLAY);

    // Next Button
    nextBtn = lv_button_create(screen);
    lv_obj_set_size(nextBtn, 44, 44);
    lv_obj_align(nextBtn, LV_ALIGN_BOTTOM_RIGHT, -MARGIN, -MARGIN);
    lv_obj_set_style_bg_color(nextBtn, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(nextBtn, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_set_style_radius(nextBtn, 22, 0);
    lv_obj_set_style_border_width(nextBtn, 0, 0);

    lv_obj_t* nextLabel = lv_label_create(nextBtn);
    lv_obj_center(nextLabel);
    lv_obj_set_style_text_font(nextLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(nextLabel, lv_color_white(), 0);
    lv_label_set_text_static(nextLabel, LV_SYMBOL_SKIP_FORWARD);

    // Add event handlers
    lv_obj_add_event_cb(prevBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify) spotify->previousTrack();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(playPauseBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify) spotify->togglePlay();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(nextBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify) spotify->nextTrack();
    }, LV_EVENT_CLICKED, NULL);
}

void NowPlayingScreen::createVolumeControl() {
    // Volume button/icon
    saveBtn = lv_button_create(screen);
    lv_obj_set_size(saveBtn, 40, 40);
    lv_obj_align(saveBtn, LV_ALIGN_TOP_RIGHT, -MARGIN, MARGIN);
    lv_obj_set_style_bg_opa(saveBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(saveBtn, 0, 0);

    lv_obj_t* saveLabel = lv_label_create(saveBtn);
    lv_obj_center(saveLabel);
    lv_obj_set_style_text_font(saveLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(saveLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(saveLabel, LV_SYMBOL_HEART);

    // Menu button
    menuBtn = lv_button_create(screen);
    lv_obj_set_size(menuBtn, 40, 40);
    lv_obj_align_to(menuBtn, saveBtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    lv_obj_set_style_bg_opa(menuBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(menuBtn, 0, 0);

    lv_obj_t* menuLabel = lv_label_create(menuBtn);
    lv_obj_center(menuLabel);
    lv_obj_set_style_text_font(menuLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(menuLabel, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text_static(menuLabel, LV_SYMBOL_LIST);
}

void NowPlayingScreen::updateTrackInfo(const SpotifyClient::TrackInfo& track) {
    currentTrack = track;

    // Update labels
    lv_label_set_text(trackTitleLabel, track.title.c_str());
    lv_label_set_text(artistLabel, track.artist.c_str());

    // Load album art
    if (!track.coverUrl.isEmpty()) {
        loadAlbumArt(track.coverUrl);
    }
}

void NowPlayingScreen::updateProgress(int progressMs, int durationMs) {
    // Calculate percentage
    int percentage = (progressMs * 100) / durationMs;

    // Update progress bar
    lv_bar_set_value(progressBar, percentage, LV_ANIM_ON);

    // Format time labels
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%d:%02d",
             progressMs / 60000, (progressMs % 60000) / 1000);
    lv_label_set_text(progressLabelLeft, timeStr);

    snprintf(timeStr, sizeof(timeStr), "%d:%02d",
             durationMs / 60000, (durationMs % 60000) / 1000);
    lv_label_set_text(progressLabelRight, timeStr);
}

void NowPlayingScreen::updatePlaybackState(bool playing) {
    isPlaying = playing;

    // Update play/pause button - find the label child
    if (playPauseBtn) {
        uint32_t childCount = lv_obj_get_child_count(playPauseBtn);
        for (uint32_t i = 0; i < childCount; i++) {
            lv_obj_t* child = lv_obj_get_child(playPauseBtn, i);
            if (lv_obj_check_type(child, &lv_label_class)) {
                lv_label_set_text_static(child,
                                         playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
                break;
            }
        }
    }
}

void NowPlayingScreen::updateVolume(int volumePercent) {
    currentVolume = volumePercent;
    // Volume is controlled via separate UI (slider or buttons)
}

void NowPlayingScreen::update() {
    // Called periodically to update UI
    auto* app = &App::getInstance();
    auto* spotify = app->getSpotifyClient();

    if (spotify) {
        SpotifyClient::TrackInfo track = spotify->getCurrentTrack();
        if (track.title != currentTrack.title) {
            updateTrackInfo(track);
        }

        if (track.isPlaying != isPlaying) {
            updatePlaybackState(track.isPlaying);
        }

        updateProgress(track.progressMs, track.durationMs);
    }
}

void NowPlayingScreen::loadAlbumArt(const String& imageUrl) {
    // TODO: Implement album art loading
    // For now, use a placeholder
    // In Phase 2, we'll download and cache images
}

} // namespace ui
