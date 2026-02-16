/**
 * @file result.h
 * @brief Result types for error propagation
 */

#pragma once

#include "errors.h"
#include <Arduino.h>

/**
 * @brief Generic result type that can hold either a value or an error
 * @tparam T The type of value to hold
 */
template<typename T>
struct Result {
    T value;
    SpotifyError error;
    String message;

    /**
     * @brief Check if the result is successful
     * @return true if no error, false otherwise
     */
    bool ok() const { return error == SpotifyError::OK; }

    /**
     * @brief Create a successful result
     * @param val The value to wrap
     * @return Result with OK status
     */
    static Result<T> success(T val) {
        return {val, SpotifyError::OK, ""};
    }

    /**
     * @brief Create a failure result
     * @param err The error code
     * @param msg Optional error message
     * @return Result with error status
     */
    static Result<T> failure(SpotifyError err, const String& msg = "") {
        return {T{}, err, msg};
    }
};

/**
 * @brief Status type for operations that don't return a value
 */
struct Status {
    SpotifyError error;
    String message;

    /**
     * @brief Check if the status is successful
     * @return true if no error, false otherwise
     */
    bool ok() const { return error == SpotifyError::OK; }

    /**
     * @brief Create a successful status
     * @return Status with OK status
     */
    static Status success() {
        return {SpotifyError::OK, ""};
    }

    /**
     * @brief Create a failure status
     * @param err The error code
     * @param msg Optional error message
     * @return Status with error
     */
    static Status failure(SpotifyError err, const String& msg = "") {
        return {err, msg};
    }
};

/**
 * @brief HTTP result with detailed response information
 */
struct HttpResult {
    int statusCode;
    String body;
    SpotifyError error;
    String message;

    /**
     * @brief Check if the HTTP request was successful
     * @return true if status code is 2xx, false otherwise
     */
    bool ok() const { return statusCode >= 200 && statusCode < 300; }

    /**
     * @brief Create a successful HTTP result
     * @param code HTTP status code
     * @param responseBody Response body
     * @return HttpResult with OK status
     */
    static HttpResult success(int code, const String& responseBody = "") {
        return {code, responseBody, SpotifyError::OK, ""};
    }

    /**
     * @brief Create a failed HTTP result
     * @param code HTTP status code
     * @param err Spotify error code
     * @param msg Error message
     * @return HttpResult with error
     */
    static HttpResult failure(int code, SpotifyError err, const String& msg = "") {
        return {code, "", err, msg};
    }

    /**
     * @brief Create a failed HTTP result from network error
     * @param err Network error code (negative)
     * @param msg Error message
     * @return HttpResult with network error
     */
    static HttpResult networkFailure(int err, const String& msg = "") {
        return {err, "", SpotifyError::NETWORK_ERROR, msg};
    }
};
