/**
 * @file errors.h
 * @brief Error handling definitions for Spotify Controller
 */

#pragma once

/**
 * @brief Spotify API error codes
 */
enum class SpotifyError {
    OK = 0,
    NETWORK_ERROR,
    AUTH_EXPIRED,
    AUTH_FAILED,
    RATE_LIMITED,
    API_ERROR,
    PARSE_ERROR,
    NOT_PLAYING,
    DEVICE_NOT_FOUND,
    INVALID_PARAMETERS,
    FILESYSTEM_ERROR,
    OUT_OF_MEMORY,
    UNKNOWN
};

/**
 * @brief Convert error code to human-readable string
 * @param err The error code
 * @return String representation of the error
 */
inline const char* errorToString(SpotifyError err) {
    switch (err) {
        case SpotifyError::OK: return "OK";
        case SpotifyError::NETWORK_ERROR: return "Network error";
        case SpotifyError::AUTH_EXPIRED: return "Authentication expired";
        case SpotifyError::AUTH_FAILED: return "Authentication failed";
        case SpotifyError::RATE_LIMITED: return "Rate limited";
        case SpotifyError::API_ERROR: return "API error";
        case SpotifyError::PARSE_ERROR: return "Parse error";
        case SpotifyError::NOT_PLAYING: return "Nothing playing";
        case SpotifyError::DEVICE_NOT_FOUND: return "Device not found";
        case SpotifyError::INVALID_PARAMETERS: return "Invalid parameters";
        case SpotifyError::FILESYSTEM_ERROR: return "Filesystem error";
        case SpotifyError::OUT_OF_MEMORY: return "Out of memory";
        default: return "Unknown error";
    }
}
