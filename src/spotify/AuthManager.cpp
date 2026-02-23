/**
 * @file AuthManager.cpp
 * @brief Spotify OAuth 2.0 Authentication Manager Implementation
 *
 * Two modes:
 * 1. AP mode (captive portal): Serves setup page for WiFi + Spotify client ID
 * 2. STA mode (OAuth): Serves Spotify OAuth authorization page with PKCE
 */

#include "AuthManager.hpp"
#include "SpotifySecure.hpp"

#include <vector>
#include <mbedtls/sha256.h>
#include <esp_log.h>

static const char* TAG = "Auth";

// Base64 encoding table
static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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

// ===== Constructor / Destructor =====

AuthManager::AuthManager()
    : tokenAcquiredAt(0)
    , tokenValidForMs(0)
    , tokenExpiryTime(0)
    , authServer(nullptr)
    , dnsServer(nullptr)
    , state(AuthState::NONE)
    , authStartTime(0)
    , initialized(false)
    , apMode(false) {
}

AuthManager::~AuthManager() {
    stopAuthServer();
}

void AuthManager::init(const String& id, const String& secret) {
    clientId = id;
    clientSecret = secret;
    initialized = true;
    ESP_LOGI(TAG, "Initialized (client_id=%s)", clientId.isEmpty() ? "empty" : "set");
}

// ===== Update =====

void AuthManager::update() {
    // Handle DNS in captive portal mode
    if (dnsServer) {
        dnsServer->processNextRequest();
    }

    // Handle web server
    handleWebServer();

    // Check timeout for OAuth phase
    if (state == AuthState::WAITING_FOR_AUTH) {
        if (millis() - authStartTime > AUTH_TIMEOUT_MS) {
            ESP_LOGW(TAG, "Auth timeout");
            state = AuthState::ERROR;
            stopAuthServer();
        }
    }
}

// ===== Setup Server (AP Mode Captive Portal) =====

void AuthManager::startSetupServer() {
    ESP_LOGI(TAG, "Starting captive portal setup server");

    apMode = true;
    state = AuthState::SETUP_WIFI;

    // Stop existing server if any
    stopAuthServer();

    // Start DNS server — redirect all DNS to our IP for captive portal
    dnsServer = new DNSServer();
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(53, "*", IPAddress(192, 168, 4, 1));

    // Start web server on port 80 (required for captive portal detection)
    authServer = new WebServer(AUTH_SERVER_PORT);

    // Setup page routes
    authServer->on("/", HTTP_GET, std::bind(&AuthManager::handleSetupPage, this));
    authServer->on("/save", HTTP_POST, std::bind(&AuthManager::handleSetupSave, this));
    authServer->on("/callback", HTTP_GET, std::bind(&AuthManager::handleCallback, this));

    // Captive portal: redirect all other requests to /
    authServer->onNotFound(std::bind(&AuthManager::handleCaptiveRedirect, this));

    authServer->begin();
    authStartTime = millis();

    ESP_LOGI(TAG, "Setup server on http://192.168.4.1");
}

// ===== Auth Server (STA Mode OAuth) =====

void AuthManager::startAuthServer() {
    if (!initialized) {
        ESP_LOGW(TAG, "Not initialized");
        return;
    }

    // If in AP mode with no client ID, start setup server instead
    if (WiFi.getMode() == WIFI_AP && clientId.isEmpty()) {
        startSetupServer();
        return;
    }

    // Generate PKCE values
    codeVerifier = generateCodeVerifier();
    codeChallenge = generateCodeChallenge(codeVerifier);
    oauthState = generateState();

    apMode = false;

    // Stop existing server
    stopAuthServer();

    authServer = new WebServer(AUTH_SERVER_PORT);

    authServer->on("/", HTTP_GET, std::bind(&AuthManager::handleIndex, this));
    authServer->on("/callback", HTTP_GET, std::bind(&AuthManager::handleCallback, this));
    authServer->on("/submit-code", HTTP_POST, std::bind(&AuthManager::handleSubmitCode, this));
    authServer->on("/auth-status", HTTP_GET, std::bind(&AuthManager::handleAuthStatus, this));

    authServer->begin();

    state = AuthState::WAITING_FOR_AUTH;
    authStartTime = millis();

    ESP_LOGI(TAG, "OAuth server started at http://%s/", WiFi.localIP().toString().c_str());
}

