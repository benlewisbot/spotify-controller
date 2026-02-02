# Troubleshooting Guide - Spotify Controller ESP32

**Version:** 1.0.0
**Date:** 2026-02-02

---

## Quick Reference

| Problem | Quick Fix |
|---------|-----------|
| Display blank | Check wiring, power, and pins |
| WiFi won't connect | Check SSID/password, use 2.4GHz |
| Spotify auth fails | Check Client ID/Secret, redirect URI |
| Touch not working | Check touch controller pins |
| UI frozen | Restart device, check serial log |
| Token expired | Wait for auto-refresh (1 hour) |
| Music won't play | Check Spotify Premium, device selection |

---

## Display Issues

### Display Shows Nothing (Blank Screen)

**Symptoms:**
- Display is completely blank
- No backlight
- No UI elements visible

**Possible Causes:**
1. Incorrect wiring
2. Insufficient power
3. Wrong display type selected
4. LVGL not initialized

**Solutions:**

1. **Check wiring:**
   - Verify SPI pins match `include/config.h`
   - Check connections: CS, DC, RST, MOSI, MISO, SCLK
   - Ensure all pins are securely connected
   - Check for loose wires or cold solder joints

2. **Check power:**
   - Ensure 3.3V supply is stable
   - Measure voltage at display VCC (should be 3.3V ±5%)
   - USB-C cable should support at least 500mA
   - Try powered USB hub

3. **Check display type:**
   ```cpp
   // include/config.h
   #define DISPLAY_TYPE ILI9488_S3  // Verify this matches your display
   ```

4. **Check initialization:**
   ```bash
   pio device monitor
   ```
   Look for:
   ```
   ✅ Display initialized
   🖥️ Display: [Display Name]
   ```

**If still blank:**
- Try another display
- Check for display damage
- Test with known-good firmware

---

### Display Shows Garbage/Noise

**Symptoms:**
- Display shows random pixels
- Text is unreadable
- Colors are wrong

**Possible Causes:**
1. Incorrect SPI frequency
2. Wrong display driver
3. Pin misconfiguration
4. Display damaged

**Solutions:**

1. **Lower SPI frequency:**
   ```cpp
   // include/config.h
   #define SPI_FREQUENCY 16000000  // Try 16MHz instead of 40MHz
   ```

2. **Verify display driver:**
   Check platformio.ini for correct TFT library:
   ```ini
   lib_deps =
       bodmer/TFT_eSPI@^2.5.0
   ```

3. **Check pin mapping in User_Setup.h:**
   Verify TFT_MISO, TFT_MOSI, TFT_SCLK, TFT_CS, TFT_DC, TFT_RST

---

### Touch Not Working

**Symptoms:**
- Display shows UI but touch doesn't respond
- Buttons don't register presses
- No touch feedback

**Possible Causes:**
1. Touch controller not initialized
2. Wrong touch pins
3. Touch controller type mismatch
4. Touch sensor damaged

**Solutions:**

1. **Check touch controller type:**
   ```cpp
   // include/config.h
   #define TOUCH_CONTROLLER FT6236  // Verify this matches your hardware
   ```

2. **Check touch pins:**
   - For FT6236 (capacitive): Check TOUCH_INT, TOUCH_SDA, TOUCH_SCL
   - For XPT2046 (resistive): Check TOUCH_IRQ, TOUCH_CS

3. **Check initialization:**
   ```bash
   pio device monitor
   ```
   Look for:
   ```
   ✅ Touch initialized
   ```

4. **Calibrate touch (if applicable):**
   Some touch controllers need calibration - check display documentation

**If still not working:**
- Test touch pins with multimeter
- Check for I2C pullup resistors (FT6236)
- Try different touch controller library

---

## WiFi Issues

### WiFi Won't Connect

**Symptoms:**
- Serial shows "Connecting..." indefinitely
- No IP address assigned
- Authentication errors

**Possible Causes:**
1. Incorrect SSID or password
2. WiFi band mismatch (5GHz vs 2.4GHz)
3. Router security settings
4. Weak signal

**Solutions:**

1. **Check credentials in `data/config.json`:**
   ```json
   {
     "wifi": {
       "ssid": "YourWiFiSSID",
       "password": "YourWiFiPassword"
     }
   }
   ```

