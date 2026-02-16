/**
 * @file AuthManager.cpp
 * @brief Spotify OAuth 2.0 Authentication Manager Implementation
 */

#include "AuthManager.hpp"
#include "SpotifySecure.hpp"

#include <vector>
#include <mbedtls/sha256.h>

// Base64 encoding table
static const char base64_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Base64 decoding table
static const int base64_index[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

// Inline base64 encode function
static String base64_encode(const unsigned char* input, size_t len) {
    String result;
    result.reserve((len + 2) / 3 * 4 + 1);
    
    for (size_t i = 0; i < len; i += 3) {
        unsigned int n = ((unsigned int)input[i]) << 16;
        if (i + 1 < len) n |= ((unsigned int)input[i + 1]) << 8;
        if (i + 2 < len) n |= input[i + 2];
        
        result += base64_chars[(n >> 18) & 63];
        result += base64_chars[(n >> 12) & 63];
        result += (i + 1 < len) ? base64_chars[(n >> 6) & 63] : '=';
        result += (i + 2 < len) ? base64_chars[n & 63] : '=';
    }
    
    return result;
}

// Inline base64 decode function
static String base64_decode(const String& input) {
    String result;
    size_t len = input.length();
    if (len == 0) return result;
    
    result.reserve(len / 4 * 3 + 1);
    
    for (size_t i = 0; i < len; i += 4) {
        int n = base64_index[(unsigned char)input[i]] << 18;
        n |= base64_index[(unsigned char)input[i + 1]] << 12;
        n |= (input[i + 2] != '=') ? base64_index[(unsigned char)input[i + 2]] << 6 : 0;
        n |= (input[i + 3] != '=') ? base64_index[(unsigned char)input[i + 3]] : 0;
        
        result += (char)((n >> 16) & 255);
        if (input[i + 2] != '=') result += (char)((n >> 8) & 255);
        if (input[i + 3] != '=') result += (char)(n & 255);
    }
    
    return result;
}

AuthManager::AuthManager()
    : tokenAcquiredAt(0)
    , tokenValidForMs(0)
    , tokenExpiryTime(0)
    , authServer(nullptr)
    , state(AuthState::NONE)
    , authStartTime(0)
    , initialized(false) {
}

AuthManager::~AuthManager() {
    stopAuthServer();
}

void AuthManager::init(const String& id, const String& secret) {
    clientId = id;
    clientSecret = secret;

    Serial.println("🔐 AuthManager initialized");
    initialized = true;
}

void AuthManager::update() {
    if (state == AuthState::WAITING_FOR_AUTH) {
        // Check for timeout
        if (millis() - authStartTime > AUTH_TIMEOUT_MS) {
            Serial.println("⏰ Auth timeout");
            state = AuthState::ERROR;
            stopAuthServer();
        }
    }

    handleWebServer();
}

void AuthManager::startAuthServer() {
    if (!initialized) {
        Serial.println("⚠️  AuthManager not initialized");
        return;
    }

    Serial.println("🌐 Starting auth server...");

    // Generate PKCE values
    codeVerifier = generateCodeVerifier();
    codeChallenge = generateCodeChallenge(codeVerifier);
    oauthState = generateState();

    // Create web server
    authServer = new WebServer(AUTH_SERVER_PORT);

    // Register handlers
    authServer->on("/", std::bind(&AuthManager::handleIndex, this));
    authServer->on("/callback", std::bind(&AuthManager::handleCallback, this));

    // Start server
    authServer->begin();

    Serial.printf("✅ Auth server started on port %d\n", AUTH_SERVER_PORT);
    Serial.printf("🔗 Auth URL: %s\n", getAuthUrl().c_str());

    this->state = AuthState::WAITING_FOR_AUTH;
    authStartTime = millis();
}

void AuthManager::stopAuthServer() {
    if (authServer) {
        authServer->stop();
        delete authServer;
        authServer = nullptr;
        Serial.println("🛑 Auth server stopped");
    }
}

String AuthManager::getAuthUrl() {
    String url;
    url.reserve(512);  // Reserve enough space for full auth URL (Bug #17 fix)

    url = String(SPOTIFY_AUTH_URL);
    url += "?client_id=" + clientId;
    url += "&response_type=code";
    url += "&redirect_uri=http://" + WiFi.localIP().toString() + ":" + String(AUTH_SERVER_PORT) + "/callback";
    url += "&scope=" + String(SPOTIFY_SCOPES);
    url += "&code_challenge=" + codeChallenge;
    url += "&code_challenge_method=S256";
    url += "&state=" + oauthState;

    return url;
}

bool AuthManager::exchangeCodeForTokens(const String& code) {
    WiFiClientSecure client;
    SpotifySecure::initSecureClient(client);

    HTTPClient http;
    http.begin(client, SPOTIFY_TOKEN_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Build request body (Bug #17 fix: add reserve)
    String body;
    body.reserve(512);
    body = "grant_type=authorization_code";
    body += "&code=" + code;
    body += "&redirect_uri=http://" + WiFi.localIP().toString() + ":" + String(AUTH_SERVER_PORT) + "/callback";
    body += "&client_id=" + clientId;
    body += "&code_verifier=" + codeVerifier;

    int httpCode = http.POST(body);
    String response = http.getString();

    http.end();

    if (httpCode == 200) {
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            accessToken = doc["access_token"].as<String>();
            refreshToken = doc["refresh_token"].as<String>();
            int expiresIn = doc["expires_in"].as<int>();

            // Use overflow-safe token tracking
            tokenAcquiredAt = millis();
            tokenValidForMs = (unsigned long)expiresIn * 1000UL;
            // For backwards compatibility (deprecated field)
            tokenExpiryTime = tokenAcquiredAt + tokenValidForMs;

            state = AuthState::AUTHENTICATED;
            Serial.println("✅ Authentication successful!");
            Serial.printf("  Access token: %s...\n", accessToken.substring(0, 20).c_str());
            Serial.printf("  Expires in: %d seconds\n", expiresIn);

            return true;
        } else {
            Serial.printf("⚠️  JSON parse error: %s\n", error.c_str());
        }
    } else {
        Serial.printf("⚠️  Token exchange failed: %d\n", httpCode);
        Serial.printf("  Response: %s\n", response.c_str());
    }

    state = AuthState::ERROR;
    return false;
}

String AuthManager::refreshAccessToken(const String& refreshToken) {
    WiFiClientSecure client;
    SpotifySecure::initSecureClient(client);

    HTTPClient http;
    http.begin(client, SPOTIFY_TOKEN_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Build request body (Bug #17 fix: add reserve)
    String body;
    body.reserve(256);
    body = "grant_type=refresh_token";
    body += "&refresh_token=" + refreshToken;
    body += "&client_id=" + clientId;

    int httpCode = http.POST(body);
    String response = http.getString();

    http.end();

    if (httpCode == 200) {
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            String newToken = doc["access_token"].as<String>();
            int expiresIn = doc["expires_in"].as<int>();

            // Use overflow-safe token tracking
            tokenAcquiredAt = millis();
            tokenValidForMs = (unsigned long)expiresIn * 1000UL;
            // For backwards compatibility (deprecated field)
            tokenExpiryTime = tokenAcquiredAt + tokenValidForMs;
            Serial.println("✅ Access token refreshed");

            return newToken;
        }
    }

    Serial.printf("⚠️  Token refresh failed: %d\n", httpCode);
    return "";
}

void AuthManager::handleWebServer() {
    if (authServer) {
        authServer->handleClient();
    }
}

void AuthManager::handleIndex() {
    // Build HTML response (Bug #17 fix: add reserve)
    String html;
    html.reserve(1024);  // Reserve enough for the full HTML page

    html = "<!DOCTYPE html><html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>Spotify Controller</title>";
    html += "<style>";
    html += "body{font-family:Arial,sans-serif;max-width:600px;margin:40px auto;padding:20px;}";
    html += "h1{color:#1DB954;text-align:center;}";
    html += ".container{text-align:center;}";
    html += ".btn{display:inline-block;background:#1DB954;color:white;";
    html += "padding:15px 30px;text-decoration:none;border-radius:50px;";
    html += "margin:20px 0;font-size:18px;}";
    html += ".status{margin:20px 0;padding:10px;background:#f0f0f0;border-radius:5px;}";
    html += "</style></head><body>";
    html += "<h1>🎵 Spotify Controller</h1>";
    html += "<div class='container'>";
    html += "<p>Connect your Spotify account to control playback.</p>";

    if (this->state == AuthState::WAITING_FOR_AUTH) {
        html += "<div class='status'>Waiting for authentication...</div>";
        html += "<a href='" + getAuthUrl() + "' class='btn'>Connect Spotify</a>";
    } else if (this->state == AuthState::AUTHENTICATED) {
        html += "<div class='status' style='background:#d4edda;'>✅ Successfully connected!</div>";
        html += "<p>You can close this window.</p>";
    } else if (this->state == AuthState::ERROR) {
        html += "<div class='status' style='background:#f8d7da;'>❌ Authentication failed</div>";
        html += "<a href='/' class='btn'>Try Again</a>";
    }

    html += "</div></body></html>";

    authServer->send(200, "text/html", html);
}

void AuthManager::handleCallback() {
    // Check state
    if (!authServer->hasArg("state") || authServer->arg("state") != oauthState) {
        Serial.println("⚠️  Invalid state parameter");
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", "Invalid state");
        return;
    }

    // Check for error
    if (authServer->hasArg("error")) {
        String errorMsg;
        errorMsg.reserve(128);  // Bug #17 fix
        errorMsg = "Authentication error: ";
        errorMsg += authServer->arg("error");
        Serial.printf("⚠️  Auth error: %s\n", errorMsg.c_str());
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", errorMsg);
        return;
    }

    // Get auth code
    if (!authServer->hasArg("code")) {
        Serial.println("⚠️  No auth code received");
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", "No code received");
        return;
    }

    String code = authServer->arg("code");

    // Exchange code for tokens
    if (exchangeCodeForTokens(code)) {
        authServer->sendHeader("Location", "/");
        authServer->send(302, "text/plain", "");
    } else {
        String failMsg;
        failMsg.reserve(64);  // Bug #17 fix
        failMsg = "Failed to exchange code";
        authServer->send(500, "text/plain", failMsg);
    }
}

String AuthManager::generateCodeVerifier() {
    // Generate 43-128 character random string
    return secureRandom(64);
}

String AuthManager::generateCodeChallenge(const String& verifier) {
    // SHA-256 hash, then Base64 URL encode
    String hash = sha256(verifier);
    return base64UrlEncode(hash);
}

String AuthManager::generateState() {
    return secureRandom(16);
}

String AuthManager::secureRandom(size_t length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    String result;

    result.reserve(length + 1);  // Bug #17 fix: reserve before loop concatenations

    for (size_t i = 0; i < length; i++) {
        // Use esp_random() for cryptographically secure random numbers
        uint32_t rand_val = esp_random();
        result += charset[rand_val % (sizeof(charset) - 1)];
    }

    return result;
}

String AuthManager::base64UrlEncode(const String& input) {
    // Use inline base64 encode
    String encoded = base64_encode((const unsigned char*)input.c_str(), input.length());
    
    // Replace + with -, / with _, and remove padding
    encoded.replace("+", "-");
    encoded.replace("/", "_");

    // Remove = padding
    while (encoded.endsWith("=")) {
        encoded.remove(encoded.length() - 1);
    }

    return encoded;
}

String AuthManager::base64UrlDecode(const String& input) {
    String decoded = input;
    decoded.replace("-", "+");
    decoded.replace("_", "/");

    // Add padding
    while (decoded.length() % 4 != 0) {
        decoded += "=";
    }

    // Decode using inline base64
    return base64_decode(decoded);
}

String AuthManager::sha256(const String& input) {
    // Use mbedtls SHA-256
    uint8_t hash[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0); // 0 = SHA256, not SHA224
    mbedtls_sha256_update(&ctx, (const unsigned char*)input.c_str(), input.length());
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);

    // Base64 encode the hash (32 bytes)
    return base64_encode(hash, 32);
}

bool AuthManager::isTokenExpired() const {
    // Overflow-safe elapsed time calculation
    // (millis() - tokenAcquiredAt) gives correct difference even after ~49 day overflow
    unsigned long elapsed = millis() - tokenAcquiredAt;
    return elapsed >= tokenValidForMs;
}
