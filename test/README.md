# Unit Tests - Spotify Controller ESP32

**Version:** 1.0.0
**Date:** 2026-02-02

---

## Overview

This directory contains unit tests for the Spotify Controller project. Tests are organized by component and can be run on ESP32 or in a simulation environment.

---

## Test Structure

```
test/
├── test_utils.hpp           # Test utilities and assert macros
├── test_spotify_client.cpp  # SpotifyClient unit tests
├── test_ui.cpp             # UI component unit tests
├── README.md               # This file
└── platformio.ini          # Test configuration (optional)
```

---

## Running Tests

### Option 1: PlatformIO (Recommended)

1. **Create test environment:**
```bash
cd /home/tod/clawd/projects/spotify-controller
mkdir -p test/platformio
```

2. **Create `test/platformio.ini`:**
```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    bblanchon/ArduinoJson@^6.21.0

[env:native]
platform = native
framework = arduino
```

3. **Run tests:**
```bash
pio test -e esp32
```

### Option 2: Manual Upload

1. **Build test:**
```bash
# Modify test file's setup() and upload as main program
# Tests will run automatically on boot
```

2. **Upload to device:**
```bash
pio run -t upload
```

3. **Monitor results:**
```bash
pio device monitor
```

---

## Test Suites

### 1. SpotifyClient Tests (`test_spotify_client.cpp`)

**Tests:**
- ✅ `testMillisOverflow()` - Token expiry with millis() overflow
- ✅ `testRateLimiting()` - Request rate limiting
- ✅ `testVolumeValidation()` - Volume input validation
- ✅ `testProgressCalculation()` - Progress bar calculations
- ✅ `testStringSafety()` - String operations
- ✅ `testTimeFormatting()` - Time display formatting

**What's Tested:**
- Token refresh after ~49 days (millis overflow)
- Minimum 100ms between API requests
- Volume clamping (0-100%)
- Progress percentage calculation
- Edge cases (zero duration, overflow values)

---

### 2. UI Tests (`test_ui.cpp`)

**Tests:**
- ✅ `testUIConstants()` - UI constant values
- ✅ `testColorValues()` - Color definitions
- ✅ `testDisplayDimensions()` - Display size calculations
- ✅ `testButtonPositioning()` - Button layout calculations
- ✅ `testVolumeSliderPosition()` - Slider positioning
- ✅ `testProgressBarPosition()` - Progress bar positioning
- ✅ `testLayoutSanity()` - Overall layout sanity checks

**What's Tested:**
- UI constants are valid and reasonable
- Colors are valid 24-bit RGB values
- Layout calculations for all UI elements
- Elements fit within display bounds
- No overlapping elements

---

## Writing New Tests

### Template

```cpp
#include "test_utils.hpp"

void testMyFeature() {
    Serial.println("\n🧪 Test: My Feature");

    TestResult result;

    // Arrange
    int value = 42;

    // Act
    int result = myFunction(value);

    // Assert
    ASSERT_EQ(42, result);

    result.printSummary();
}

void runMyTests() {
    Serial.println("\n========================================");
    Serial.println("  🧪 My Test Suite");
    Serial.println("========================================");

    testMyFeature();

    Serial.println("\n✅ All tests completed!");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    runMyTests();
}

void loop() {
    // Nothing to do
}
```

### Assert Macros

| Macro | Purpose | Example |
|-------|---------|---------|
| `ASSERT_TRUE(cond)` | Verify condition is true | `ASSERT_TRUE(x > 0)` |
| `ASSERT_FALSE(cond)` | Verify condition is false | `ASSERT_FALSE(x < 0)` |
| `ASSERT_EQ(exp, act)` | Verify equality | `ASSERT_EQ(42, value)` |
| `ASSERT_NE(exp, act)` | Verify inequality | `ASSERT_NE(0, value)` |
| `ASSERT_STR_EQ(exp, act)` | String equality | `ASSERT_STR_EQ("hello", str)` |
| `ASSERT_GT(val1, val2)` | Greater than | `ASSERT_GT(x, 0)` |
| `ASSERT_LT(val1, val2)` | Less than | `ASSERT_LT(x, 100)` |

---

## Test Coverage

### Current Coverage

| Module | Coverage | Notes |
|--------|----------|-------|
| SpotifyClient | ~60% | Core logic tested |
| UI Components | ~40% | Layout calculations tested |
| AuthManager | 0% | Needs tests |
| WiFiManager | 0% | Needs tests |
| Config | 0% | Needs tests |

### Target Coverage

| Module | Target | Priority |
|--------|--------|----------|
| SpotifyClient | 90% | HIGH |
| UI Components | 80% | HIGH |
| AuthManager | 80% | MEDIUM |
| WiFiManager | 70% | MEDIUM |
| Config | 60% | LOW |

---

## Continuous Integration

### GitHub Actions (Planned)

```yaml
name: Run Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Set up PlatformIO
        run: pip install platformio
      - name: Run Tests
        run: pio test
```

---

## Known Issues

### Test Limitations

1. **No Mocking Framework** - Currently using simple mock objects. Consider using:
   - [FakeIt](https://github.com/eranpeer/FakeIt)
   - [CppUMock](https://github.com/ThrowTheSwitch/CppUTest)

2. **Hardware Dependencies** - Some tests require ESP32 hardware. Can't run:
   - Real WiFi connections
   - Real HTTPS requests
   - Real display rendering

3. **No Test Isolation** - Tests share global state in current implementation.

### Future Improvements

- [ ] Add mocking framework
- [ ] Create hardware-independent tests
- [ ] Add test fixtures for setup/teardown
- [ ] Add parameterized tests
- [ ] Add fuzz testing for input validation

---

## Troubleshooting

### Tests Fail on Upload

**Problem:** Tests don't run after upload.

**Solution:** Ensure `setup()` calls the test function and `loop()` is empty.

### Serial Monitor Shows Garbage

**Problem:** Serial output is unreadable.

**Solution:** Check baud rate is 115200 (defined in `SERIAL_BAUD`).

### Tests Hang

**Problem:** Test execution stops mid-way.

**Solution:** Add debug prints to identify which test is hanging.

---

## Resources

- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- [Arduino Unit Testing](https://github.com/Arduino-CI/arduino_ci)
- [ESP32 Testing Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/unit-tests.html)

---

**Last Updated:** 2026-02-02
**Maintainer:** Spotify GUI Agent
