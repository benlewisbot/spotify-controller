# Known Issues - Spotify Controller ESP32

**Version:** 1.0.0
**Date:** 2026-02-02

---

## Critical Issues

### None at this time ✅

All critical issues have been resolved in v1.1.0

---

## High Priority Issues

### 1. Cover Image Display Not Implemented

**Severity:** High
**Impact:** User Experience
**Status:** Placeholder implemented

**Description:**
Album art displays as a color placeholder instead of the actual image. The image download function exists (`SpotifyClient::downloadImage()`), but there's no LVGL JPEG/PNG decoder to render the image.

**Current Workaround:**
- Shows gradient placeholder color based on album art URL hash
- Different albums have different placeholder colors

**Solution Required:**
1. Integrate LVGL image decoder (JPEG, PNG)
2. Or use external decoder library (e.g., TJpgDec)
3. Implement image caching in LittleFS

**Complexity:** High
**Estimated Time:** 4-8 hours

---

### 2. Settings Screen Not Implemented

**Severity:** High
**Impact:** User Experience
**Status:** Not started

**Description:**
Settings screen mentioned in TODO but not implemented. Users cannot configure device without editing `data/config.json` manually.

**Missing Features:**
- WiFi SSID/password configuration
- Spotify credentials management
- Display settings (brightness, rotation)
- Touch calibration
- About/Info screen

**Current Workaround:**
- Edit `data/config.json` via serial or LittleFS upload
- Reboot device for changes to take effect

**Solution Required:**
1. Create `SettingsScreen` class
2. Implement UI forms for all settings
3. Add save/load functionality
4. Add validation

**Complexity:** Medium-High
**Estimated Time:** 6-10 hours

---

## Medium Priority Issues

### 3. Playlist Browser Not Implemented

**Severity:** Medium
**Impact:** Feature Gap
**Status:** Not started

**Description:**
Users can only view the currently playing track. No way to browse playlists or select specific tracks.

**Missing Features:**
- Playlist list view
- Track list view
- Search functionality
- Play from playlist/track

**Current Workaround:**
- Use Spotify app to browse
- ESP32 only controls playback

**Solution Required:**
1. Create `PlaylistBrowser` screen
2. Implement list widget with scrolling
3. Add search screen with on-screen keyboard
4. Integrate with Spotify API endpoints

**Complexity:** High
**Estimated Time:** 10-15 hours

---

### 4. No Device Selection UI

**Severity:** Medium
**Impact:** User Experience
**Status:** Partial (API exists)

**Description:**
Users can programmatically select which Spotify device to control, but no UI to choose from available devices.

**Current Workaround:**
- Use Spotify app to switch active device
- Controls work on whatever device is active

**Solution Required:**
1. Create device selection screen
2. Fetch available devices via `getDevices()`
3. Show device list with active indicator
4. Allow user to select device

**Complexity:** Low-Medium
**Estimated Time:** 3-5 hours

---

### 5. No Screensaver

**Severity:** Medium
**Impact:** Power Consumption / Display Life
**Status:** Not implemented

**Description:**
Display is always on, even when not in use. This wastes power and could cause burn-in on OLED displays.

**Missing Features:**
- Auto-dim after inactivity
- Auto-off after long inactivity
- Wake on touch
- Configurable timeout

**Current Workaround:**
- Manually power off display
- Or accept constant-on behavior

**Solution Required:**
1. Implement inactivity timer
2. Add dim/off functions
3. Add wake-on-touch handler
4. Add timeout settings

**Complexity:** Low-Medium
**Estimated Time:** 2-4 hours

---

## Low Priority Issues

### 6. No Volume Limit Enforcement

**Severity:** Low
**Impact:** Hearing Safety
**Status:** Config exists, not enforced

**Description:**
`VOLUME_LIMIT_DEFAULT` is defined (80%), but not actually enforced. Users can set volume to 100%.

**Current Behavior:**
- Volume slider goes 0-100%
- No limit applied in UI
- No limit applied to API calls

**Solution Required:**
1. Enforce limit in `setVolume()`
2. Clamp slider maximum to limit
3. Add setting to adjust limit

**Complexity:** Low
**Estimated Time:** 1 hour

---

