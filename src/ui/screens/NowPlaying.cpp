/**
 * @file NowPlaying.cpp
 * @brief Now Playing Screen Implementation
 */

#include "NowPlaying.hpp"
#include "../../include/config.h"
#include "../display/themes/SpotifyTheme.hpp"
#include "../spotify/SpotifyClient.hpp"
#include "../app/App.hpp"
#include <cmath>

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
    lv_obj_set_style_bg_color(screen, lv_color_hex(COLOR_SPOTIFY_BG), 0);
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
    // Volume control first (creates saveBtn and menuBtn)
    createVolumeControl();

    // Album Art
    createAlbumArt();

    // Track Info
    createTrackInfo();

    // Progress Bar
    createProgressBar();

    // Controls
    createControls();
}

void NowPlayingScreen::createAlbumArt() {
    // Create album art image widget
    albumArt = lv_image_create(screen);

    // Position on the left side
    lv_obj_align(albumArt, LV_ALIGN_LEFT_MID, UI_MARGIN, -20);
    lv_obj_set_size(albumArt, UI_ALBUM_ART_SIZE, UI_ALBUM_ART_SIZE);

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
    lv_obj_align(trackTitleLabel, LV_ALIGN_TOP_LEFT, UI_MARGIN, UI_MARGIN);
    lv_obj_set_style_text_font(trackTitleLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(trackTitleLabel, lv_color_hex(COLOR_SPOTIFY_TEXT), 0);
    lv_label_set_long_mode(trackTitleLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_width(trackTitleLabel, LV_PCT(50));
    lv_label_set_text(trackTitleLabel, "No Track");

    // Artist Name
    artistLabel = lv_label_create(screen);
    lv_obj_align_to(artistLabel, trackTitleLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
    lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(artistLabel, lv_color_hex(COLOR_SPOTIFY_TEXT_SEC), 0);
    lv_label_set_long_mode(artistLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_width(artistLabel, LV_PCT(50));
    lv_label_set_text(artistLabel, "Not Playing");
}

void NowPlayingScreen::createProgressBar() {
    // Progress Bar
    progressBar = lv_bar_create(screen);
    lv_obj_set_size(progressBar, LV_PCT(100) - (UI_MARGIN * 2), UI_PROGRESS_BAR_HEIGHT);
    lv_obj_align(progressBar, LV_ALIGN_BOTTOM_LEFT, UI_MARGIN, -60);

    // Style
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(COLOR_SPOTIFY_SURFACE), 0);
    lv_obj_set_style_bg_opa(progressBar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(progressBar, 2, 0);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(COLOR_SPOTIFY_TEXT), LV_PART_INDICATOR);

    // Make progress bar clickable for seek
    lv_obj_add_flag(progressBar, LV_OBJ_FLAG_CLICKABLE);

    // Progress Labels
    progressLabelLeft = lv_label_create(screen);
    lv_obj_align_to(progressLabelLeft, progressBar, LV_ALIGN_OUT_TOP_LEFT, 0, -4);
    lv_obj_set_style_text_font(progressLabelLeft, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(progressLabelLeft, lv_color_hex(COLOR_SPOTIFY_TEXT_SEC), 0);
    lv_label_set_text(progressLabelLeft, "0:00");

    progressLabelRight = lv_label_create(screen);
    lv_obj_align_to(progressLabelRight, progressBar, LV_ALIGN_OUT_TOP_RIGHT, 0, -4);
    lv_obj_set_style_text_font(progressLabelRight, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(progressLabelRight, lv_color_hex(COLOR_SPOTIFY_TEXT_SEC), 0);
    lv_label_set_text(progressLabelRight, "0:00");

    // Add press event for seek
    lv_obj_add_event_cb(progressBar, [](lv_event_t* e) {
        NowPlayingScreen* screen = static_cast<NowPlayingScreen*>(lv_event_get_user_data(e));

        if (screen && screen->currentTrack.durationMs > 0) {
            lv_obj_t* bar = lv_event_get_target(e);

            if (lv_event_get_code(e) == LV_EVENT_PRESSED || lv_event_get_code(e) == LV_EVENT_PRESSING) {
                lv_point_t point;
                lv_indev_get_point(lv_indev_get_act(), &point);

                lv_area_t barArea;
                lv_obj_get_coords(bar, &barArea);

                // Calculate click position relative to bar width
                int32_t clickX = point.x - barArea.x1;
                int32_t barWidth = barArea.x2 - barArea.x1;

                // Clamp to bar bounds
                if (clickX < 0) clickX = 0;
                if (clickX > barWidth) clickX = barWidth;

                // Calculate percentage
                int32_t percentage = (clickX * 100) / barWidth;

                // Calculate position in milliseconds
                int32_t positionMs = (percentage * screen->currentTrack.durationMs) / 100;

                // Update progress bar immediately for visual feedback
                lv_bar_set_value(bar, percentage, LV_ANIM_OFF);

                // Update left label
                char timeStr[16];
                snprintf(timeStr, sizeof(timeStr), "%d:%02d",
                         positionMs / 60000, (positionMs % 60000) / 1000);
                lv_label_set_text(screen->progressLabelLeft, timeStr);

                // Seek on release
                if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
                    auto* spotify = App::getInstance().getSpotifyClient();
                    if (spotify) {
                        spotify->seek(positionMs);
                        screen->currentTrack.progressMs = positionMs;
                    }
                }
            }
        }
    }, LV_EVENT_ALL, this);
}

void NowPlayingScreen::createControls() {
    // Calculate X positions accounting for volume slider on the right
    int controlAreaWidth = DISPLAY_WIDTH - UI_MARGIN * 2 - UI_VOLUME_SLIDER_WIDTH - 8;
    int centerX = UI_MARGIN + controlAreaWidth / 2;

    // Previous Button
    prevBtn = lv_button_create(screen);
    lv_obj_set_size(prevBtn, UI_BUTTON_PREV_NEXT_SIZE, UI_BUTTON_PREV_NEXT_SIZE);
    lv_obj_align(prevBtn, LV_ALIGN_BOTTOM_LEFT, UI_MARGIN, -UI_MARGIN);
    lv_obj_set_style_bg_color(prevBtn, lv_color_hex(COLOR_SPOTIFY_SURFACE), 0);
    lv_obj_set_style_bg_opa(prevBtn, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_set_style_radius(prevBtn, UI_RADIUS_SMALL, 0);
    lv_obj_set_style_border_width(prevBtn, 0, 0);

    lv_obj_t* prevLabel = lv_label_create(prevBtn);
    lv_obj_center(prevLabel);
    lv_obj_set_style_text_font(prevLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(prevLabel, lv_color_hex(COLOR_SPOTIFY_TEXT), 0);
    lv_label_set_text_static(prevLabel, LV_SYMBOL_SKIP_BACKWARD);

    // Play/Pause Button (larger, centered)
    playPauseBtn = lv_button_create(screen);
    lv_obj_set_size(playPauseBtn, UI_BUTTON_PLAY_SIZE, UI_BUTTON_PLAY_SIZE);
    lv_obj_align(playPauseBtn, LV_ALIGN_BOTTOM_MID, -(UI_VOLUME_SLIDER_WIDTH / 2 + 4), -UI_MARGIN);
    lv_obj_set_style_bg_color(playPauseBtn, lv_color_hex(COLOR_SPOTIFY_PRIMARY), 0);
    lv_obj_set_style_bg_opa(playPauseBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(playPauseBtn, lv_color_hex(COLOR_SPOTIFY_PRIMARY_PRESSED), LV_STATE_PRESSED);
    lv_obj_set_style_radius(playPauseBtn, UI_RADIUS_LARGE, 0);
    lv_obj_set_style_border_width(playPauseBtn, 0, 0);

    lv_obj_t* playPauseLabel = lv_label_create(playPauseBtn);
    lv_obj_center(playPauseLabel);
    lv_obj_set_style_text_font(playPauseLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(playPauseLabel, lv_color_hex(COLOR_SPOTIFY_TEXT), 0);
    lv_label_set_text_static(playPauseLabel, LV_SYMBOL_PLAY);

    // Next Button
    nextBtn = lv_button_create(screen);
    lv_obj_set_size(nextBtn, UI_BUTTON_PREV_NEXT_SIZE, UI_BUTTON_PREV_NEXT_SIZE);
    lv_obj_align(nextBtn, LV_ALIGN_BOTTOM_RIGHT, -(UI_MARGIN + UI_VOLUME_SLIDER_WIDTH + 4), -UI_MARGIN);
    lv_obj_set_style_bg_color(nextBtn, lv_color_hex(COLOR_SPOTIFY_SURFACE), 0);
    lv_obj_set_style_bg_opa(nextBtn, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_set_style_radius(nextBtn, UI_RADIUS_SMALL, 0);
    lv_obj_set_style_border_width(nextBtn, 0, 0);

    lv_obj_t* nextLabel = lv_label_create(nextBtn);
    lv_obj_center(nextLabel);
    lv_obj_set_style_text_font(nextLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(nextLabel, lv_color_hex(COLOR_SPOTIFY_TEXT), 0);
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

    // Save button event handler (toggle save)
    lv_obj_add_event_cb(saveBtn, [](lv_event_t* e) {
        auto* spotify = App::getInstance().getSpotifyClient();
        lv_obj_t* btn = lv_event_get_target(e);

        if (spotify && btn) {
            // Find the label to update visual feedback
            uint32_t childCount = lv_obj_get_child_count(btn);
            lv_obj_t* label = nullptr;
            for (uint32_t i = 0; i < childCount; i++) {
                lv_obj_t* child = lv_obj_get_child(btn, i);
                if (lv_obj_check_type(child, &lv_label_class)) {
                    label = child;
                    break;
                }
            }

            if (label) {
                // Toggle between empty and filled heart
                const char* currentText = lv_label_get_text(label);
                if (strcmp(currentText, LV_SYMBOL_HEART) == 0) {
                    lv_label_set_text_static(label, LV_SYMBOL_AUDIO); // Filled heart
                    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_SPOTIFY_PRIMARY), 0);
                } else {
                    lv_label_set_text_static(label, LV_SYMBOL_HEART); // Empty heart
                    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_SPOTIFY_TEXT_SEC), 0);
                }
            }

            // TODO: Actually save/remove track when track ID is available
            // spotify->saveTrack(trackId) or spotify->removeTrack(trackId)
        }
    }, LV_EVENT_CLICKED, NULL);
}

void NowPlayingScreen::createVolumeControl() {
    // Heart button (save to liked)
    saveBtn = lv_button_create(screen);
    lv_obj_set_size(saveBtn, UI_BUTTON_SMALL_SIZE, UI_BUTTON_SMALL_SIZE);
    lv_obj_align(saveBtn, LV_ALIGN_TOP_RIGHT, -UI_MARGIN - UI_VOLUME_SLIDER_WIDTH, UI_MARGIN);
    lv_obj_set_style_bg_opa(saveBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(saveBtn, 0, 0);

    lv_obj_t* saveLabel = lv_label_create(saveBtn);
    lv_obj_center(saveLabel);
    lv_obj_set_style_text_font(saveLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(saveLabel, lv_color_hex(COLOR_SPOTIFY_TEXT_SEC), 0);
    lv_label_set_text_static(saveLabel, LV_SYMBOL_HEART);

    // Menu button
    menuBtn = lv_button_create(screen);
    lv_obj_set_size(menuBtn, UI_BUTTON_SMALL_SIZE, UI_BUTTON_SMALL_SIZE);
    lv_obj_align_to(menuBtn, saveBtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    lv_obj_set_style_bg_opa(menuBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(menuBtn, 0, 0);

    lv_obj_t* menuLabel = lv_label_create(menuBtn);
    lv_obj_center(menuLabel);
    lv_obj_set_style_text_font(menuLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(menuLabel, lv_color_hex(COLOR_SPOTIFY_TEXT_SEC), 0);
    lv_label_set_text_static(menuLabel, LV_SYMBOL_LIST);

    // Volume slider (vertical on the right side)
    volumeSlider = lv_slider_create(screen);
    lv_obj_set_size(volumeSlider, UI_VOLUME_SLIDER_WIDTH, UI_VOLUME_SLIDER_HEIGHT);
    lv_obj_align(volumeSlider, LV_ALIGN_RIGHT_MID, -UI_MARGIN, 20);

    // Vertical orientation
    lv_slider_set_range(volumeSlider, 0, 100);
    lv_slider_set_value(volumeSlider, currentVolume, LV_ANIM_OFF);
    lv_obj_add_flag(volumeSlider, LV_OBJ_FLAG_CLICKABLE);

    // Style - knob
    lv_obj_set_style_bg_color(volumeSlider, lv_color_hex(COLOR_SPOTIFY_PRIMARY), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(volumeSlider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(volumeSlider, UI_RADIUS_KNOB, LV_PART_KNOB);
    lv_obj_set_style_width(volumeSlider, 30, LV_PART_KNOB);
    lv_obj_set_style_height(volumeSlider, 30, LV_PART_KNOB);

    // Style - main bar
    lv_obj_set_style_bg_color(volumeSlider, lv_color_hex(COLOR_SPOTIFY_SURFACE), 0);
    lv_obj_set_style_bg_opa(volumeSlider, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(volumeSlider, UI_RADIUS_KNOB, 0);

    // Style - filled part
    lv_obj_set_style_bg_color(volumeSlider, lv_color_hex(COLOR_SPOTIFY_PRIMARY), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(volumeSlider, LV_OPA_COVER, LV_PART_INDICATOR);

    // Make slider vertical
    lv_obj_set_style_transform_angle(volumeSlider, 90, 0);

    // Add event handler for volume change
    lv_obj_add_event_cb(volumeSlider, [](lv_event_t* e) {
        NowPlayingScreen* screen = static_cast<NowPlayingScreen*>(lv_event_get_user_data(e));

        if (screen) {
            auto code = lv_event_get_code(e);

            if (code == LV_EVENT_VALUE_CHANGED) {
                lv_obj_t* slider = lv_event_get_target(e);
                int32_t value = lv_slider_get_value(slider);

                screen->currentVolume = value;

                auto* spotify = App::getInstance().getSpotifyClient();
                if (spotify) {
                    spotify->setVolume(value);
                }
            }
        }
    }, LV_EVENT_VALUE_CHANGED, this);

    // Volume icon at top of slider
    lv_obj_t* volumeIcon = lv_label_create(screen);
    lv_obj_set_style_text_font(volumeIcon, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(volumeIcon, lv_color_hex(COLOR_SPOTIFY_TEXT_SEC), 0);
    lv_label_set_text_static(volumeIcon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_align_to(volumeIcon, volumeSlider, LV_ALIGN_OUT_TOP_MID, 0, -4);
    lv_obj_set_style_text_align(volumeIcon, LV_TEXT_ALIGN_CENTER, 0);
}

void NowPlayingScreen::updateTrackInfo(const SpotifyClient::TrackInfo& track) {
    currentTrack = track;

    // Update labels (with NULL checks)
    if (trackTitleLabel) {
        lv_label_set_text(trackTitleLabel, track.title.c_str());
    }
    if (artistLabel) {
        lv_label_set_text(artistLabel, track.artist.c_str());
    }

    // Load album art
    if (!track.coverUrl.isEmpty() && albumArt) {
        loadAlbumArt(track.coverUrl);
    }
}

void NowPlayingScreen::updateProgress(int progressMs, int durationMs) {
    // Validate inputs
    if (durationMs < 0) durationMs = 0;
    if (progressMs < 0) progressMs = 0;
    if (progressMs > durationMs) progressMs = durationMs;

    // Calculate percentage
    int percentage = (durationMs > 0) ? (progressMs * 100) / durationMs : 0;
    percentage = (percentage > 100) ? 100 : percentage;
    percentage = (percentage < 0) ? 0 : percentage;

    // Update progress bar
    if (progressBar) {
        lv_bar_set_value(progressBar, percentage, LV_ANIM_OFF);
    }

    // Format time labels
    if (progressLabelLeft) {
        char timeStr[16];
        snprintf(timeStr, sizeof(timeStr), "%d:%02d",
                 progressMs / 60000, (progressMs % 60000) / 1000);
        lv_label_set_text(progressLabelLeft, timeStr);
    }

    if (progressLabelRight) {
        char timeStr[16];
        snprintf(timeStr, sizeof(timeStr), "%d:%02d",
                 durationMs / 60000, (durationMs % 60000) / 1000);
        lv_label_set_text(progressLabelRight, timeStr);
    }
}

void NowPlayingScreen::updatePlaybackState(bool playing) {
    isPlaying = playing;

    // Update play/pause button - find the label child
    if (playPauseBtn) {
        uint32_t childCount = lv_obj_get_child_count(playPauseBtn);
        for (uint32_t i = 0; i < childCount; i++) {
            lv_obj_t* child = lv_obj_get_child(playPauseBtn, i);
            if (child && lv_obj_check_type(child, &lv_label_class)) {
                lv_label_set_text_static(child,
                                         playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
                break;
            }
        }
    }
}

void NowPlayingScreen::updateVolume(int volumePercent) {
    // Clamp volume to valid range
    currentVolume = constrain(volumePercent, 0, 100);

    // Update volume slider
    if (volumeSlider) {
        lv_slider_set_value(volumeSlider, currentVolume, LV_ANIM_ON);
    }
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
    Serial.printf("🖼️  Loading album art: %s\n", imageUrl.c_str());

    // For now, use a placeholder gradient
    // In production, this would:
    // 1. Check if image is cached in LittleFS
    // 2. Download if not cached
    // 3. Decode JPEG/PNG
    // 4. Create LVGL image descriptor

    // Create a simple placeholder based on image URL hash
    // This ensures different tracks have different "covers"
    uint32_t hash = 0;
    for (size_t i = 0; i < imageUrl.length(); i++) {
        hash = (hash << 5) + hash + imageUrl[i];
    }

    // Generate a gradient based on hash
    uint8_t r = (hash >> 16) & 0xFF;
    uint8_t g = (hash >> 8) & 0xFF;
    uint8_t b = hash & 0xFF;

    // Ensure colors are in dark range
    r = 0x28 + (r % 50);
    g = 0x28 + (g % 50);
    b = 0x28 + (b % 50);

    // Set the album art background color
    lv_obj_set_style_bg_color(albumArt, lv_color_make(r, g, b), 0);
    lv_obj_set_style_bg_opa(albumArt, LV_OPA_COVER, 0);

    Serial.println("   Using placeholder album art (image download requires decoder)");

    // TODO: Implement full image loading:
    // 1. Download image to LittleFS cache
    // 2. Use LVGL image decoder (JPEG, PNG support)
    // 3. Create lv_image_dsc_t and set as source
}

} // namespace ui
