/**
 * @file Logger.cpp
 * @brief Logger Implementation
 */

#include "Logger.hpp"

void Logger::debug(const char* tag, const char* format, ...) {
    if (logLevel > LogLevel::DEBUG || !enabled) {
        return;
    }

    va_list args;
    va_start(args, format);
    log(LogLevel::DEBUG, tag, format, args);
    va_end(args);
}

void Logger::info(const char* tag, const char* format, ...) {
    if (logLevel > LogLevel::INFO || !enabled) {
        return;
    }

    va_list args;
    va_start(args, format);
    log(LogLevel::INFO, tag, format, args);
    va_end(args);
}

void Logger::warning(const char* tag, const char* format, ...) {
    if (logLevel > LogLevel::WARNING || !enabled) {
        return;
    }

    va_list args;
    va_start(args, format);
    log(LogLevel::WARNING, tag, format, args);
    va_end(args);
}

void Logger::error(const char* tag, const char* format, ...) {
    if (logLevel > LogLevel::ERROR || !enabled) {
        return;
    }

    va_list args;
    va_start(args, format);
    log(LogLevel::ERROR, tag, format, args);
    va_end(args);
}

void Logger::fatal(const char* tag, const char* format, ...) {
    if (!enabled) {
        return;
    }

    va_list args;
    va_start(args, format);
    log(LogLevel::FATAL, tag, format, args);
    va_end(args);
}

void Logger::log(LogLevel level, const char* tag, const char* format, va_list args) {
    // Print timestamp
    unsigned long ms = millis();
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    Serial.printf("[%02lu:%02lu:%02lu] ", hours % 24, minutes % 60, seconds % 60);

    // Print level
    Serial.printf("%s ", getLevelPrefix(level));

    // Print tag
    if (tag) {
        Serial.printf("[%s] ", tag);
    }

    // Print message
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
}

const char* Logger::getLevelPrefix(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "🔍";
        case LogLevel::INFO:    return "ℹ️ ";
        case LogLevel::WARNING: return "⚠️ ";
        case LogLevel::ERROR:   return "❌";
        case LogLevel::FATAL:   return "💀";
        default:                return "?";
    }
}
