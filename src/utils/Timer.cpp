/**
 * @file Timer.cpp
 * @brief Timer Implementation
 */

#include "Timer.hpp"

Timer::Timer(unsigned long intervalMs)
    : interval(intervalMs)
    , lastTime(millis()) {
}

bool Timer::elapsed() {
    return (millis() - lastTime) >= interval;
}

bool Timer::elapsedAndReset() {
    if (elapsed()) {
        reset();
        return true;
    }
    return false;
}

void Timer::reset() {
    lastTime = millis();
}

void Timer::setInterval(unsigned long intervalMs) {
    interval = intervalMs;
}