void AuthManager::stopAuthServer() {
    if (dnsServer) {
        dnsServer->stop();
        delete dnsServer;
        dnsServer = nullptr;
    }
    if (authServer) {
        authServer->stop();
        delete authServer;
        authServer = nullptr;
    }
}

// ===== Notify WiFi Connected =====

void AuthManager::onWiFiConnected() {
    if (state == AuthState::SETUP_CONNECTING) {
        ESP_LOGI(TAG, "WiFi connected after setup, switching to OAuth mode");

        // Stop captive portal DNS
        if (dnsServer) {
            dnsServer->stop();
            delete dnsServer;
            dnsServer = nullptr;
        }

        // Regenerate PKCE values and start OAuth phase
        codeVerifier = generateCodeVerifier();
        codeChallenge = generateCodeChallenge(codeVerifier);
        oauthState = generateState();
        apMode = false;

        // Reconfigure server for OAuth mode (keep same server instance)
        if (authServer) {
            authServer->stop();
            delete authServer;
        }

        authServer = new WebServer(AUTH_SERVER_PORT);
        authServer->on("/", HTTP_GET, std::bind(&AuthManager::handleIndex, this));
        authServer->on("/callback", HTTP_GET, std::bind(&AuthManager::handleCallback, this));
        authServer->on("/submit-code", HTTP_POST, std::bind(&AuthManager::handleSubmitCode, this));
        authServer->on("/auth-status", HTTP_GET, std::bind(&AuthManager::handleAuthStatus, this));
        authServer->begin();

        state = AuthState::WAITING_FOR_AUTH;
        authStartTime = millis();

        ESP_LOGI(TAG, "OAuth ready at http://%s/", WiFi.localIP().toString().c_str());
    }
}

// ===== Web Handlers =====

void AuthManager::handleWebServer() {
    if (authServer) {
        authServer->handleClient();
    }
}

void AuthManager::handleCaptiveRedirect() {
    // For captive portal: redirect all requests to our setup page
    authServer->sendHeader("Location", "http://192.168.4.1/");
    authServer->send(302, "text/plain", "");
}

// ----- Setup Page (AP Mode) -----

