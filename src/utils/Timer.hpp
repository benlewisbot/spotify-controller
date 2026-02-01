/**
 * @file Timer.hpp
 * @brief Timer Utility Class
 *
 * Simple interval timer for periodic tasks.
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

/**
 * @brief Timer Class
 *
 * Simple interval-based timer for triggering periodic tasks.
 */
class Timer {
public:
    /**
     * @brief Create a timer
     * @param intervalMs Interval in milliseconds
     */
    explicit Timer(unsigned long intervalMs = 1000);

    /**
     * @brief Check if timer has elapsed
     * @return true if interval has passed since last check
     */
    bool elapsed();

    /**
     * @brief Check if timer has elapsed and reset
     * @return true if interval has passed since last check
     */
    bool elapsedAndReset();

    /**
     * @brief Reset the timer
     */
    void reset();

    /**
     * @brief Set new interval
     */
    void setInterval(unsigned long intervalMs);

    /**
     * @brief Get current interval
     */
    unsigned long getInterval() const { return interval; }

    /**
     * @brief Get time elapsed since last reset
     */
    unsigned long getElapsed() const { return millis() - lastTime; }

private:
    unsigned long interval;
    unsigned long lastTime;
};

/**
 * @brief Debounce Class
 *
 * Simple debounce utility for buttons and switches.
 */
class Debounce {
public:
    Debounce(unsigned long debounceMs = 50)
        : debounceTime(debounceMs)
        , lastState(LOW)
        , lastDebounceTime(0)
        , state(LOW) {
    }

    /**
     * @brief Update debounce state
     * @param current Current pin reading
     * @return Debounced state
     */
    bool update(bool current) {
        // Check for state change
        if (current != lastState) {
            lastDebounceTime = millis();
            lastState = current;
        }

        // Check debounce time
        if ((millis() - lastDebounceTime) > debounceTime) {
            state = current;
        }

        return state;
    }

    /**
     * @brief Check for rising edge (button pressed)
     */
    bool risingEdge() {
        bool current = state;
        if (current && !previousState) {
            previousState = current;
            return true;
        }
        previousState = current;
        return false;
    }

    /**
     * @brief Check for falling edge (button released)
     */
    bool fallingEdge() {
        bool current = state;
        if (!current && previousState) {
            previousState = current;
            return true;
        }
        previousState = current;
        return false;
    }

private:
    unsigned long debounceTime;
    bool lastState;
    unsigned long lastDebounceTime;
    bool state;
    bool previousState;
};

#endif // TIMER_HPP
