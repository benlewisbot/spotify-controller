# Manual Test Plan - Spotify Controller ESP32

**Version:** 1.0.0
**Date:** 2026-02-02
**Tester:** _________________
**Device:** ESP32-S3 + Display

---

## Test Environment

### Hardware
- [ ] ESP32-S3 board (e.g., LilyGo T-Display S3 Touch)
- [ ] Display (ILI9488 / ST7789 / etc.)
- [ ] USB-C cable
- [ ] Stable WiFi (2.4GHz)
- [ ] Spotify Premium account

### Software
- [ ] Arduino IDE or PlatformIO installed
- [ ] Latest firmware flashed
- [ ] Serial monitor ready (115200 baud)
- [ ] Spotify Developer account with app configured

---

## Pre-Flight Checks

### Configuration
- [ ] WiFi SSID and password in `data/config.json`
- [ ] Spotify Client ID and Secret in `data/config.json`
- [ ] Correct display type in `include/config.h`
- [ ] `data/config.json` uploaded to LittleFS

### Network
- [ ] Device connects to WiFi
- [ ] Device receives IP address
- [ ] DNS resolution working
- [ ] HTTPS connections successful (port 443)

---

## Test Suite 1: Basic Functionality

### 1.1 Device Boot
**Test:** Power on device
**Expected:**
- Serial output shows initialization sequence
- Display turns on
- UI renders correctly
- No error messages

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 1.2 WiFi Connection
**Test:** Connect to configured WiFi
**Expected:**
- Serial shows "Connecting to WiFi: [SSID]"
- Serial shows "WiFi got IP: [IP]"
- Connection is stable (no disconnects in 5 minutes)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 1.3 Spotify Authentication
**Test:** Complete OAuth2 flow
**Steps:**
1. Power on device
2. Note device IP from serial monitor
3. Open browser to `http://[device-ip]:8080`
4. Click "Connect Spotify"
5. Authorize app
6. Return to device

**Expected:**
- Browser shows "Successfully connected!"
- Serial shows "✅ Authentication successful!"
- Display shows Now Playing screen
- Tokens saved to LittleFS

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 2: UI Rendering

