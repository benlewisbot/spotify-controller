/**
 * @file SpotifySecure.cpp
 * @brief Secure HTTPS Client Implementation
 */

#include "SpotifySecure.hpp"
#include <Arduino.h>

// DigiCert Global Root CA (valid until Nov 10 2031)
// Used for api.spotify.com and accounts.spotify.com
const char SpotifySecure::SPOTIFY_CERT_PEM[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWNXAI8b+ePR1\n"
    "KKhHNPMnFGax1asBolhk+AI7HcbO7FGwOMOfGBs3iTO6BM7WlbSyIB5KlGax1aQd\n"
    "ePpajgKMeKzCN/p3JQOIupSKbAIKv2lLfcqKOLQaZ9B7F7bTajSsBnLViSB+nMbs\n"
    "6ejQ/5OK2l1mY0cjEGiK5Glp5urZbXJJD+rM6i47sJpcEeI=\n"
    "-----END CERTIFICATE-----";

bool SpotifySecure::initSecureClient(WiFiClientSecure& client) {
    if constexpr (USE_CERT_VALIDATION) {
        // Set the certificate
        client.setCACert(SPOTIFY_CERT_PEM);
        Serial.println("Secure client initialized with certificate validation");
    } else {
        // Insecure mode (development only!)
        client.setInsecure();
        Serial.println("WARNING: Using insecure HTTPS connection (dev mode)");
    }

    return true;
}

void SpotifySecure::initInsecureClient(WiFiClientSecure& client) {
    client.setInsecure();
    Serial.println("WARNING: Using insecure HTTPS for image downloads");
}