2. **Use 2.4GHz WiFi only:**
   - ESP32 doesn't support 5GHz
   - Check router settings
   - Ensure dual-band router broadcasts both bands

3. **Check WiFi security:**
   - Supported: WPA2-PSK, WPA3-PSK (depends on ESP-IDF version)
   - Not supported: WEP, WPA (deprecated)
   - Enterprise auth: Not currently supported

4. **Check signal strength:**
   ```bash
   pio device monitor
   ```
   Look for:
   ```
   📶 WiFi got IP: 192.168.1.100
      Signal strength: -50 dBm
   ```
   - Signal should be > -70 dBm
   - Move closer to router if weak

5. **Restart WiFi:**
   - Power cycle device
   - Wait 30 seconds
   - Watch for auto-reconnect

---

### WiFi Intermittent Disconnections

**Symptoms:**
- WiFi connects but disconnects frequently
- IP address changes
- API requests timeout

**Possible Causes:**
1. Weak signal
2. Router power saving
3. WiFi channel congestion
4. ESP32 WiFi power settings

**Solutions:**

1. **Improve signal:**
   - Move device closer to router
   - Use WiFi extender
   - Check for interference (microwave, Bluetooth)

2. **Check router settings:**
   - Disable AP isolation
   - Enable DHCP
   - Check MAC filtering (if enabled)

3. **Adjust WiFi power:**
   ```cpp
   // In WiFiManager.cpp (advanced)
   WiFi.setTxPower(WIFI_POWER_19_5dBm);
   ```

4. **Enable WiFi keep-alive:**
   ```cpp
   // In WiFiManager.cpp
   WiFi.setSleep(false);  // Disable power saving
   ```

---

## Authentication Issues

### Spotify Authentication Fails

**Symptoms:**
- OAuth2 flow completes but shows error
- Browser shows "Authentication failed"
- No access token received

**Possible Causes:**
1. Invalid Client ID or Secret
2. Incorrect redirect URI
3. Incorrect scopes
4. Spotify Premium not active

**Solutions:**

