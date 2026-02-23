/**
 * @file NowPlaying.cpp
 * @brief Now Playing Screen Implementation - 480x480 Square Layout
 *
 * Layout (top to bottom):
 *   Top bar (48px): menu | title | heart, settings
 *   Album art (240x240, centered)
 *   Track info: title + artist (centered)
 *   Progress bar + time labels
 *   Controls: shuffle | prev | play/pause | next | repeat
 *   Volume: - slider +
 */

#include "NowPlaying.hpp"
#include "../../display/themes/SpotifyTheme.hpp"
#include "../../spotify/SpotifyClient.hpp"
#include "../../app/App.hpp"
#include <cmath>

// ===== Layout constants for 480x480 =====
#define SCREEN_W        480
#define SCREEN_H        480
#define MARGIN          16
#define MARGIN_WIDE     24

// Top bar
#define TOP_BAR_H       48
#define TOP_BTN_SIZE    40

// Album art
#define ART_SIZE        220
#define ART_Y           56
#define ART_RADIUS      12

// Track info
#define INFO_Y          (ART_Y + ART_SIZE + 12)
#define TITLE_MAX_W     (SCREEN_W - MARGIN * 4)

// Progress
#define PROGRESS_Y      (INFO_Y + 56)
#define PROGRESS_H      6
#define PROGRESS_W      (SCREEN_W - MARGIN * 4)

// Controls
#define CTRL_Y          (PROGRESS_Y + 32)
#define BTN_PLAY_SIZE   56
#define BTN_SKIP_SIZE   44
#define BTN_MODE_SIZE   36
#define CTRL_GAP        16

// Volume
#define VOL_Y           (CTRL_Y + BTN_PLAY_SIZE + 8)
#define VOL_SLIDER_W    (SCREEN_W - MARGIN * 8)
#define VOL_SLIDER_H    6

// Colors
#define BG_COLOR        0x121212
#define SURFACE_COLOR   0x282828
#define GREEN_COLOR     0x1DB954
#define GREEN_DIM       0x1A7A3E
#define TEXT_PRIMARY    0xFFFFFF
#define TEXT_SECONDARY  0xB3B3B3
#define TEXT_DISABLED   0x535353
#define PROGRESS_BG     0x535353

// Debounce
#define VOLUME_DEBOUNCE_MS 500

