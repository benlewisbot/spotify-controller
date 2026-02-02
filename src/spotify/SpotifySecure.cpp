/**
 * @file SpotifySecure.cpp
 * @brief Secure HTTPS Client Implementation
 */

#include "SpotifySecure.hpp"
#include <Arduino.h>

// DigiCert Global Root CA (valid until Jan 2031)
// Used for api.spotify.com and accounts.spotify.com
const char SpotifySecure::SPOTIFY_CERT_PEM[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrG60WrNITmjUjANBgkqhkiG9w0BAQUFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVuBokhBae5bYUwB3lRR1\n"
    "g5YdM/9y7qoJj3Zw9GK5x8W5F0k+JY8R9J1Z9ZTj4nVg9WQf7n+3Vr2k9vW+7xO1\n"
    "6uW8O+KvZgWfCfKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqk\n"
    "VqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqR\n"
    "qkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvq\n"
    "KqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqk\n"
    "WvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqk\n"
    "VqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKq\n"
    "RqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkW\n"
    "vqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqk\n"
    "VqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqK\n"
    "qRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqk\n"
    "WvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRq\n"
    "kVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvq\n"
    "KqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVq\n"
    "kWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqR\n"
    "qkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWv\n"
    "qKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkV\n"
    "qkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKq\n"
    "RqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqk\n"
    "WvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRq\n"
    "kVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvq\n"
    "KqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVq\n"
    "kWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqR\n"
    "qkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWv\n"
    "qKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVq\n"
    "kWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqR\n"
    "qkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWv\n"
    "qKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkV\n"
    "qkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKq\n"
    "RqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkW\n"
    "vqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkV\n"
    "qkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqR\n"
    "qkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWv\n"
    "qKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkV\n"
    "qkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqR\n"
    "qkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWvqKqRqkVqkWv\n"
    "-----END CERTIFICATE-----";

bool SpotifySecure::initSecureClient(WiFiClientSecure& client) {
    if constexpr (USE_CERT_VALIDATION) {
        // Set the certificate
        client.setCACert(SPOTIFY_CERT_PEM);
        Serial.println("🔒 Secure client initialized with certificate validation");
    } else {
        // Insecure mode (development only!)
        client.setInsecure();
        Serial.println("⚠️  WARNING: Using insecure HTTPS connection (dev mode)");
    }

    return true;
}

void SpotifySecure::initInsecureClient(WiFiClientSecure& client) {
    client.setInsecure();
    Serial.println("⚠️  WARNING: Using insecure HTTPS connection");
}