1. **Verify Client ID and Secret:**
   - Log in to [Spotify Developer Dashboard](https://developer.spotify.com/dashboard)
   - Copy Client ID exactly (case-sensitive)
   - Copy Client Secret exactly

2. **Check redirect URI:**
   In Spotify Dashboard, set:
   ```
   http://<device-ip>:8080/callback
   ```
   Or for local development:
   ```
   http://localhost:8080/callback
   ```

3. **Check scopes in SpotifyClient.hpp:**
   ```cpp
   #define SPOTIFY_SCOPES \
       "user-read-playback-state " \
       "user-modify-playback-state " \
       "user-read-currently-playing " \
       // ... etc
   ```

4. **Verify Spotify Premium:**
   - Check account status at spotify.com/account
   - Premium required for playback control
   - Free accounts only support track info (read-only)

---

### Token Expired Errors

**Symptoms:**
- API calls fail with 401 error
- Controls don't work
- Serial shows "Token expired"

**Possible Causes:**
1. Token refresh failed
2. millis() overflow (very rare now - fixed in v1.1)
3. Refresh token invalid

**Solutions:**

1. **Wait for auto-refresh:**
   - Tokens expire after 1 hour
   - Device should refresh automatically
   - If not, check refresh token

2. **Check refresh token:**
   ```bash
   pio device monitor
   ```
   Look for:
   ```
   🔄 Token expired, refreshing...
   ✅ Token refreshed
   ```
   If error appears, re-authenticate

3. **Re-authenticate:**
   - Delete `data/config.json` tokens section
   - Reboot device
   - Complete OAuth2 flow again

4. **Check token expiry time:**
   - Ensure device time is reasonable
   - millis() overflow handled in v1.1+

---

## API Issues

### API Rate Limit Errors (429)

**Symptoms:**
- Serial shows HTTP 429 error
- Controls stop working temporarily
- "Too many requests" error

**Possible Causes:**
1. Too many rapid API calls
2. Bug in code causing infinite loops

**Solutions:**

1. **Wait for rate limit reset:**
   - Spotify rate limits reset after ~15-30 seconds
   - Controls should work again after waiting

2. **Check rate limiting:**
   Rate limiting was added in v1.1:
   ```cpp
   // SpotifyClient.cpp
   static constexpr unsigned long MIN_REQUEST_INTERVAL_MS = 100;
   ```

3. **Reduce polling frequency:**
   ```cpp
   // include/config.h
   #define SPOTIFY_API_POLL_MS 5000  // Increase from 3000 to 5000
   ```

---

### No Active Device Error

**Symptoms:**
- API returns "No active device found"
- Controls don't affect playback
- Nothing plays

**Possible Causes:**
1. Spotify app not open on any device
2. Active device changed to another device
3. Device selection not implemented yet

**Solutions:**

1. **Open Spotify app:**
   - Open Spotify on phone, desktop, or web player
   - Start playing music
   - Controls should work now

2. **Check active device:**
   - Use Spotify desktop app to check "Connect to a device"
   - Ensure ESP32 is listed as available device

3. **Set device programmatically (if implemented):**
   ```cpp
   spotifyClient->setDevice(deviceId);
   ```

---

## UI Issues

### UI Frozen / Unresponsive

**Symptoms:**
- UI doesn't respond to touch
- Progress bar doesn't update
- Buttons don't work

**Possible Causes:**
1. Main loop blocked
2. Out of memory
3. LVGL not updating
4. Stack overflow

**Solutions:**

1. **Check serial monitor:**
   ```bash
   pio device monitor
   ```
   Look for errors or hanging messages

2. **Check free memory:**
   ```cpp
   Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
   ```
   Should be > 100KB

3. **Ensure LVGL task runs:**
   ```cpp
   // In App.cpp
   if (windowManager) {
       windowManager->update();  // This calls lv_timer_handler()
   }
   ```

4. **Reduce UI complexity:**
   - Reduce number of widgets
   - Reduce animation count
   - Reduce font sizes

---

### UI Layout Broken / Overlapping Elements

**Symptoms:**
- Elements overlap each other
- Text truncated incorrectly
- Elements off-screen

**Possible Causes:**
1. Wrong display size
2. Incorrect UI constants
3. Font size too large
4. Bug in positioning code

**Solutions:**

1. **Verify display size:**
   ```cpp
   // include/config.h
   #define DISPLAY_WIDTH 320
   #define DISPLAY_HEIGHT 480
   ```

2. **Check UI constants:**
   ```cpp
   // include/config.h
   #define UI_MARGIN 16
   #define UI_ALBUM_ART_SIZE 220
   ```

3. **Adjust for different displays:**
   - Portrait vs landscape mode
   - Different resolutions
   - May need custom layout code

---

## Performance Issues

### Slow Response Time

**Symptoms:**
- Touch delay > 1 second
- API requests take long time
- UI lags

**Possible Causes:**
1. WiFi latency
2. Spotify API slowness
3. High CPU usage
4. Network congestion

**Solutions:**

1. **Measure WiFi latency:**
   ```bash
   ping api.spotify.com
   ```
   Should be < 100ms

2. **Check CPU usage:**
   ```cpp
   Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
   Serial.printf("Min free heap: %d\n", ESP.getMinFreeHeap());
   ```

3. **Reduce API polling:**
   ```cpp
   // include/config.h
   #define SPOTIFY_API_POLL_MS 5000  // Increase interval
   ```

4. **Optimize LVGL:**
   ```cpp
   // src/config/lv_conf.h
   #define LV_USE_REFR 1
   #define LV_REFR_PERIOD 30  // Reduce refresh rate
   ```

---

### Memory Leaks

**Symptoms:**
- Device crashes after hours of use
- Free memory decreases over time
- Random crashes

**Possible Causes:**
1. LVGL objects not deleted
2. HTTP clients not cleaned up
3. Strings not freed
4. Malloc without free

**Solutions:**

1. **Check for memory leaks:**
   ```cpp
   uint32_t initialHeap = ESP.getFreeHeap();
   // ... run for 1 hour ...
   uint32_t finalHeap = ESP.getFreeHeap();
   Serial.printf("Memory leak: %d bytes\n", initialHeap - finalHeap);
   ```

2. **Ensure proper cleanup:**
   ```cpp
   // In destructors
   if (object) {
       lv_obj_del(object);
       object = nullptr;
   }
   ```

3. **Use String carefully:**
   ```cpp
   // Bad (creates temporary String)
   String result = String(num) + " bytes";

   // Good (reuse buffer)
   char buffer[32];
   snprintf(buffer, sizeof(buffer), "%d bytes", num);
   ```

4. **Enable PSRAM:**
   ```ini
   # platformio.ini
   board_build.arduino.memory_type = qio_opi
   ```
   or select board with PSRAM in `include/config.h`

---

## Hardware Issues

### Device Won't Boot

**Symptoms:**
- No serial output
- Device shows as offline
- LED not blinking

**Possible Causes:**
1. Boot mode not correct
2. Flash corrupted
3. Power supply issue
4. USB driver issue

**Solutions:**

1. **Check boot mode:**
   - GPIO0 must be HIGH on boot
   - GPIO2 must be LOW on boot
   - EN (enable) pin must be HIGH

2. **Reflash firmware:**
   ```bash
   pio run -t upload --target erase
   pio run -t upload
   ```

3. **Check power supply:**
   - USB-C must support data (not power-only)
   - Voltage should be 5V ±5%
   - Current > 500mA

4. **Try different USB port/cable:**
   - Use USB 2.0 port (not 3.0)
   - Try different cable
   - Try powered USB hub

---

### ESP32 Reboots Randomly

**Symptoms:**
- Device reboots unexpectedly
- Brownout detector triggered
- Watchdog timer reset

**Possible Causes:**
1. Power supply insufficient
2. Stack overflow
3. Exception in code
4. Watchdog timer starvation

**Solutions:**

1. **Check power supply:**
   - Use USB-C cable with 5V, 1A+ capability
   - Measure VCC (should be 3.3V ±5%)
   - Try powered USB hub

2. **Check exception decoder:**
   ```bash
   pio device monitor
   ```
   Look for:
   ```
   Guru Meditation Error: Core 1 panic'ed (LoadProhibited)
   ```
   Use [ESP32 Exception Decoder](https://github.com/me-no-dev/ESP32ExceptionDecoder)

3. **Increase stack size:**
   ```cpp
   xTaskCreatePinnedToCore(task, "Task", 8192, NULL, 1, NULL, 0);
   // Increase from 4096 to 8192
   ```

4. **Feed watchdog:**
   ```cpp
   // In long-running loops
   esp_task_wdt_reset();
   ```

---

## Getting Help

### Before Asking for Help

1. **Search existing issues:**
   - Check GitHub issues
   - Search problem description

2. **Gather information:**
   ```bash
   # Board info
   pio device monitor
   # Firmware version
   git log -1
   # Config
   cat include/config.h
   ```

3. **Collect error logs:**
   - Full serial output
   - Stack traces (if crash)
   - Screenshot of display (if possible)

### When to Report an Issue

- Device crashes consistently
- Features don't work as documented
- Documentation is unclear
- Security vulnerability found

### Where to Report

- GitHub Issues: [github.com/BenLewisbot/spotify-controller-esp32/issues]
- Discord Community: (link if available)
- Email: (if available)

---

## FAQ

### Q: Does this work with Spotify Free?
A: No, Spotify Premium is required for playback control. Free accounts can only display current track info (read-only).

### Q: Can I use a different display?
A: Yes! Edit `include/config.h` and set `DISPLAY_TYPE` to match your display. See `README.md` for supported displays.

### Q: Does this work on ESP8266?
A: No, ESP8266 doesn't have enough RAM for LVGL. ESP32 with PSRAM is required.

### Q: Can I control multiple Spotify accounts?
A: No, only one account can be authenticated at a time. To switch, delete tokens from `data/config.json` and re-authenticate.

### Q: How often does it poll Spotify?
A: By default, every 3 seconds. This can be adjusted in `include/config.h` with `SPOTIFY_API_POLL_MS`.

### Q: Does this support offline playback?
A: No, Spotify doesn't allow offline playback on third-party devices.

---

**Last Updated:** 2026-02-02
**Maintainer:** Spotify GUI Agent