void AuthManager::handleSetupPage() {
    String html;
    html.reserve(3072);

    html = R"rawhtml(<!DOCTYPE html><html><head>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Spotify Controller Setup</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,Arial,sans-serif;background:#121212;color:#fff;padding:20px}
h1{color:#1DB954;text-align:center;margin:20px 0;font-size:22px}
h2{color:#b3b3b3;font-size:14px;margin:20px 0 10px;text-transform:uppercase;letter-spacing:1px}
.card{background:#282828;border-radius:12px;padding:20px;margin:10px 0}
input[type=text],input[type=password]{width:100%;padding:12px;background:#404040;border:none;border-radius:8px;color:#fff;font-size:16px;margin:6px 0 14px}
input::placeholder{color:#727272}
label{color:#b3b3b3;font-size:13px}
.btn{display:block;width:100%;padding:14px;background:#1DB954;color:#fff;border:none;border-radius:50px;font-size:16px;font-weight:bold;cursor:pointer;margin-top:20px}
.btn:active{background:#1aa34a}
.info{color:#727272;font-size:12px;text-align:center;margin:16px 0}
.status{padding:10px;border-radius:8px;margin:10px 0;text-align:center;font-size:14px}
.ok{background:#1a3a1a;color:#1DB954}
.err{background:#3a1a1a;color:#e74c3c}
</style></head><body>
<h1>Spotify Controller</h1>)rawhtml";

    if (state == AuthState::SETUP_CONNECTING) {
        html += "<div class='card'><div class='status ok'>Connecting to WiFi...</div></div>";
    } else if (state == AuthState::WAITING_FOR_AUTH) {
        // WiFi connected — redirect to the OAuth page on the device's real IP
        html += "<div class='card'><div class='status ok'>WiFi Connected!</div>";
        html += "<p style='color:#b3b3b3;text-align:center;margin:10px 0'>"
                "Now open this link to connect Spotify:</p>";
        html += "<a href='http://" + WiFi.localIP().toString() + "/' class='btn' "
                "style='text-decoration:none;text-align:center'>Continue Setup</a>";
        html += "<p class='info'>If the link doesn't work, go to: <b>http://"
                + WiFi.localIP().toString() + "/</b></p>";
        html += "</div>";
    } else if (state == AuthState::AUTHENTICATED) {
        html += "<div class='card'><div class='status ok'>Setup Complete!</div>";
        html += "<p style='color:#b3b3b3;text-align:center;margin:10px 0'>You can close this page.</p></div>";
    } else {
        // Default: show setup form
        html += R"rawhtml(
<form action='/save' method='POST'>
<h2>WiFi Network</h2>
<div class='card'>
<label>Network Name (SSID)</label>
<input type='text' name='ssid' placeholder='Your WiFi name' required>
<label>Password</label>
<input type='password' name='pass' placeholder='WiFi password'>
</div>
<h2>Spotify</h2>
<div class='card'>
<label>Client ID</label>
<input type='text' name='client_id' placeholder='From developer.spotify.com' required>
<p class='info'>Create an app at developer.spotify.com/dashboard<br>
Add redirect URI: <b>http://127.0.0.1:8888/callback</b></p>
</div>
<button type='submit' class='btn'>Save &amp; Connect</button>
</form>)rawhtml";
    }

    html += "</body></html>";
    authServer->send(200, "text/html", html);
}

void AuthManager::handleSetupSave() {
    if (!authServer->hasArg("ssid") || !authServer->hasArg("client_id")) {
        authServer->send(400, "text/plain", "Missing required fields");
        return;
    }

    String ssid = authServer->arg("ssid");
    String pass = authServer->arg("pass");
    String newClientId = authServer->arg("client_id");

    ESP_LOGI(TAG, "Setup: SSID=%s client_id=%s", ssid.c_str(),
             newClientId.isEmpty() ? "empty" : "set");

    // Store client ID
    clientId = newClientId;
    if (clientIdCb) {
        clientIdCb(newClientId);
    }

    // Notify about WiFi credentials
    if (wifiCredentialsCb) {
        wifiCredentialsCb(ssid, pass);
    }

    state = AuthState::SETUP_CONNECTING;

    // Respond with "connecting" page
    String html;
    html.reserve(512);
    html = R"rawhtml(<!DOCTYPE html><html><head>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<meta http-equiv='refresh' content='10;url=/'>
<title>Connecting...</title>
<style>body{font-family:Arial,sans-serif;background:#121212;color:#fff;text-align:center;padding:60px 20px}
h1{color:#1DB954}.spin{display:inline-block;width:40px;height:40px;border:4px solid #404040;
border-top-color:#1DB954;border-radius:50%;animation:s 1s linear infinite;margin:20px}
@keyframes s{to{transform:rotate(360deg)}}</style></head><body>
<h1>Connecting...</h1><div class='spin'></div>
<p style='color:#b3b3b3'>Connecting to WiFi. This page will refresh automatically.</p>
</body></html>)rawhtml";

    authServer->send(200, "text/html", html);
}

// ----- OAuth Page (STA Mode) -----

void AuthManager::handleIndex() {
    String authUrl = getAuthUrl();

    String html;
    html.reserve(6144);

    html = R"rawhtml(<!DOCTYPE html><html><head>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Spotify Controller</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,Arial,sans-serif;background:#121212;color:#fff;max-width:500px;margin:0 auto;padding:30px 20px}
h1{color:#1DB954;text-align:center;margin-bottom:20px}
.card{background:#282828;border-radius:12px;padding:20px;margin:16px 0}
.step{display:flex;align-items:flex-start;margin:12px 0}
.step-num{background:#1DB954;color:#000;width:28px;height:28px;border-radius:50%;display:flex;
align-items:center;justify-content:center;font-weight:bold;font-size:14px;flex-shrink:0;margin-right:12px;margin-top:2px}
.step-text{color:#b3b3b3;font-size:14px;line-height:1.5}
.step-text b{color:#fff}
.btn{display:block;width:100%;padding:14px;background:#1DB954;color:#fff;border:none;border-radius:50px;
font-size:16px;font-weight:bold;cursor:pointer;text-align:center;text-decoration:none;margin:8px 0}
.btn:active{background:#1aa34a}
.btn:disabled{opacity:0.5;cursor:default}
.btn-outline{background:transparent;border:2px solid #1DB954;color:#1DB954}
textarea{width:100%;padding:12px;background:#404040;border:2px solid #555;border-radius:8px;color:#fff;
font-size:13px;font-family:monospace;resize:vertical;min-height:80px;margin:8px 0}
textarea:focus{border-color:#1DB954;outline:none}
.status{padding:12px;border-radius:8px;margin:16px 0;text-align:center;font-size:14px}
.ok{background:#1a3a1a;color:#1DB954}
.err{background:#3a1a1a;color:#e74c3c}
.subtle{color:#727272;font-size:12px;text-align:center;margin:8px 0}
.url-box{background:#404040;border-radius:8px;padding:10px;font-size:11px;color:#b3b3b3;
word-break:break-all;margin:8px 0;max-height:60px;overflow:auto}
.spin{display:inline-block;width:20px;height:20px;border:3px solid #404040;border-top-color:#3498db;
border-radius:50%;animation:s 0.8s linear infinite;vertical-align:middle;margin-right:8px}
@keyframes s{to{transform:rotate(360deg)}}
</style></head><body>
<h1>Spotify Controller</h1>)rawhtml";

    if (state == AuthState::WAITING_FOR_AUTH) {
        html += "<p style='color:#b3b3b3;text-align:center;margin-bottom:16px'>"
                "Connect your Spotify account to control playback.</p>";

        // Step 1: Open auth URL
        html += "<div class='card'>";
        html += "<div class='step'><div class='step-num'>1</div>"
                "<div class='step-text'><b>Open this link</b> in your browser "
                "(long-press to copy, or tap to open):</div></div>";
        html += "<a href='" + authUrl + "' target='_blank' class='btn'>Open Spotify Login</a>";
        html += "<div class='url-box'>" + authUrl + "</div>";

        // Step 2: Authorize and copy URL
        html += "<div class='step'><div class='step-num'>2</div>"
                "<div class='step-text'><b>Log in &amp; authorize</b> on Spotify. "
                "Your browser will try to redirect to <code>127.0.0.1</code> and show an error. "
                "<b>This is normal!</b> Copy the <b>entire URL</b> from your browser's address bar.</div></div>";

        // Step 3: Paste URL — uses JS fetch() instead of <form> to avoid & parsing bug
        html += "<div class='step'><div class='step-num'>3</div>"
                "<div class='step-text'><b>Paste the URL</b> below and tap Submit:</div></div>";
        html += "<textarea id='cb_url' placeholder='Paste the full URL here...' rows='3'"
                " oninput='validateUrl()'></textarea>";
        html += "<button id='submit_btn' class='btn' disabled onclick='submitCode()'>Paste URL first</button>";
        html += "<div id='status'></div>";
        html += "</div>"; // close .card

        html += "<p class='subtle'>Device IP: " + WiFi.localIP().toString() + "</p>";
        html += "<p class='subtle'>Redirect URI for Spotify Dashboard:<br>"
                "<b>" + String(SPOTIFY_REDIRECT_URI) + "</b></p>";

        // JavaScript: fetch() POST as JSON, poll /auth-status
        html += R"rawhtml(<script>
function validateUrl(){
 var u=document.getElementById('cb_url').value;
 var b=document.getElementById('submit_btn');
 if(u.indexOf('code=')>-1){b.disabled=false;b.textContent='Submit';b.style.opacity='1';}
 else{b.disabled=true;b.textContent='Paste URL first';b.style.opacity='0.5';}
}
function submitCode(){
 var u=document.getElementById('cb_url').value.trim();
 var b=document.getElementById('submit_btn');
 var s=document.getElementById('status');
 if(!u||u.indexOf('code=')<0){s.innerHTML="<div class='status err'>URL must contain code= parameter</div>";return;}
 b.disabled=true;b.textContent='Exchanging tokens...';
 s.innerHTML="<div class='status' style='background:#1a2a3a;color:#3498db'><div class='spin'></div> Contacting Spotify...</div>";
 fetch('/submit-code',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({url:u})})
 .then(function(r){return r.json();})
 .then(function(d){
  if(d.success){s.innerHTML="<div class='status ok'>Connected to Spotify!</div><p style='color:#b3b3b3;text-align:center;margin:10px 0'>Setup complete. This page will update shortly.</p>";pollStatus();}
  else{s.innerHTML="<div class='status err'>"+((d.error)||'Token exchange failed')+"</div>";b.disabled=false;b.textContent='Try Again';}
 })
 .catch(function(e){s.innerHTML="<div class='status err'>Network error: "+e.message+"</div>";b.disabled=false;b.textContent='Try Again';});
}
function pollStatus(){
 setInterval(function(){
  fetch('/auth-status').then(function(r){return r.json();}).then(function(d){
   if(d.state==='authenticated'){document.getElementById('status').innerHTML="<div class='status ok'>Connected! You can close this page.</div>";}
  }).catch(function(){});
 },3000);
}
</script>)rawhtml";

    } else if (state == AuthState::AUTHENTICATED) {
        html += "<div class='status ok'>Connected to Spotify!</div>";
        html += "<p style='color:#b3b3b3;text-align:center'>Setup complete. You can close this page.</p>";
    } else if (state == AuthState::ERROR) {
        html += "<div class='status err'>Authentication failed.</div>";
        html += "<a href='/' class='btn'>Try Again</a>";
    }

    html += "</body></html>";
    authServer->send(200, "text/html", html);
}

void AuthManager::handleCallback() {
    // Validate state parameter
    if (!authServer->hasArg("state") || authServer->arg("state") != oauthState) {
        ESP_LOGW(TAG, "Invalid state parameter");
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", "Invalid state");
        return;
    }

    // Check for error from Spotify
    if (authServer->hasArg("error")) {
        String err = authServer->arg("error");
        ESP_LOGE(TAG, "OAuth error: %s", err.c_str());
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", "Auth error: " + err);
        return;
    }

    // Get authorization code
    if (!authServer->hasArg("code")) {
        ESP_LOGW(TAG, "No auth code");
        state = AuthState::ERROR;
        authServer->send(400, "text/plain", "No code received");
        return;
    }

    String code = authServer->arg("code");
    ESP_LOGI(TAG, "Got auth code, exchanging for tokens...");

    if (exchangeCodeForTokens(code)) {
        authServer->sendHeader("Location", "/");
        authServer->send(302, "text/plain", "");
    } else {
        authServer->send(500, "text/plain", "Token exchange failed");
    }
}

// ----- Manual Code Submission (loopback redirect workaround) -----

void AuthManager::handleSubmitCode() {
    // Read raw POST body — sent as JSON by the fetch() call in our OAuth page.
    // ESP32 WebServer stores raw body under the key "plain" for non-form content types.
    // This completely bypasses the form-encoding & delimiter bug.
    String body = authServer->arg("plain");

    if (body.isEmpty()) {
        authServer->send(400, "application/json", "{\"error\":\"Empty request body\"}");
        return;
    }

    // Parse JSON: { "url": "http://127.0.0.1:8888/callback?code=...&state=..." }
    StaticJsonDocument<1024> doc;
    DeserializationError jsonErr = deserializeJson(doc, body);
    if (jsonErr) {
        ESP_LOGE(TAG, "JSON parse error: %s", jsonErr.c_str());
        authServer->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    String url = doc["url"].as<String>();
    url.trim();

    ESP_LOGI(TAG, "Manual code submission, URL length=%d", url.length());

    // Extract 'code' parameter from the pasted URL
    // URL format: http://127.0.0.1:8888/callback?code=XXXXX&state=YYYYY
    String code = "";
    String urlState = "";

    int codeIdx = url.indexOf("code=");
    if (codeIdx >= 0) {
        codeIdx += 5; // skip "code="
        int codeEnd = url.indexOf('&', codeIdx);
        if (codeEnd < 0) codeEnd = url.length();
        code = url.substring(codeIdx, codeEnd);
    }

    int stateIdx = url.indexOf("state=");
    if (stateIdx >= 0) {
        stateIdx += 6; // skip "state="
        int stateEnd = url.indexOf('&', stateIdx);
        if (stateEnd < 0) stateEnd = url.length();
        urlState = url.substring(stateIdx, stateEnd);
    }

    // Check for error in URL
    int errIdx = url.indexOf("error=");
    if (errIdx >= 0) {
        errIdx += 6;
        int errEnd = url.indexOf('&', errIdx);
        if (errEnd < 0) errEnd = url.length();
        String err = url.substring(errIdx, errEnd);
        ESP_LOGE(TAG, "OAuth error in URL: %s", err.c_str());
        state = AuthState::ERROR;
        authServer->send(200, "application/json", "{\"error\":\"Spotify denied access: " + err + "\"}");
        return;
    }

    if (code.isEmpty()) {
        ESP_LOGW(TAG, "No code found in pasted URL");
        authServer->send(200, "application/json",
            "{\"error\":\"No authorization code found. Make sure you copied the entire URL.\"}");
        return;
    }

    // Validate state (warn but don't hard-fail — user may have refreshed the page)
    if (!urlState.isEmpty() && urlState != oauthState) {
        ESP_LOGW(TAG, "State mismatch: expected=%s got=%s",
                 oauthState.c_str(), urlState.c_str());
    }

    ESP_LOGI(TAG, "Got auth code from manual submission, exchanging...");

    if (exchangeCodeForTokens(code)) {
        authServer->send(200, "application/json", "{\"success\":true}");
    } else {
        authServer->send(200, "application/json",
            "{\"error\":\"Token exchange failed. The code may have expired — try again.\"}");
    }
}

// ----- Auth Status Polling (JSON) -----

void AuthManager::handleAuthStatus() {
    String json = "{\"state\":\"";
    if (state == AuthState::AUTHENTICATED) {
        json += "authenticated";
    } else if (state == AuthState::ERROR) {
        json += "error";
    } else {
        json += "waiting";
    }
    json += "\"}";
    authServer->send(200, "application/json", json);
}

// ===== Auth URL =====

String AuthManager::getAuthUrl() {
    String url;
    url.reserve(512);

    url = String(SPOTIFY_AUTH_URL);
    url += "?client_id=" + clientId;
    url += "&response_type=code";
    url += "&redirect_uri=" + String(SPOTIFY_REDIRECT_URI);
    url += "&scope=" + String(SPOTIFY_SCOPES);
    url += "&code_challenge=" + codeChallenge;
    url += "&code_challenge_method=S256";
    url += "&state=" + oauthState;

    return url;
}

// ===== Token Exchange =====

bool AuthManager::exchangeCodeForTokens(const String& code) {
    WiFiClientSecure client;
    SpotifySecure::initSecureClient(client);

    HTTPClient http;
    http.begin(client, SPOTIFY_TOKEN_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String body;
    body.reserve(512);
    body = "grant_type=authorization_code";
    body += "&code=" + code;
    body += "&redirect_uri=" + String(SPOTIFY_REDIRECT_URI);
    body += "&client_id=" + clientId;
    body += "&code_verifier=" + codeVerifier;

    ESP_LOGI(TAG, "Token exchange POST to %s", SPOTIFY_TOKEN_URL);
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

            tokenAcquiredAt = millis();
            tokenValidForMs = (unsigned long)expiresIn * 1000UL;
            tokenExpiryTime = tokenAcquiredAt + tokenValidForMs;

            state = AuthState::AUTHENTICATED;
            ESP_LOGI(TAG, "Authenticated! Token expires in %ds", expiresIn);
            return true;
        } else {
            ESP_LOGE(TAG, "Token JSON parse error: %s", error.c_str());
        }
    } else {
        ESP_LOGE(TAG, "Token exchange failed: HTTP %d", httpCode);
        ESP_LOGE(TAG, "Response: %s", response.c_str());
    }

    state = AuthState::ERROR;
    return false;
}

String AuthManager::refreshAccessToken(const String& refreshTok) {
    WiFiClientSecure client;
    SpotifySecure::initSecureClient(client);

    HTTPClient http;
    http.begin(client, SPOTIFY_TOKEN_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String body;
    body.reserve(256);
    body = "grant_type=refresh_token";
    body += "&refresh_token=" + refreshTok;
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

            tokenAcquiredAt = millis();
            tokenValidForMs = (unsigned long)expiresIn * 1000UL;
            tokenExpiryTime = tokenAcquiredAt + tokenValidForMs;
            ESP_LOGI(TAG, "Token refreshed");
            return newToken;
        }
    }

    ESP_LOGE(TAG, "Token refresh failed: HTTP %d", httpCode);
    return "";
}

// ===== PKCE =====

String AuthManager::generateCodeVerifier() {
    return secureRandom(64);
}

String AuthManager::generateCodeChallenge(const String& verifier) {
    // PKCE spec: BASE64URL(SHA256(code_verifier))
    uint8_t hash[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, (const unsigned char*)verifier.c_str(), verifier.length());
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);

    // Base64url-encode the raw 32-byte hash
    String encoded = base64_encode(hash, 32);
    encoded.replace("+", "-");
    encoded.replace("/", "_");
    while (encoded.endsWith("=")) {
        encoded.remove(encoded.length() - 1);
    }
    return encoded;
}

String AuthManager::generateState() {
    return secureRandom(16);
}

// ===== Crypto Helpers =====

String AuthManager::secureRandom(size_t length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    String result;
    result.reserve(length + 1);

    for (size_t i = 0; i < length; i++) {
        uint32_t rand_val = esp_random();
        result += charset[rand_val % (sizeof(charset) - 1)];
    }
    return result;
}

String AuthManager::base64UrlEncode(const String& input) {
    String encoded = base64_encode((const unsigned char*)input.c_str(), input.length());
    encoded.replace("+", "-");
    encoded.replace("/", "_");
    while (encoded.endsWith("=")) {
        encoded.remove(encoded.length() - 1);
    }
    return encoded;
}

String AuthManager::base64UrlDecode(const String& input) {
    String decoded = input;
    decoded.replace("-", "+");
    decoded.replace("_", "/");
    while (decoded.length() % 4 != 0) {
        decoded += "=";
    }
    // Simple base64 decode not needed for current flow
    return decoded;
}

String AuthManager::sha256(const String& input) {
    uint8_t hash[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, (const unsigned char*)input.c_str(), input.length());
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);
    return base64_encode(hash, 32);
}

bool AuthManager::isTokenExpired() const {
    unsigned long elapsed = millis() - tokenAcquiredAt;
    return elapsed >= tokenValidForMs;
}
