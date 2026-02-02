/**
 * @file test_utils.hpp
 * @brief Utility functions for unit testing
 */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <Arduino.h>

// Test result tracking
struct TestResult {
    int passed;
    int failed;
    int total;

    TestResult() : passed(0), failed(0), total(0) {}

    void addPass() {
        passed++;
        total++;
    }

    void addFail() {
        failed++;
        total++;
    }

    void printSummary() {
        Serial.println("\n========================================");
        Serial.printf("Test Summary: %d/%d passed\n", passed, total);
        if (failed > 0) {
            Serial.printf("❌ %d test(s) FAILED\n", failed);
        } else {
            Serial.println("✅ All tests PASSED!");
        }
        Serial.println("========================================\n");
    }
};

// Assert macros
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        Serial.printf("❌ FAILED: %s (line %d)\n", #condition, __LINE__); \
        result.addFail(); \
    } else { \
        result.addPass(); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        Serial.printf("❌ FAILED: %s should be false (line %d)\n", #condition, __LINE__); \
        result.addFail(); \
    } else { \
        result.addPass(); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        Serial.printf("❌ FAILED: %d != %d (line %d)\n", (int)(expected), (int)(actual), __LINE__); \
        result.addFail(); \
    } else { \
        result.addPass(); \
    }

#define ASSERT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        Serial.printf("❌ FAILED: %d == %d (line %d)\n", (int)(expected), (int)(actual), __LINE__); \
        result.addFail(); \
    } else { \
        result.addPass(); \
    }

#define ASSERT_STR_EQ(expected, actual) \
    if (strcmp((expected), (actual)) != 0) { \
        Serial.printf("❌ FAILED: '%s' != '%s' (line %d)\n", (expected), (actual), __LINE__); \
        result.addFail(); \
    } else { \
        result.addPass(); \
    }

#define ASSERT_GT(val1, val2) \
    if (!((val1) > (val2))) { \
        Serial.printf("❌ FAILED: %d not > %d (line %d)\n", (int)(val1), (int)(val2), __LINE__); \
        result.addFail(); \
    } else { \
        result.addPass(); \
    }

#define ASSERT_LT(val1, val2) \
    if (!((val1) < (val2))) { \
        Serial.printf("❌ FAILED: %d not < %d (line %d)\n", (int)(val1), (int)(val2), __LINE__); \
        result.addFail(); \
    } else { \
        result.addPass(); \
    }

#endif // TEST_UTILS_HPP