### 7. No Progress Bar Seek Feedback

**Severity:** Low
**Impact:** User Experience
**Status:** Implemented, no visual feedback during drag

**Description:**
Progress bar seek works (you can touch to jump to position), but there's no real-time feedback while dragging.

**Current Behavior:**
- Touch progress bar → jumps to position
- No preview while dragging
- Updates only on release

**Solution Required:**
1. Update time label during drag
2. Show scrubbing position
3. Smooth scrubbing animation

**Complexity:** Low
**Estimated Time:** 2-3 hours

---

### 8. No Track Save Functionality

**Severity:** Low
**Impact:** Feature Gap
**Status:** UI button exists, API not connected

**Description:**
Save (heart) button toggles visually, but doesn't actually save/remove track from user's library.

**Current Behavior:**
- Touch heart → toggles icon
- No API call made
- Track not saved/removed in Spotify

**Solution Required:**
1. Connect heart button to `saveTrack()`/`removeTrack()`
2. Check if track is already saved (`isTrackSaved()`)
3. Update UI based on save status

**Complexity:** Low
**Estimated Time:** 1-2 hours

---

### 9. No Swipe Gestures

**Severity:** Low
**Impact:** User Experience
**Status:** Not implemented

**Description:**
Navigation requires button presses instead of natural swipe gestures.

**Missing Gestures:**
- Swipe left/right to change screens
- Swipe up/down for volume (alternative)

**Current Workaround:**
- Use menu button (if implemented)
- Or accept button-only navigation

**Solution Required:**
1. Implement LVGL gesture recognition
2. Add swipe handlers
3. Add gesture animation

**Complexity:** Medium
**Estimated Time:** 4-6 hours

---

### 10. No Track History/Queue

**Severity:** Low
**Impact:** Feature Gap
**Status:** Not implemented

**Description:**
Users can only see the currently playing track, not upcoming tracks in queue or recently played tracks.

**Missing Features:**
- Queue view
- History view
- Skip queue functionality

**Current Workaround:**
- Use Spotify app to manage queue

**Solution Required:**
1. Create queue/history screens
2. Integrate with Spotify API (queue endpoints)
3. Implement queue management

**Complexity:** Medium
**Estimated Time:** 6-8 hours

---

## Minor Issues / Improvements

### 11. Animations Not Smooth

**Severity:** Cosmetic
**Impact:** Polish
**Status:** Can be improved

**Description:**
UI transitions could be smoother. LVGL animations exist but not fully utilized.

**Improvements:**
- Fade transitions between screens
- Smooth progress bar updates
- Button press animations
- Volume slider smooth update

**Complexity:** Low-Medium
**Estimated Time:** 2-4 hours

---

### 12. No Haptic Feedback

**Severity:** Cosmetic
**Impact:** Polish
**Status:** Not implemented

**Description:**
No vibration feedback on button presses (requires vibration motor).

**Requirements:**
- Hardware: Vibration motor
- Software: PWM output to motor

**Complexity:** Low (hardware), Medium (software)
**Estimated Time:** 2-3 hours

---

### 13. No Audio Visualization

**Severity:** Cosmetic
**Impact:** Polish
**Status:** Not implemented

**Description:**
No visual representation of audio being played (waveform, bars, etc.).

**Requirements:**
- Audio input/output access
- FFT for frequency analysis
- Real-time visualization

**Complexity:** High
**Estimated Time:** 10-15 hours

---

### 14. Limited Multi-Language Support

**Severity:** Minor
**Impact:** Accessibility
**Status:** English only

**Description:**
All text is hardcoded in English. No internationalization support.

**Solution Required:**
1. Extract all strings to translation files
2. Implement language selection
3. Load translations at runtime

**Complexity:** Medium
**Estimated Time:** 4-6 hours

---

### 15. No Accessibility Features

**Severity:** Minor
**Impact:** Accessibility
**Status:** None implemented

**Description:**
No accessibility features for visually impaired users.

**Missing Features:**
- High contrast mode
- Larger text mode
- Audio feedback
- Screen reader support (if applicable)

**Complexity:** Medium
**Estimated Time:** 3-5 hours

---

## Technical Debt

### 16. Hardcoded UI Layout

