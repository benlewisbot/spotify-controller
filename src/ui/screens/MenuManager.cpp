/**
 * @file MenuManager.cpp
 * @brief Screen Navigation Manager Implementation
 */

#include "MenuManager.hpp"
#include "Settings.hpp"
#include "../../display/themes/SpotifyTheme.hpp"

#define SPINNER_COLOR lv_color_hex(0x1DB954)
#define OVERLAY_BG lv_color_hex(0x000000)

void MenuManager::init(lv_obj_t* rootObj) {
    root = rootObj;
    currentScreen = ScreenType::SETTINGS_MAIN;
    
    // Create loading overlay (hidden by default)
    loadingOverlay = lv_obj_create(root);
    lv_obj_set_size(loadingOverlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(loadingOverlay);
    lv_obj_set_style_bg_color(loadingOverlay, OVERLAY_BG, 0);
    lv_obj_set_style_bg_opa(loadingOverlay, LV_OPA_80, 0);
    lv_obj_add_flag(loadingOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(loadingOverlay, LV_OBJ_FLAG_CLICKABLE);
    
    // Loading spinner (using arc as spinner in LVGL 9)
    loadingSpinner = lv_arc_create(loadingOverlay);
    lv_obj_set_size(loadingSpinner, 40, 40);
    lv_obj_center(loadingSpinner);
    lv_obj_set_style_arc_color(loadingSpinner, SPINNER_COLOR, LV_PART_INDICATOR);
    
    // Loading label
    loadingLabel = lv_label_create(loadingOverlay);
    lv_obj_align(loadingLabel, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_font(loadingLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(loadingLabel, lv_color_white(), 0);
    
    // Create message overlay (hidden by default)
    messageOverlay = lv_obj_create(root);
    lv_obj_set_size(messageOverlay, 300, 200);
    lv_obj_center(messageOverlay);
    lv_obj_set_style_bg_color(messageOverlay, lv_color_hex(0x282828), 0);
    lv_obj_set_style_bg_opa(messageOverlay, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(messageOverlay, 16, 0);
    lv_obj_set_style_border_width(messageOverlay, 0, 0);
    lv_obj_add_flag(messageOverlay, LV_OBJ_FLAG_HIDDEN);
    
    // Message title
    messageTitle = lv_label_create(messageOverlay);
    lv_obj_align(messageTitle, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(messageTitle, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(messageTitle, lv_color_white(), 0);
    
    // Message body
    messageBody = lv_label_create(messageOverlay);
    lv_obj_set_width(messageBody, 260);
    lv_obj_align(messageBody, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(messageBody, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(messageBody, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_long_mode(messageBody, LV_LABEL_LONG_WRAP);
    
    // Confirm button
    messageConfirmBtn = lv_button_create(messageOverlay);
    lv_obj_set_size(messageConfirmBtn, 120, 40);
    lv_obj_align(messageConfirmBtn, LV_ALIGN_BOTTOM_RIGHT, -10, -15);
    lv_obj_set_style_bg_color(messageConfirmBtn, lv_color_hex(0x1DB954), 0);
    lv_obj_set_style_bg_opa(messageConfirmBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(messageConfirmBtn, 8, 0);
    
    lv_obj_t* confirmLabel = lv_label_create(messageConfirmBtn);
    lv_obj_center(confirmLabel);
    lv_obj_set_style_text_font(confirmLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(confirmLabel, lv_color_white(), 0);
    lv_label_set_text_static(confirmLabel, "OK");
    
    // Cancel button
    messageCancelBtn = lv_button_create(messageOverlay);
    lv_obj_set_size(messageCancelBtn, 120, 40);
    lv_obj_align(messageCancelBtn, LV_ALIGN_BOTTOM_LEFT, 10, -15);
    lv_obj_set_style_bg_color(messageCancelBtn, lv_color_hex(0x404040), 0);
    lv_obj_set_style_bg_opa(messageCancelBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(messageCancelBtn, 8, 0);
    lv_obj_add_flag(messageCancelBtn, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_t* cancelLabel = lv_label_create(messageCancelBtn);
    lv_obj_center(cancelLabel);
    lv_obj_set_style_text_font(cancelLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(cancelLabel, lv_color_white(), 0);
    lv_label_set_text_static(cancelLabel, "Cancel");
    
    Serial.println("✅ MenuManager initialized");
}

void MenuManager::navigateTo(ScreenType screenType, void* contextData) {
    // Add current screen to history
    if (currentScreen != ScreenType::SETTINGS_MAIN) {
        historyStack.push_back(NavigationEntry(currentScreen, nullptr));
    }
    
    // Navigate to new screen
    showScreen(screenType, contextData);
}

void MenuManager::goBack() {
    if (historyStack.empty()) {
        // Go to home if no history
        goToHome();
        return;
    }
    
    // Get previous screen
    NavigationEntry entry = historyStack.back();
    historyStack.pop_back();
    
    // Navigate back
    showScreen(entry.screenType, entry.contextData);
}

void MenuManager::goToHome() {
    historyStack.clear();
    showScreen(ScreenType::SETTINGS_MAIN, nullptr);
}

void MenuManager::clearHistory() {
    historyStack.clear();
}

void MenuManager::clearHistoryAndFree(bool freeContext) {
    if (freeContext) {
        for (auto& entry : historyStack) {
            if (entry.contextData != nullptr) {
                free(entry.contextData);
                entry.contextData = nullptr;
            }
        }
    }
    historyStack.clear();
}

void MenuManager::clearDialogHandlers() {
    // Remove all existing event handlers from dialog buttons
    // This prevents memory leaks from accumulating callbacks

    if (messageConfirmBtn) {
        lv_obj_remove_event_cb(messageConfirmBtn, nullptr);
        // lv_obj_remove_event_cb with nullptr removes ALL callbacks
    }

    if (messageCancelBtn) {
        lv_obj_remove_event_cb(messageCancelBtn, nullptr);
        // lv_obj_remove_event_cb with nullptr removes ALL callbacks
    }
}

void MenuManager::hideAllScreens() {
    for (auto& pair : screens) {
        if (pair.second) {
            lv_obj_t* screenObj = static_cast<lv_obj_t*>(pair.second);
            lv_obj_add_flag(screenObj, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void MenuManager::showScreen(ScreenType screenType, void* contextData) {
    // Call transition callback
    onScreenTransition(currentScreen, screenType);
    
    // Hide all screens
    hideAllScreens();
    
    // Show target screen
    auto it = screens.find(screenType);
    if (it != screens.end() && it->second) {
        lv_obj_t* screenObj = static_cast<lv_obj_t*>(it->second);
        lv_obj_clear_flag(screenObj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(screenObj);
    }
    
    currentScreen = screenType;
    
    Serial.printf("📱 Navigated to screen %d\n", static_cast<int>(screenType));
}

void MenuManager::onScreenTransition(ScreenType from, ScreenType to) {
    // Override for custom transition effects
}

void MenuManager::showLoading(const String& message) {
    if (!loadingOverlay) return;
    
    if (!message.isEmpty()) {
        lv_label_set_text(loadingLabel, message.c_str());
        lv_obj_clear_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(loadingLabel, LV_OBJ_FLAG_HIDDEN);
    }
    
    lv_obj_clear_flag(loadingOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(loadingOverlay);
}

void MenuManager::hideLoading() {
    if (loadingOverlay) {
        lv_obj_add_flag(loadingOverlay, LV_OBJ_FLAG_HIDDEN);
    }
}

void MenuManager::showSuccess(const String& message) {
    // FIX: Clear existing event handlers to prevent memory leaks
    clearDialogHandlers();

    lv_obj_set_style_text_color(messageTitle, lv_color_hex(0x1DB954), 0);
    lv_label_set_text_static(messageTitle, "Success");
    lv_label_set_text(messageBody, message.c_str());
    lv_obj_add_flag(messageCancelBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(messageConfirmBtn, 260, 40);
    lv_obj_center(messageConfirmBtn);
    lv_obj_align(messageConfirmBtn, LV_ALIGN_BOTTOM_MID, 0, -15);

    lv_obj_clear_flag(messageOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(messageOverlay);
}

void MenuManager::showError(const String& message) {
    // FIX: Clear existing event handlers to prevent memory leaks
    clearDialogHandlers();

    lv_obj_set_style_text_color(messageTitle, lv_color_hex(0xE91E63), 0);
    lv_label_set_text_static(messageTitle, "Error");
    lv_label_set_text(messageBody, message.c_str());
    lv_obj_add_flag(messageCancelBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(messageConfirmBtn, 260, 40);
    lv_obj_center(messageConfirmBtn);
    lv_obj_align(messageConfirmBtn, LV_ALIGN_BOTTOM_MID, 0, -15);

    lv_obj_clear_flag(messageOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(messageOverlay);
}

void MenuManager::showConfirm(const String& title, const String& message,
                              std::function<void()> onConfirm,
                              std::function<void()> onCancel) {
    // FIX: Clear existing event handlers before adding new ones to prevent memory leaks
    clearDialogHandlers();

    lv_obj_set_style_text_color(messageTitle, lv_color_white(), 0);
    lv_label_set_text(messageTitle, title.c_str());
    lv_label_set_text(messageBody, message.c_str());

    // Show both buttons
    lv_obj_clear_flag(messageCancelBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(messageConfirmBtn, 120, 40);
    lv_obj_align(messageConfirmBtn, LV_ALIGN_BOTTOM_RIGHT, -10, -15);

    // Store callbacks
    confirmCallback = onConfirm;
    cancelCallback = onCancel;

    // Set up event handlers with correct LVGL callback signature
    lv_obj_add_event_cb(messageConfirmBtn, [](lv_event_t* e) {
        MenuManager* mm = static_cast<MenuManager*>(lv_event_get_user_data(e));
        if (mm) {
            lv_obj_add_flag(mm->messageOverlay, LV_OBJ_FLAG_HIDDEN);
            if (mm->confirmCallback) {
                mm->confirmCallback();
            }
        }
    }, LV_EVENT_CLICKED, this);

    lv_obj_add_event_cb(messageCancelBtn, [](lv_event_t* e) {
        MenuManager* mm = static_cast<MenuManager*>(lv_event_get_user_data(e));
        if (mm) {
            lv_obj_add_flag(mm->messageOverlay, LV_OBJ_FLAG_HIDDEN);
            if (mm->cancelCallback) {
                mm->cancelCallback();
            }
        }
    }, LV_EVENT_CLICKED, this);

    lv_obj_clear_flag(messageOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(messageOverlay);
}
