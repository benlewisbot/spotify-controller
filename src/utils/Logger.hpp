/**
 * @file Logger.hpp
 * @brief Simple Logging Utility
 *
 * Provides formatted logging with log levels and tags.
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <Arduino.h>

/**
 * @brief Log Level Enum
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

/**
 * @brief Logger Class
 *
 * Simple singleton logger for the application.
 */
class Logger {
public:
    /**
     * @brief Get the logger instance
     */
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Set log level
     */
    void setLevel(LogLevel level) { logLevel = level; }

    /**
     * @brief Enable/disable logging
     */
    void setEnabled(bool enable) { enabled = enable; }

    /**
     * @brief Log at DEBUG level
     */
    void debug(const char* tag, const char* format, ...);

    /**
     * @brief Log at INFO level
     */
    void info(const char* tag, const char* format, ...);

    /**
     * @brief Log at WARNING level
     */
    void warning(const char* tag, const char* format, ...);

    /**
     * @brief Log at ERROR level
     */
    void error(const char* tag, const char* format, ...);

    /**
     * @brief Log at FATAL level
     */
    void fatal(const char* tag, const char* format, ...);

private:
    Logger() : logLevel(LogLevel::INFO), enabled(true) {}

    /**
     * @brief Format and print log message
     */
    void log(LogLevel level, const char* tag, const char* format, va_list args);

    /**
     * @brief Get level prefix
     */
    const char* getLevelPrefix(LogLevel level);

    LogLevel logLevel;
    bool enabled;
};

// Convenience macros
#define LOG_D(tag, ...) Logger::getInstance().debug(tag, __VA_ARGS__)
#define LOG_I(tag, ...) Logger::getInstance().info(tag, __VA_ARGS__)
#define LOG_W(tag, ...) Logger::getInstance().warning(tag, __VA_ARGS__)
#define LOG_E(tag, ...) Logger::getInstance().error(tag, __VA_ARGS__)
#define LOG_F(tag, ...) Logger::getInstance().fatal(tag, __VA_ARGS__)

#endif // LOGGER_HPP
