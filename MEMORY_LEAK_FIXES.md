# Memory Leak Fixes - Batch C

## Summary

Fixed memory leaks and overflow bugs in HTTP handling, MenuManager, and Token management.

## Issues Fixed

### 1. HTTP Memory Leaks (SpotifyClient.cpp)

**Problem:** Missing implementations of `httpGetEx()`, `httpPutEx()`, `httpPostEx()`, `httpDeleteEx()` methods that were declared but never implemented.

**Solution:** Added complete implementations for all four `*Ex` HTTP methods with proper error handling and `http.end()` cleanup:
- `httpGetEx()` - line 897
- `httpPutEx()` - line 924
- `httpPostEx()` - line 951
- `httpDeleteEx()` - line 978

### 2. Token Overflow Bugs (SpotifyClient.cpp, AuthManager.cpp)

**Problem:** Token expiry calculation using `millis() + expiresIn * 1000` will overflow after ~49 days when `millis()` wraps around.

**Solution:** 
- Added `tokenExpiryTime` member variable to `SpotifyClient.hpp` for backwards compatibility
- Updated `refreshTokenIfNeeded()` to use overflow-safe tracking:
  ```cpp
  tokenAcquiredAt = millis();
  tokenValidForMs = 3600000UL;
  ```
- Updated `AuthManager.cpp`:
  - `exchangeCodeForTokens()` - uses `tokenAcquiredAt` and `tokenValidForMs`
  - `refreshAccessToken()` - uses overflow-safe tracking
  - `isTokenExpired()` - already implemented with overflow-safe logic

### 3. MenuManager Event Handler Memory Leaks (MenuManager.cpp)

**Problem:** Every call to `showConfirm()`, `showSuccess()`, or `showError()` added new event callbacks to dialog buttons using `lv_obj_add_event_cb()`. These callbacks were never removed, causing memory to accumulate over time.

**Solution:**
- Added `clearDialogHandlers()` method that removes all existing event handlers from dialog buttons using `lv_obj_remove_event_cb()`
- Added `clearHistoryAndFree()` method for cleaning up navigation history with optional context data freeing
- Updated `showConfirm()`, `showSuccess()`, and `showError()` to call `clearDialogHandlers()` before adding new handlers

### 4. SpotifyTokenScreen Token Overflow (SpotifyTokenScreen.cpp)

**Problem:** `formatTimeRemaining()` calculated remaining time as `expiresAt - millis()` which fails when `millis()` wraps around.

**Solution:**
- Updated `TokenInfo` struct to include `tokenAcquiredAt` and `tokenValidForMs` fields
- Added `isExpired()` and `getRemainingMs()` methods to `TokenInfo` that use overflow-safe elapsed time calculation
- Updated `formatTimeRemaining()` to use `tokenInfo.getRemainingMs()` instead of raw timestamp subtraction
- Updated `updateTokenInfo()` to use `info.isExpired()` for validity checking

## Files Modified

1. `src/spotify/SpotifyClient.hpp` - Added `tokenExpiryTime` member variable
2. `src/spotify/SpotifyClient.cpp` - Added `*Ex` HTTP methods, fixed token refresh overflow
3. `src/spotify/AuthManager.hpp` - Added `tokenExpiryTime` member variable
4. `src/spotify/AuthManager.cpp` - Fixed token overflow in `exchangeCodeForTokens()` and `refreshAccessToken()`
5. `src/ui/screens/MenuManager.hpp` - Added `clearDialogHandlers()` and `clearHistoryAndFree()` declarations
6. `src/ui/screens/MenuManager.cpp` - Implemented `clearDialogHandlers()`, updated dialog methods
7. `src/ui/screens/SpotifyTokenScreen.hpp` - Added overflow-safe fields and methods to `TokenInfo`
8. `src/ui/screens/SpotifyTokenScreen.cpp` - Fixed `formatTimeRemaining()` and `updateTokenInfo()`

## Testing Notes

- The ESP32 build environment (PlatformIO) is required to compile and test
- The overflow-safe token tracking uses elapsed time calculation `(millis() - tokenAcquiredAt)` which handles wraparound correctly due to unsigned arithmetic
- Event handler cleanup should be verified by repeatedly opening/closing dialogs and monitoring heap usage

## Technical Details

### Overflow-Safe Time Calculation

The key insight is that for unsigned integers, `(current - start)` gives the correct elapsed time even when `current` has wrapped around to a smaller value than `start`. This is because unsigned subtraction wraps around correctly:

```cpp
// Example: millis() = 100, tokenAcquiredAt = ULONG_MAX - 100
// elapsed = 100 - (ULONG_MAX - 100) = 201 (correct!)
unsigned long elapsed = millis() - tokenAcquiredAt;
bool expired = elapsed >= tokenValidForMs;
```

### LVGL Event Handler Cleanup

LVGL maintains a linked list of event handlers per object. Calling `lv_obj_remove_event_cb(obj, nullptr)` removes all handlers from the object, preventing accumulation.
