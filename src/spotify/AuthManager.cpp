/**
 * @file AuthManager.cpp
 * @brief Spotify OAuth 2.0 Authentication Manager Implementation
 */

#include "AuthManager.hpp"

AuthManager::AuthManager()
    : tokenExpiryTime(0)
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
    state = generateState();

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
    String url = String(SPOTIFY_AUTH_URL);
    url += "?client_id=" + clientId;
    url += "&response_type=code";
    url += "&redirect_uri=http://" + WiFi.localIP().toString() + ":" + String(AUTH_SERVER_PORT) + "/callback";
    url += "&scope=" + String(SPOTIFY_SCOPES);
    url += "&code_challenge=" + codeChallenge;
    url += "&code_challenge_method=S256";
    url += "&state=" + state;

    return url;
}

bool AuthManager::exchangeCodeForTokens(const String& code) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, SPOTIFY_TOKEN_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Build request body
    String body = "grant_type=authorization_code";
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

            tokenExpiryTime = millis() + (expiresIn * 1000);

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
    client.setInsecure();

    HTTPClient http;
    http.begin(client, SPOTIFY_TOKEN_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String body = "grant_type=refresh_token";
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

            tokenExpiryTime = millis() + (expiresIn * 1000);
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
    String html = "<!DOCTYPE html><html><head>";
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
    if (!authServer->hasArg("state") || authServer->arg("state") != state) {
        Serial.println("⚠️  Invalid state parameter");
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", "Invalid state");
        return;
    }

    // Check for error
    if (authServer->hasArg("error")) {
        String error = authServer->arg("error");
        Serial.printf("⚠️  Auth error: %s\n", error.c_str());
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", "Authentication error: " + error);
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
        authServer->send(500, "text/plain", "Failed to exchange code");
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

    for (size_t i = 0; i < length; i++) {
        result += charset[random(0, sizeof(charset) - 1)];
    }

    return result;
}

String AuthManager::base64UrlEncode(const String& input) {
    // Simple Base64 URL encode
    String encoded = base64::encode(input);

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

    return base64::decode(decoded);
}

String AuthManager::sha256(const String& input) {
    // Use ESP32 hardware SHA-256
    uint8_t hash[32];
    esp_sha(SHA256, (const uint8_t*)input.c_str(), input.length(), hash);

    // Convert to hex string, then encode
    String hex;
    for (int i = 0; i < 32; i++) {
        char buf[4];
        sprintf(buf, "%02x", hash[i]);
        hex += buf;
    }

    // Convert hex to bytes
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        uint8_t byte = strtol(hex.substring(i, i + 2).c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    // Base64 encode
    return base64::encode((const char*)bytes.data(), bytes.size());
}