ui::NowPlayingScreen::NowPlayingScreen(lv_obj_t* parent)
    : screen(nullptr)
    , menuBtn(nullptr), saveBtn(nullptr), saveBtnLabel(nullptr), settingsBtn(nullptr)
    , albumArtContainer(nullptr), albumArtImg(nullptr)
    , albumArtBuffer(nullptr), albumArtBufferSize(0)
    , trackTitleLabel(nullptr), artistLabel(nullptr)
    , progressBar(nullptr), progressLabelLeft(nullptr), progressLabelRight(nullptr)
    , shuffleBtn(nullptr), shuffleBtnLabel(nullptr)
    , prevBtn(nullptr)
    , playPauseBtn(nullptr), playPauseBtnLabel(nullptr)
    , nextBtn(nullptr)
    , repeatBtn(nullptr), repeatBtnLabel(nullptr)
    , volumeLabel(nullptr), volumeSlider(nullptr), volumePlusLabel(nullptr)
    , isPlaying(false)
    , currentVolume(50)
    , isSaved(false)
    , isShuffled(false)
    , repeatMode(0)
    , lastVolumeChangeMs(0)
    , lastProgressUpdateMs(0)
    , interpolatedProgressMs(0) {

    screen = lv_obj_create(parent);
    lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
    lv_obj_set_pos(screen, 0, 0);
    lv_obj_set_style_bg_color(screen, lv_color_hex(BG_COLOR), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

    createUI();
}

ui::NowPlayingScreen::~NowPlayingScreen() {
    if (albumArtBuffer) {
        free(albumArtBuffer);
        albumArtBuffer = nullptr;
    }
    if (screen) {
        lv_obj_del(screen);
    }
}

void ui::NowPlayingScreen::createUI() {
    createTopBar();
    createAlbumArt();
    createTrackInfo();
    createProgressBar();
    createControls();
    createVolumeControl();
}

// ===== Top Bar: Menu | Save | Settings =====
void ui::NowPlayingScreen::createTopBar() {
    // Menu button (hamburger) - top left
    menuBtn = lv_button_create(screen);
    lv_obj_set_size(menuBtn, TOP_BTN_SIZE, TOP_BTN_SIZE);
    lv_obj_set_pos(menuBtn, MARGIN, 4);
    lv_obj_set_style_bg_opa(menuBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(menuBtn, 0, 0);
    lv_obj_set_style_shadow_width(menuBtn, 0, 0);

    lv_obj_t* menuLabel = lv_label_create(menuBtn);
    lv_obj_center(menuLabel);
    lv_obj_set_style_text_font(menuLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(menuLabel, lv_color_hex(TEXT_SECONDARY), 0);
    lv_label_set_text_static(menuLabel, LV_SYMBOL_LIST);

    lv_obj_add_event_cb(menuBtn, [](lv_event_t* e) {
        ui::NowPlayingScreen* self = static_cast<ui::NowPlayingScreen*>(lv_event_get_user_data(e));
        if (self && self->onMenuPressed) {
            self->onMenuPressed();
        }
    }, LV_EVENT_CLICKED, this);

    // Settings button - top right
    settingsBtn = lv_button_create(screen);
    lv_obj_set_size(settingsBtn, TOP_BTN_SIZE, TOP_BTN_SIZE);
    lv_obj_set_pos(settingsBtn, SCREEN_W - MARGIN - TOP_BTN_SIZE, 4);
    lv_obj_set_style_bg_opa(settingsBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(settingsBtn, 0, 0);
    lv_obj_set_style_shadow_width(settingsBtn, 0, 0);

    lv_obj_t* settingsLabel = lv_label_create(settingsBtn);
    lv_obj_center(settingsLabel);
    lv_obj_set_style_text_font(settingsLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(settingsLabel, lv_color_hex(TEXT_SECONDARY), 0);
    lv_label_set_text_static(settingsLabel, LV_SYMBOL_SETTINGS);

    lv_obj_add_event_cb(settingsBtn, [](lv_event_t* e) {
        ui::NowPlayingScreen* self = static_cast<ui::NowPlayingScreen*>(lv_event_get_user_data(e));
        if (self && self->onMenuPressed) {
            self->onMenuPressed();
        }
    }, LV_EVENT_CLICKED, this);

    // Save/heart button - next to settings
    saveBtn = lv_button_create(screen);
    lv_obj_set_size(saveBtn, TOP_BTN_SIZE, TOP_BTN_SIZE);
    lv_obj_set_pos(saveBtn, SCREEN_W - MARGIN - TOP_BTN_SIZE * 2 - 4, 4);
    lv_obj_set_style_bg_opa(saveBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(saveBtn, 0, 0);
    lv_obj_set_style_shadow_width(saveBtn, 0, 0);

    saveBtnLabel = lv_label_create(saveBtn);
    lv_obj_center(saveBtnLabel);
    lv_obj_set_style_text_font(saveBtnLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(saveBtnLabel, lv_color_hex(TEXT_SECONDARY), 0);
    lv_label_set_text_static(saveBtnLabel, LV_SYMBOL_OK); // Heart not available, use checkmark

    lv_obj_add_event_cb(saveBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        ui::NowPlayingScreen* self = static_cast<ui::NowPlayingScreen*>(lv_event_get_user_data(e));
        if (spotify && self && !self->currentTrack.id.isEmpty()) {
            if (self->isSaved) {
                spotify->removeTrack(self->currentTrack.id);
                self->updateSaveState(false);
            } else {
                spotify->saveTrack(self->currentTrack.id);
                self->updateSaveState(true);
            }
        }
    }, LV_EVENT_CLICKED, this);
}

// ===== Album Art: 220x220 centered =====
void ui::NowPlayingScreen::createAlbumArt() {
    albumArtContainer = lv_obj_create(screen);
    int artX = (SCREEN_W - ART_SIZE) / 2;
    lv_obj_set_pos(albumArtContainer, artX, ART_Y);
    lv_obj_set_size(albumArtContainer, ART_SIZE, ART_SIZE);
    lv_obj_set_style_radius(albumArtContainer, ART_RADIUS, 0);
    lv_obj_set_style_border_width(albumArtContainer, 0, 0);
    lv_obj_set_style_bg_color(albumArtContainer, lv_color_hex(SURFACE_COLOR), 0);
    lv_obj_set_style_bg_opa(albumArtContainer, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(albumArtContainer, 0, 0);
    lv_obj_set_scrollbar_mode(albumArtContainer, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_clip_corner(albumArtContainer, true, 0);

    // Placeholder icon (music note)
    lv_obj_t* placeholder = lv_label_create(albumArtContainer);
    lv_obj_center(placeholder);
    lv_obj_set_style_text_font(placeholder, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(placeholder, lv_color_hex(TEXT_DISABLED), 0);
    lv_label_set_text_static(placeholder, LV_SYMBOL_AUDIO);
}

// ===== Track Info: title + artist, centered =====
void ui::NowPlayingScreen::createTrackInfo() {
    trackTitleLabel = lv_label_create(screen);
    lv_obj_set_width(trackTitleLabel, TITLE_MAX_W);
    lv_obj_set_pos(trackTitleLabel, 0, INFO_Y);
    lv_obj_set_style_text_font(trackTitleLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(trackTitleLabel, lv_color_hex(TEXT_PRIMARY), 0);
    lv_obj_set_style_text_align(trackTitleLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(trackTitleLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_align(trackTitleLabel, LV_ALIGN_TOP_MID, 0, INFO_Y);
    lv_label_set_text(trackTitleLabel, "No Track Playing");

    artistLabel = lv_label_create(screen);
    lv_obj_set_width(artistLabel, TITLE_MAX_W);
    lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(artistLabel, lv_color_hex(TEXT_SECONDARY), 0);
    lv_obj_set_style_text_align(artistLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(artistLabel, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_align_to(artistLabel, trackTitleLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_label_set_text(artistLabel, "Connect to Spotify");
}

// ===== Progress Bar + Time Labels =====
void ui::NowPlayingScreen::createProgressBar() {
    // Progress bar
    progressBar = lv_bar_create(screen);
    lv_obj_set_size(progressBar, PROGRESS_W, PROGRESS_H);
    lv_obj_align(progressBar, LV_ALIGN_TOP_MID, 0, PROGRESS_Y);
    lv_bar_set_range(progressBar, 0, 1000); // Use 0-1000 for finer resolution
    lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(PROGRESS_BG), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(progressBar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(progressBar, 3, LV_PART_MAIN);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(GREEN_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_radius(progressBar, 3, LV_PART_INDICATOR);

    // Time left label
    progressLabelLeft = lv_label_create(screen);
    lv_obj_set_style_text_font(progressLabelLeft, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(progressLabelLeft, lv_color_hex(TEXT_SECONDARY), 0);
    lv_obj_align(progressLabelLeft, LV_ALIGN_TOP_LEFT, MARGIN * 2, PROGRESS_Y + PROGRESS_H + 4);
    lv_label_set_text(progressLabelLeft, "0:00");

    // Time right label
    progressLabelRight = lv_label_create(screen);
    lv_obj_set_style_text_font(progressLabelRight, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(progressLabelRight, lv_color_hex(TEXT_SECONDARY), 0);
    lv_obj_align(progressLabelRight, LV_ALIGN_TOP_RIGHT, -MARGIN * 2, PROGRESS_Y + PROGRESS_H + 4);
    lv_label_set_text(progressLabelRight, "0:00");
}

// ===== Controls: shuffle | prev | play/pause | next | repeat =====
void ui::NowPlayingScreen::createControls() {
    int centerX = SCREEN_W / 2;
    int ctrlY = CTRL_Y;

    // Shuffle button (left of prev)
    shuffleBtn = lv_button_create(screen);
    lv_obj_set_size(shuffleBtn, BTN_MODE_SIZE, BTN_MODE_SIZE);
    lv_obj_set_pos(shuffleBtn, centerX - BTN_PLAY_SIZE / 2 - CTRL_GAP - BTN_SKIP_SIZE - CTRL_GAP - BTN_MODE_SIZE,
                   ctrlY + (BTN_PLAY_SIZE - BTN_MODE_SIZE) / 2);
    lv_obj_set_style_bg_opa(shuffleBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(shuffleBtn, 0, 0);
    lv_obj_set_style_shadow_width(shuffleBtn, 0, 0);

    shuffleBtnLabel = lv_label_create(shuffleBtn);
    lv_obj_center(shuffleBtnLabel);
    lv_obj_set_style_text_font(shuffleBtnLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(shuffleBtnLabel, lv_color_hex(TEXT_DISABLED), 0);
    lv_label_set_text_static(shuffleBtnLabel, LV_SYMBOL_SHUFFLE);

    lv_obj_add_event_cb(shuffleBtn, [](lv_event_t* e) {
        NowPlayingScreen* self = static_cast<NowPlayingScreen*>(lv_event_get_user_data(e));
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify && self) {
            bool newState = !self->isShuffled;
            spotify->setShuffle(newState);
            self->updateShuffleState(newState);
        }
    }, LV_EVENT_CLICKED, this);

    // Previous button
    prevBtn = lv_button_create(screen);
    lv_obj_set_size(prevBtn, BTN_SKIP_SIZE, BTN_SKIP_SIZE);
    lv_obj_set_pos(prevBtn, centerX - BTN_PLAY_SIZE / 2 - CTRL_GAP - BTN_SKIP_SIZE,
                   ctrlY + (BTN_PLAY_SIZE - BTN_SKIP_SIZE) / 2);
    lv_obj_set_style_bg_color(prevBtn, lv_color_hex(SURFACE_COLOR), 0);
    lv_obj_set_style_bg_opa(prevBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(prevBtn, BTN_SKIP_SIZE / 2, 0);
    lv_obj_set_style_border_width(prevBtn, 0, 0);
    lv_obj_set_style_shadow_width(prevBtn, 0, 0);

    lv_obj_t* prevLabel = lv_label_create(prevBtn);
    lv_obj_center(prevLabel);
    lv_obj_set_style_text_font(prevLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(prevLabel, lv_color_hex(TEXT_PRIMARY), 0);
    lv_label_set_text_static(prevLabel, LV_SYMBOL_PREV);

    lv_obj_add_event_cb(prevBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify) spotify->previousTrack();
    }, LV_EVENT_CLICKED, nullptr);

    // Play/Pause button (center, larger, green)
    playPauseBtn = lv_button_create(screen);
    lv_obj_set_size(playPauseBtn, BTN_PLAY_SIZE, BTN_PLAY_SIZE);
    lv_obj_set_pos(playPauseBtn, centerX - BTN_PLAY_SIZE / 2, ctrlY);
    lv_obj_set_style_bg_color(playPauseBtn, lv_color_hex(GREEN_COLOR), 0);
    lv_obj_set_style_bg_opa(playPauseBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(playPauseBtn, BTN_PLAY_SIZE / 2, 0);
    lv_obj_set_style_border_width(playPauseBtn, 0, 0);
    lv_obj_set_style_shadow_width(playPauseBtn, 0, 0);
    // Pressed state
    lv_obj_set_style_bg_color(playPauseBtn, lv_color_hex(GREEN_DIM), LV_STATE_PRESSED);

    playPauseBtnLabel = lv_label_create(playPauseBtn);
    lv_obj_center(playPauseBtnLabel);
    lv_obj_set_style_text_font(playPauseBtnLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(playPauseBtnLabel, lv_color_hex(TEXT_PRIMARY), 0);
    lv_label_set_text_static(playPauseBtnLabel, LV_SYMBOL_PLAY);

    lv_obj_add_event_cb(playPauseBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify) spotify->togglePlay();
    }, LV_EVENT_CLICKED, nullptr);

    // Next button
    nextBtn = lv_button_create(screen);
    lv_obj_set_size(nextBtn, BTN_SKIP_SIZE, BTN_SKIP_SIZE);
    lv_obj_set_pos(nextBtn, centerX + BTN_PLAY_SIZE / 2 + CTRL_GAP,
                   ctrlY + (BTN_PLAY_SIZE - BTN_SKIP_SIZE) / 2);
    lv_obj_set_style_bg_color(nextBtn, lv_color_hex(SURFACE_COLOR), 0);
    lv_obj_set_style_bg_opa(nextBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(nextBtn, BTN_SKIP_SIZE / 2, 0);
    lv_obj_set_style_border_width(nextBtn, 0, 0);
    lv_obj_set_style_shadow_width(nextBtn, 0, 0);

    lv_obj_t* nextLabel = lv_label_create(nextBtn);
    lv_obj_center(nextLabel);
    lv_obj_set_style_text_font(nextLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(nextLabel, lv_color_hex(TEXT_PRIMARY), 0);
    lv_label_set_text_static(nextLabel, LV_SYMBOL_NEXT);

    lv_obj_add_event_cb(nextBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify) spotify->nextTrack();
    }, LV_EVENT_CLICKED, nullptr);

    // Repeat button (right of next)
    repeatBtn = lv_button_create(screen);
    lv_obj_set_size(repeatBtn, BTN_MODE_SIZE, BTN_MODE_SIZE);
    lv_obj_set_pos(repeatBtn, centerX + BTN_PLAY_SIZE / 2 + CTRL_GAP + BTN_SKIP_SIZE + CTRL_GAP,
                   ctrlY + (BTN_PLAY_SIZE - BTN_MODE_SIZE) / 2);
    lv_obj_set_style_bg_opa(repeatBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(repeatBtn, 0, 0);
    lv_obj_set_style_shadow_width(repeatBtn, 0, 0);

    repeatBtnLabel = lv_label_create(repeatBtn);
    lv_obj_center(repeatBtnLabel);
    lv_obj_set_style_text_font(repeatBtnLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(repeatBtnLabel, lv_color_hex(TEXT_DISABLED), 0);
    lv_label_set_text_static(repeatBtnLabel, LV_SYMBOL_LOOP);

    lv_obj_add_event_cb(repeatBtn, [](lv_event_t* e) {
        NowPlayingScreen* self = static_cast<NowPlayingScreen*>(lv_event_get_user_data(e));
        auto* spotify = App::getInstance().getSpotifyClient();
        if (spotify && self) {
            // Cycle: off(0) -> context(1) -> track(2) -> off(0)
            int newMode = (self->repeatMode + 1) % 3;
            const char* states[] = {"off", "context", "track"};
            spotify->setRepeat(states[newMode]);
            self->updateRepeatState(newMode);
        }
    }, LV_EVENT_CLICKED, this);
}

// ===== Volume: - slider + =====
void ui::NowPlayingScreen::createVolumeControl() {
    int volY = VOL_Y;

    // Minus label
    volumeLabel = lv_label_create(screen);
    lv_obj_set_pos(volumeLabel, MARGIN * 3, volY);
    lv_obj_set_style_text_font(volumeLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(volumeLabel, lv_color_hex(TEXT_SECONDARY), 0);
    lv_label_set_text_static(volumeLabel, LV_SYMBOL_VOLUME_MID);

    // Volume slider
    volumeSlider = lv_slider_create(screen);
    lv_obj_set_size(volumeSlider, VOL_SLIDER_W, VOL_SLIDER_H);
    lv_obj_align(volumeSlider, LV_ALIGN_TOP_MID, 0, volY);
    lv_slider_set_range(volumeSlider, 0, 100);
    lv_slider_set_value(volumeSlider, currentVolume, LV_ANIM_OFF);

    // Slider styles
    lv_obj_set_style_bg_color(volumeSlider, lv_color_hex(PROGRESS_BG), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(volumeSlider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(volumeSlider, 3, LV_PART_MAIN);
    lv_obj_set_style_bg_color(volumeSlider, lv_color_hex(GREEN_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_radius(volumeSlider, 3, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(volumeSlider, lv_color_hex(TEXT_PRIMARY), LV_PART_KNOB);
    lv_obj_set_style_pad_all(volumeSlider, 6, LV_PART_KNOB);

    // Plus label
    volumePlusLabel = lv_label_create(screen);
    lv_obj_set_pos(volumePlusLabel, SCREEN_W - MARGIN * 3 - 16, volY);
    lv_obj_set_style_text_font(volumePlusLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(volumePlusLabel, lv_color_hex(TEXT_SECONDARY), 0);
    lv_label_set_text_static(volumePlusLabel, LV_SYMBOL_VOLUME_MAX);

    // Volume change callback with debounce
    lv_obj_add_event_cb(volumeSlider, [](lv_event_t* e) {
        ui::NowPlayingScreen* self = static_cast<ui::NowPlayingScreen*>(lv_event_get_user_data(e));
        if (!self) return;

        int newVol = lv_slider_get_value(static_cast<lv_obj_t*>(lv_event_get_target(e)));
        self->currentVolume = newVol;

        // Debounce: only send API call if enough time has passed
        unsigned long now = millis();
        if (now - self->lastVolumeChangeMs >= VOLUME_DEBOUNCE_MS) {
            self->lastVolumeChangeMs = now;
            auto* spotify = App::getInstance().getSpotifyClient();
            if (spotify) {
                spotify->setVolume(newVol);
            }
        }
    }, LV_EVENT_VALUE_CHANGED, this);
}

// ===== Update Methods =====

void ui::NowPlayingScreen::updateTrackInfo(const SpotifyClient::TrackInfo& track) {
    currentTrack = track;

    if (trackTitleLabel) {
        lv_label_set_text(trackTitleLabel, track.title.c_str());
    }
    if (artistLabel) {
        lv_label_set_text(artistLabel, track.artist.c_str());
    }

    // Load album art if URL changed
    if (!track.coverUrl.isEmpty() && track.coverUrl != currentArtUrl) {
        currentArtUrl = track.coverUrl;
        loadAlbumArt(track.coverUrl);
    }
}

void ui::NowPlayingScreen::updateProgress(int progressMs, int durationMs) {
    if (durationMs <= 0) durationMs = 1;
    if (progressMs < 0) progressMs = 0;
    if (progressMs > durationMs) progressMs = durationMs;

    // Use 0-1000 range for smoother bar
    int permille = (int)((long long)progressMs * 1000 / durationMs);
    if (permille > 1000) permille = 1000;
    if (permille < 0) permille = 0;

    if (progressBar) lv_bar_set_value(progressBar, permille, LV_ANIM_ON);

    char timeBuf[16];

    if (progressLabelLeft) {
        formatTime(progressMs, timeBuf, sizeof(timeBuf));
        lv_label_set_text(progressLabelLeft, timeBuf);
    }

    if (progressLabelRight) {
        formatTime(durationMs, timeBuf, sizeof(timeBuf));
        lv_label_set_text(progressLabelRight, timeBuf);
    }
}

void ui::NowPlayingScreen::updatePlaybackState(bool playing) {
    isPlaying = playing;
    if (playPauseBtnLabel) {
        lv_label_set_text_static(playPauseBtnLabel, playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
    }
}

void ui::NowPlayingScreen::updateVolume(int volumePercent) {
    currentVolume = constrain(volumePercent, 0, 100);
    if (volumeSlider) {
        lv_slider_set_value(volumeSlider, currentVolume, LV_ANIM_ON);
    }
}

void ui::NowPlayingScreen::updateSaveState(bool saved) {
    isSaved = saved;
    if (saveBtnLabel) {
        lv_obj_set_style_text_color(saveBtnLabel,
            saved ? lv_color_hex(GREEN_COLOR) : lv_color_hex(TEXT_SECONDARY), 0);
    }
}

void ui::NowPlayingScreen::updateShuffleState(bool shuffle) {
    isShuffled = shuffle;
    if (shuffleBtnLabel) {
        lv_obj_set_style_text_color(shuffleBtnLabel,
            shuffle ? lv_color_hex(GREEN_COLOR) : lv_color_hex(TEXT_DISABLED), 0);
    }
}

void ui::NowPlayingScreen::updateRepeatState(int mode) {
    repeatMode = mode;
    if (repeatBtnLabel) {
        lv_obj_set_style_text_color(repeatBtnLabel,
            mode > 0 ? lv_color_hex(GREEN_COLOR) : lv_color_hex(TEXT_DISABLED), 0);
        // Could change icon for track repeat vs context repeat
    }
}

void ui::NowPlayingScreen::update() {
    auto* spotify = App::getInstance().getSpotifyClient();
    if (!spotify) return;

    SpotifyClient::TrackInfo track = spotify->getCurrentTrack();

    // Update track info if changed
    if (track.title != currentTrack.title || track.artist != currentTrack.artist) {
        updateTrackInfo(track);
    }

    // Update play/pause state
    if (track.isPlaying != isPlaying) {
        updatePlaybackState(track.isPlaying);
    }

    // Update progress with interpolation
    if (track.isPlaying) {
        // Interpolate progress between API polls
        unsigned long now = millis();
        int elapsed = (int)(now - lastProgressUpdateMs);
        interpolatedProgressMs = track.progressMs + elapsed;
        if (interpolatedProgressMs > track.durationMs) {
            interpolatedProgressMs = track.durationMs;
        }
        updateProgress(interpolatedProgressMs, track.durationMs);
    } else {
        updateProgress(track.progressMs, track.durationMs);
    }
    lastProgressUpdateMs = millis();

    // Update volume if different
    if (track.volumePercent != currentVolume) {
        updateVolume(track.volumePercent);
    }

    // Update shuffle/repeat state from API
    if (track.shuffleState != isShuffled) {
        updateShuffleState(track.shuffleState);
    }
    if (track.repeatMode != repeatMode) {
        updateRepeatState(track.repeatMode);
    }
}

void ui::NowPlayingScreen::loadAlbumArt(const String& imageUrl) {
    // Generate a colored placeholder based on URL hash
    uint32_t hash = 0;
    for (size_t i = 0; i < imageUrl.length(); i++) {
        hash = (hash * 31) + imageUrl[i];
    }
    uint8_t r = 0x28 + ((hash >> 16) & 0x3F);
    uint8_t g = 0x28 + ((hash >> 8) & 0x3F);
    uint8_t b = 0x28 + (hash & 0x3F);

    if (albumArtContainer) {
        lv_obj_set_style_bg_color(albumArtContainer, lv_color_make(r, g, b), 0);
    }

    // TODO: Phase 1.2 - Download JPEG and decode to LVGL image
    // This requires:
    // 1. Download JPEG from Spotify CDN to PSRAM buffer
    // 2. Use LVGL's built-in SJPG decoder or TJpgDecoder to decode
    // 3. Create lv_img_dsc_t and set on albumArtImg
    // For now, the colored placeholder with music icon works.
}

void ui::NowPlayingScreen::downloadAndDisplayArt(const String& url) {
    // Future implementation: download JPEG from URL and display
    // Will use WiFiClientSecure to download, PSRAM for buffer,
    // and LVGL image widget to display
}

void ui::NowPlayingScreen::formatTime(int ms, char* buf, size_t bufSize) {
    if (ms < 0) ms = 0;
    int totalSec = ms / 1000;
    int min = totalSec / 60;
    int sec = totalSec % 60;
    snprintf(buf, bufSize, "%d:%02d", min, sec);
}