**Severity:** Technical Debt
**Impact:** Maintainability
**Status:** Improved in v1.1

**Description:**
UI layout still partially hardcoded. Not fully responsive to different display sizes.

**Current State:**
- Constants moved to config.h (good)
- But layout calculations still assume 320x480
- Not responsive to different resolutions

**Solution Required:**
1. Make layout fully percentage-based
2. Implement responsive design
3. Test on multiple display sizes

**Complexity:** Medium
**Estimated Time:** 4-6 hours

---

### 17. No Comprehensive Error Codes

**Severity:** Technical Debt
**Impact:** Debugging
**Status:** Basic error handling

**Description:**
Errors are logged to serial, but no structured error codes or error recovery system.

**Solution Required:**
1. Define error code enum
2. Implement error recovery strategies
3. Add error UI (error screen/toast)

**Complexity:** Medium
**Estimated Time:** 3-5 hours

---

### 18. Limited Unit Test Coverage

**Severity:** Technical Debt
**Impact:** Quality Assurance
**Status:** ~30% coverage

**Description:**
Only basic unit tests exist. No integration tests or end-to-end tests.

**Solution Required:**
1. Add AuthManager tests
2. Add WiFiManager tests
3. Add integration tests
4. Add E2E tests with mocking

**Complexity:** High
**Estimated Time:** 15-20 hours

---

### 19. No Logging Framework

**Severity:** Technical Debt
**Impact:** Debugging
**Status:** Basic Serial.print

**Description:**
All logging is via `Serial.printf()`. No log levels, filtering, or file logging.

**Solution Required:**
1. Implement logging framework
2. Add log levels (DEBUG, INFO, WARN, ERROR)
3. Add filtering by level
4. Optional file logging

**Complexity:** Low-Medium
**Estimated Time:** 3-4 hours

---

### 20. No Configuration Migration

**Severity:** Technical Debt
**Impact:** Upgrade Path
**Status:** Not implemented

**Description:**
No migration path when config format changes between versions.

**Solution Required:**
1. Store config version
2. Implement migration functions
3. Apply migrations on boot if needed

**Complexity:** Medium
**Estimated Time:** 4-6 hours

---

## Resolved Issues (for reference)

### ✅ millis() Overflow (RESOLVED in v1.1)

**Issue:** Token expiry check failed after ~49 days.

**Solution:** Added overflow detection in `ensureValidToken()`.

---

### ✅ Rate Limiting (RESOLVED in v1.1)

**Issue:** No protection against Spotify API rate limits.

**Solution:** Implemented `enforceRateLimit()` with 100ms minimum interval.

---

### ✅ Magic Numbers (RESOLVED in v1.1)

**Issue:** UI constants scattered throughout code.

**Solution:** Moved all UI constants to `include/config.h`.

---

### ✅ saveBtn Initialization Order (RESOLVED in v1.1)

**Issue:** Event handler attached before button created.

**Solution:** Changed initialization order in `createUI()`.

---

### ✅ Certificate Validation (RESOLVED in v1.1)

**Issue:** All HTTPS connections insecure.

**Solution:** Created `SpotifySecure` class with certificate pinning.

---

## Issue Prioritization Matrix

```
                    | Impact | Effort | Priority
--------------------|--------|--------|----------
Cover Image Display | High   | High   | HIGH
Settings Screen     | High   | Med-High| HIGH
Playlist Browser    | Medium | High   | MED
Device Selection    | Medium | Low-Med| MED
Screensaver         | Medium | Low-Med| MED
Volume Limit        | Low     | Low    | LOW
Progress Feedback   | Low     | Low    | LOW
Track Save         | Low     | Low    | LOW
Swipe Gestures     | Low     | Med    | LOW
```

---

## Contributing

If you want to fix an issue:

1. **Comment on the issue** to let others know you're working on it
2. **Create a branch:** `git checkout -b fix/issue-name`
3. **Implement the fix**
4. **Add tests** (if applicable)
5. **Update documentation**
6. **Submit a pull request**

See [CONTRIBUTING.md](../CONTRIBUTING.md) for more details.

---

**Last Updated:** 2026-02-02
**Maintainer:** Spotify GUI Agent
