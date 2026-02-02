/**
 * @file SpotifySecure.hpp
 * @brief Secure HTTPS Client for Spotify API
 *
 * Provides certificate-pinned HTTPS connections for secure
 * communication with Spotify API endpoints.
 */

#ifndef SPOTIFY_SECURE_HPP
#define SPOTIFY_SECURE_HPP

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

class SpotifySecure {
public:
    /**
     * @brief Initialize secure client with certificate pinning
     *
     * @param client WiFiClientSecure reference
     * @return true if successful
     */
    static bool initSecureClient(WiFiClientSecure& client);

    /**
     * @brief Initialize client without certificate validation (dev mode)
     *
     * WARNING: Only use for development! Not secure!
     *
     * @param client WiFiClientSecure reference
     */
    static void initInsecureClient(WiFiClientSecure& client);

private:
    // Spotify API certificate (DigiCert Global Root CA)
    // Expires: Jan 2031
    static const char SPOTIFY_CERT_PEM[];

    // Whether to use certificate validation
    static constexpr bool USE_CERT_VALIDATION = true;
};

#endif // SPOTIFY_SECURE_HPP