### 2.1 Display Layout
**Test:** Verify all UI elements visible
**Expected:**
- Album art placeholder visible (left side)
- Track title visible (top right)
- Artist name visible (below title)
- Progress bar visible (bottom)
- Time labels visible (0:00 / 3:05)
- Play/Pause button visible (center bottom)
- Previous/Next buttons visible (left/right bottom)
- Volume slider visible (right side)
- Save (heart) button visible (top right)
- Menu button visible (below save button)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 2.2 UI Colors
**Test:** Verify color scheme matches Spotify design
**Expected:**
- Background is dark (#121212)
- Play button is green (#1DB954)
- Text is white (primary) and gray (secondary)
- Progress bar is white
- Volume slider uses green accent

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 2.3 Typography
**Test:** Verify text is readable and properly formatted
**Expected:**
- Track title: Large, bold font
- Artist name: Medium, lighter font
- Time labels: Small font
- All text visible against dark background
- Long text truncated with ellipsis (...)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 3: Playback Controls

### 3.1 Play/Pause Toggle
**Test:** Touch play/pause button
**Steps:**
1. Start playing music on Spotify (phone or desktop)
2. Touch the play/pause button on display
3. Verify playback state changes
4. Touch again to toggle back

**Expected:**
- Icon changes from ▶ to ⏸ (and vice versa)
- Spotify app reflects the change
- Music plays/pauses immediately
- No delay (> 1 second)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 3.2 Next Track
**Test:** Touch next button (>>)
**Steps:**
1. Play a track
2. Touch next button
3. Verify next track starts

**Expected:**
- Next track starts playing
- Display updates with new track info
- Progress bar resets to 0:00
- No delay (> 1 second)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 3.3 Previous Track
**Test:** Touch previous button (<<)
**Steps:**
1. Play a track (more than 3 seconds in)
2. Touch previous button
3. Verify previous track starts

**Expected:**
- Previous track starts playing
- Display updates with new track info
- Progress bar resets to 0:00
- No delay (> 1 second)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 3.4 Previous Track < 3s
**Test:** Touch previous button within 3 seconds
**Steps:**
1. Play a track (less than 3 seconds in)
2. Touch previous button
3. Verify track restarts

**Expected:**
- Current track restarts from beginning
- Display shows same track
- Progress bar resets to 0:00

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 4: Volume Control

### 4.1 Volume Slider
**Test:** Touch and drag volume slider
**Steps:**
1. Touch volume slider
2. Drag up to increase volume
3. Drag down to decrease volume
4. Release at desired level

**Expected:**
- Volume changes smoothly
- Slider knob follows finger
- Volume icon visible above slider
- Spotify app reflects volume change

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 4.2 Volume Range
**Test:** Set volume to minimum and maximum
**Steps:**
1. Drag slider to bottom (0%)
2. Verify volume is muted
3. Drag slider to top (100%)
4. Verify volume is maximum

**Expected:**
- Volume works at 0% (muted)
- Volume works at 100% (max)
- No audio distortion at maximum
- Smooth volume curve

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 5: Progress Bar

### 5.1 Progress Updates
**Test:** Monitor progress bar during playback
**Expected:**
- Progress bar updates in real-time
- Movement is smooth (not jerky)
- Updates every ~3 seconds (SPOTIFY_POLL_INTERVAL_MS)
- Left time label updates correctly
- Right time label shows total duration

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 5.2 Time Formatting
**Test:** Verify time labels display correctly
**Test Cases:**
- 0:00 to 0:59 (seconds)
- 1:00 to 9:59 (single-digit minutes)
- 10:00 to 59:59 (double-digit minutes)

**Expected:**
- Times formatted as M:SS
- Leading zero for seconds < 10
- No overflow or wrapping

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 6: Track Information

### 6.1 Track Info Updates
**Test:** Verify track info updates when track changes
**Steps:**
1. Let current track finish
2. Wait for auto-advance to next track
3. Verify display updates

**Expected:**
- Track title updates to new track
- Artist name updates to new artist
- Album art placeholder updates (different color)
- Progress bar resets
- Play button shows pause icon (⏸)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 6.2 Long Track Titles
**Test:** Play track with very long title
**Expected:**
- Title truncated with ellipsis (...)
- Text doesn't overlap other elements
- UI layout remains stable

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 7: Error Handling

### 7.1 WiFi Disconnect
**Test:** Simulate WiFi disconnection
**Steps:**
1. While device is playing, disable WiFi on router
2. Wait 30 seconds
3. Re-enable WiFi

**Expected:**
- Serial shows "WiFi disconnected"
- Device attempts to reconnect automatically
- Reconnects when WiFi is available
- No crash or freeze

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 7.2 Token Expiry
**Test:** Wait for access token to expire
**Steps:**
1. Note current time
2. Wait 1 hour (or modify token expiry to 5 minutes for testing)
3. Trigger a playback control

**Expected:**
- Token is refreshed automatically
- Control still works after expiry
- Serial shows "Token refreshed"
- No error messages

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 7.3 Spotify Service Down
**Test:** Make API request while Spotify is unavailable
**Steps:**
1. Block Spotify API access (firewall)
2. Try to change track
3. Unblock Spotify API

**Expected:**
- Error message in serial log
- UI remains responsive
- Device doesn't crash
- Retries automatically when available

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 8: Edge Cases

### 8.1 Rapid Button Presses
**Test:** Press buttons rapidly in succession
**Steps:**
1. Press play/pause 10 times quickly
2. Press next/prev alternately
3. Observe behavior

**Expected:**
- No crashes
- Commands processed in order
- No API rate limit errors
- UI remains responsive

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 8.2 Very Long Uptime
**Test:** Run device for 24+ hours
**Expected:**
- No memory leaks
- millis() overflow handled correctly
- Token refresh works after 49 days
- WiFi stays connected
- No performance degradation

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 8.3 Multiple Devices
**Test:** Use multiple Spotify devices simultaneously
**Steps:**
1. Play music on phone
2. Control from ESP32 device
3. Play from desktop app
4. Try to control from ESP32 again

**Expected:**
- ESP32 can control when active device
- If another device becomes active, ESP32 may show error
- User can switch active device from UI (if implemented)

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 9: Performance

### 9.1 Touch Response Time
**Test:** Measure time from touch to response
**Method:** Use stopwatch or serial timestamps
**Expected:**
- Response time < 100ms
- UI feels responsive
- No lag or stuttering

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 9.2 API Latency
**Test:** Measure time from command to playback change
**Method:** Check serial timestamps
**Expected:**
- API response time < 500ms
- Playback changes smoothly
- No visible lag

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 9.3 Battery Consumption (if portable)
**Test:** Monitor battery usage over time
**Method:** Measure current draw
**Expected:**
- Idle: < 50mA
- Active (playback): < 200mA
- Sleep mode (if implemented): < 10mA

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Test Suite 10: Wokwi Demo

### 10.1 Demo Loads Correctly
**Test:** Open Wokwi demo in browser
**Steps:**
1. Visit https://wokwi.com/projects/new/esp32
2. Paste code from `wokwi/sketch.ino`
3. Press Start

**Expected:**
- Simulation starts
- Serial output shows initialization
- Display renders correctly
- No errors

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

### 10.2 Serial Commands Work
**Test:** Control demo via serial commands
**Commands to test:**
- `play` - Toggle play/pause
- `next` - Next track
- `prev` - Previous track
- `vol 50` - Set volume to 50%

**Expected:**
- Commands are recognized
- Display updates accordingly
- Demo cycles through tracks

**Result:** ✅ PASS / ❌ FAIL
**Notes:** ___________________________

---

## Summary

### Overall Results
- **Total Tests:** ____
- **Passed:** ____
- **Failed:** ____
- **Pass Rate:** ____%

### Critical Issues
1. ___________________________
2. ___________________________
3. ___________________________

### Non-Critical Issues
1. ___________________________
2. ___________________________
3. ___________________________

### Suggestions for Improvement
1. ___________________________
2. ___________________________
3. ___________________________

---

## Sign-Off

**Tester Name:** _________________
**Date:** _________________
**Tested on:** ESP32-S3 + ILI9488 Display
**Firmware Version:** 1.0.0
**Overall Status:** ✅ APPROVED / ❌ NEEDS REVISION / ⚠️ APPROVED WITH ISSUES

---

**Last Updated:** 2026-02-02
**Maintainer:** Spotify GUI Agent
