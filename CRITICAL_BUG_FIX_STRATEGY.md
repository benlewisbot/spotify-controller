# 🎯 CRITICAL BUG FIX STRATEGY

**Erstellt von:** Claude Opus  
**Datum:** 12. Februar 2026  
**Zweck:** Priorisierte Anleitung für GLM zum systematischen Bug-Fixing

---

## 📊 Übersicht: Top 17 Critical Bugs

| # | Bug ID | Beschreibung | Est. Zeit | Abhängigkeiten |
|---|--------|--------------|-----------|----------------|
| 1 | KRIT-05 | yield() fehlt in Timeout-Loops | 5 min | - |
| 2 | KRIT-08 | LVGL Event Handler falscher Parameter | 20 min | - |
| 3 | KRIT-07 | WiFi Event Handler Memory Corruption | 15 min | - |
| 4 | HOCH-05 | Division by Zero in Progress Bar | 5 min | - |
| 5 | KRIT-02 | Typo "SpototifyClient" + nullptr | 10 min | - |
| 6 | KRIT-04 | StaticJsonDocument Stack Overflow | 15 min | - |
| 7 | KRIT-01 | Memory Leak HTTP in downloadImage() | 30 min | - |
| 8 | KRIT-03 | Use-After-Free in CoverManager | 30 min | Nach #7 |
| 9 | KRIT-06 | MenuManager Overlay Memory Leak | 30 min | Nach #2 |
| 10 | KRIT-09 | Token Expiry millis() Overflow | 20 min | - |
| 11 | KRIT-10 | LittleFS Race Condition | 45 min | - |
| 12 | HOCH-07 | UART Buffer Overflow/Sync Loss | 30 min | - |
| 13 | HOCH-06 | Uninitialized Variables in structs | 15 min | - |
| 14 | HOCH-08 | Missing volatile für ISR | 10 min | - |
| 15 | HOCH-02 | Touch Handler Bounds Check | 10 min | - |
| 16 | HOCH-04 | Fehlende Error Propagation | 60 min | Nach #7,#8 |
| 17 | HOCH-01 | String Memory Fragmentation | 45 min | - |

**Gesamtzeit geschätzt:** ~6 Stunden

---

## 🔧 FIX BATCHES (Optimale Reihenfolge)

### Batch A: Quick Wins (25 min)
Bugs #1, #4, #5 - Sofort umsetzbar, verhindern Crashes

### Batch B: LVGL & Events (35 min)  
Bugs #2, #3 - Event-Handler Fixes

### Batch C: Memory Management (75 min)
Bugs #6, #7, #8, #9 - HTTP und JSON Memory Fixes

### Batch D: Stability (95 min)
Bugs #10, #11, #12, #13, #14, #15 - System-Stabilität

### Batch E: Quality (105 min)
Bugs #16, #17 - Error Handling & Performance

---

# 📋 DETAILLIERTE FIX-ANLEITUNG PRO BUG

---

## Bug #1: KRIT-05 - yield() fehlt in Timeout-Loops

### Problem
`src/utils.cpp` Zeile ~80: while-Loop ohne yield() triggert ESP32 Watchdog Reset.

### GLM Fix-Anleitung

**Schritt 1:** Datei öffnen
```bash
# Finde die Datei
find . -name "utils.cpp" -o -name "Utils.cpp"
```

**Schritt 2:** Suche nach while-Loops mit millis() Timeout
```cpp
// SUCHE NACH PATTERN:
while (client.available() == 0) {
    if (millis() > timeout) {
```

**Schritt 3:** Füge yield() ein
```cpp
// ERSETZE MIT:
while (client.available() == 0) {
    if (millis() > timeout) {
        client.stop();
        return "";
    }
    yield();  // ← HINZUFÜGEN: Wichtig für ESP32 Watchdog!
    delay(1);  // ← Optional: Kurze Pause um CPU zu entlasten
}
```

**Schritt 4:** Suche nach ALLEN ähnlichen Patterns im Projekt
```bash
grep -rn "while.*available.*== 0" src/
grep -rn "while.*millis" src/
```

**Schritt 5:** Jeden Fund prüfen und yield() hinzufügen

### Quality Check
- [ ] Alle while-Loops mit millis() haben yield()
- [ ] Kompiliert ohne Fehler
- [ ] ESP32 bootet ohne Watchdog Reset

---

## Bug #2: KRIT-08 - LVGL Event Handler falscher Parameter

### Problem
`src/ui/screens/DisplaySettingsScreen.cpp`: LVGL 8.x Event-Callback hat falsches Signatur.

### GLM Fix-Anleitung

**Schritt 1:** Finde alle LVGL Event Callbacks
```bash
grep -rn "lv_obj_add_event_cb" src/
grep -rn "LV_EVENT_" src/
```

**Schritt 2:** Identifiziere falsches Pattern
```cpp
// FALSCH (LVGL 7.x Syntax):
lv_obj_add_event_cb(obj, [](lv_obj_t* obj, lv_event_t e) {
    if (e == LV_EVENT_VALUE_CHANGED) {  // e ist POINTER, nicht enum!
```

**Schritt 3:** Korrigiere zu LVGL 8.x Syntax
```cpp
// RICHTIG (LVGL 8.x Syntax):
lv_obj_add_event_cb(obj, [](lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t* target = lv_event_get_target(e);
        void* user_data = lv_event_get_user_data(e);
        // ...
    }
}, LV_EVENT_VALUE_CHANGED, this);  // Event-Filter + User-Data
```

**Schritt 4:** Dateien zu prüfen
- `src/ui/screens/DisplaySettingsScreen.cpp`
- `src/ui/screens/WiFiSettingsScreen.cpp`
- `src/ui/screens/SpotifySettingsScreen.cpp`
- `src/ui/screens/MenuManager.cpp`
- Alle anderen `*Screen.cpp` Dateien

**Schritt 5:** Template für häufige Events
```cpp
// Slider Value Changed:
lv_obj_add_event_cb(slider, [](lv_event_t* e) {
    int value = lv_slider_get_value(lv_event_get_target(e));
    auto* self = static_cast<MyScreen*>(lv_event_get_user_data(e));
    self->onValueChanged(value);
}, LV_EVENT_VALUE_CHANGED, this);

// Button Clicked:
lv_obj_add_event_cb(btn, [](lv_event_t* e) {
    auto* self = static_cast<MyScreen*>(lv_event_get_user_data(e));
    self->onButtonClick();
}, LV_EVENT_CLICKED, this);
```

### Quality Check
- [ ] Alle `lv_obj_add_event_cb` Calls haben `lv_event_t*` Parameter
- [ ] Kein direkter Vergleich `e == LV_EVENT_*` (muss `lv_event_get_code(e)` sein)
- [ ] User-Data wird korrekt gecastet
- [ ] Kompiliert ohne Warnings

---

## Bug #3: KRIT-07 - WiFi Event Handler Memory Corruption

### Problem
`src/network/WiFiManager.cpp` Zeile ~150: Falscher Cast von TaskHandle zu WiFiEventInfo_t*.

### GLM Fix-Anleitung

**Schritt 1:** Finde WiFi Event Handler
```bash
grep -rn "ARDUINO_EVENT_WIFI" src/
grep -rn "WiFiEvent_t" src/
```

**Schritt 2:** Identifiziere den Bug
```cpp
// FALSCH:
case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    Serial.printf("📶 WiFi disconnected: %d\n",
        ((WiFiEventInfo_t*)xTaskGetCurrentTaskHandle())->disconnected.reason);
    // ↑ KOMPLETT FALSCH! xTaskGetCurrentTaskHandle() gibt KEINEN Event-Info!
```

**Schritt 3:** Prüfe die Event-Registration
```cpp
// Event-Handler Registrierung sollte so aussehen:
WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    // Hier ist info bereits korrekt!
}, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
```

**Schritt 4:** Korrigiere den Handler
```cpp
// RICHTIG:
void WiFiManager::onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.printf("📶 WiFi disconnected, reason: %d\n", 
                info.wifi_sta_disconnected.reason);
            // Reconnect-Logik...
            break;
            
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("📶 WiFi connected!");
            break;
            
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.printf("📶 Got IP: %s\n", 
                WiFi.localIP().toString().c_str());
            break;
    }
}

// Registration:
void WiFiManager::init() {
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        this->onWiFiEvent(event, info);
    });
}
```

**Schritt 5:** Entferne alle falschen TaskHandle Casts
```bash
grep -rn "xTaskGetCurrentTaskHandle" src/
# Alle Funde prüfen - dies sollte NICHT für Event-Info verwendet werden!
```

### Quality Check
- [ ] Kein `xTaskGetCurrentTaskHandle()` Cast zu Event-Info
- [ ] `WiFiEventInfo_t info` wird als Parameter übergeben
- [ ] Disconnect-Reason wird korrekt ausgelesen
- [ ] WiFi reconnected nach Disconnect

---

## Bug #4: HOCH-05 - Division by Zero in Progress Bar

### Problem
`src/main_uart.cpp` Zeile ~180: Division durch `currentTrack.duration` ohne Check.

### GLM Fix-Anleitung

**Schritt 1:** Finde alle Division mit duration
```bash
grep -rn "/ *duration" src/
grep -rn "/duration" src/
grep -rn "/ currentTrack.duration" src/
```

**Schritt 2:** Identifiziere das Problem
```cpp
// FALSCH:
progressW = (currentTrack.progress * w) / currentTrack.duration;
// Wenn duration == 0 → Division by Zero → Crash!
```

**Schritt 3:** Fix mit Guard
```cpp
// RICHTIG:
if (currentTrack.duration > 0) {
    progressW = (currentTrack.progress * w) / currentTrack.duration;
} else {
    progressW = 0;
}

// ODER einzeilig:
progressW = (currentTrack.duration > 0) 
    ? (currentTrack.progress * w) / currentTrack.duration 
    : 0;
```

**Schritt 4:** Suche nach ähnlichen Problemen
```bash
grep -rn "/ *\w*\." src/  # Division durch Member-Variable
```

Typische Fälle:
- Progress / Duration
- Current / Total  
- Used / Capacity

### Quality Check
- [ ] Alle Divisionen haben Null-Check
- [ ] Kein `/ 0` möglich
- [ ] Progress-Bar zeigt 0% wenn duration == 0

---

## Bug #5: KRIT-02 - Typo "SpototifyClient" + nullptr

### Problem
`src/playback_controller.cpp` Zeile 15: Typo im Typ + fehlender nullptr Check.

### GLM Fix-Anleitung

**Schritt 1:** Finde den Typo
```bash
grep -rn "Spoto" src/ include/
grep -rn "SpototifyClient" src/ include/
```

**Schritt 2:** Korrigiere alle Vorkommen
```cpp
// FALSCH:
PlaybackController::PlaybackController(SpototifyClient* spotify)

// RICHTIG:
PlaybackController::PlaybackController(SpotifyClient* spotify)
```

**Schritt 3:** Füge nullptr Check hinzu
```cpp
PlaybackController::PlaybackController(SpotifyClient* spotify)
    : spotifyClient(spotify) {
    if (!spotify) {
        Serial.println("⚠️ WARNING: PlaybackController created with null SpotifyClient!");
    }
}

bool PlaybackController::isReady() const {
    return spotifyClient != nullptr && spotifyClient->isAuthenticated();
}
```

**Schritt 4:** Prüfe alle Methoden auf nullptr-Verwendung
```cpp
bool PlaybackController::play() {
    if (!isReady()) {
        Serial.println("❌ Cannot play: SpotifyClient not ready");
        return false;
    }
    return spotifyClient->play();
}
```

### Quality Check
- [ ] Kein "Spoto" mehr im Code
- [ ] Constructor prüft nullptr
- [ ] `isReady()` existiert und wird genutzt
- [ ] Kompiliert ohne Typ-Errors

---

## Bug #6: KRIT-04 - StaticJsonDocument Stack Overflow

### Problem
`src/spotify/SpotifyClient.cpp`: 4KB StaticJsonDocument auf Stack kann Stack Overflow verursachen.

### GLM Fix-Anleitung

**Schritt 1:** Finde alle StaticJsonDocument
```bash
grep -rn "StaticJsonDocument" src/
```

**Schritt 2:** Analysiere die Größen
```cpp
// PROBLEMATISCH (auf Stack):
StaticJsonDocument<4096> doc;   // 4KB Stack!
StaticJsonDocument<8192> doc;   // 8KB Stack! SEHR GEFÄHRLICH!
StaticJsonDocument<2048> doc;   // 2KB noch OK, aber grenzwertig
```

**Schritt 3:** Ersetze mit DynamicJsonDocument für große Docs
```cpp
// RICHTIG (auf Heap):
DynamicJsonDocument doc(4096);

// Error-Handling hinzufügen:
DynamicJsonDocument doc(4096);
DeserializationError error = deserializeJson(doc, payload);
if (error) {
    Serial.printf("❌ JSON parse error: %s\n", error.c_str());
    return false;
}

// Memory check:
Serial.printf("📊 JSON memory used: %d/%d bytes\n", 
    doc.memoryUsage(), doc.capacity());
```

**Schritt 4:** Für kleine, bekannte Responses - StaticJsonDocument OK
```cpp
// OK für kleine Responses (< 1KB):
StaticJsonDocument<512> doc;  // Für einfache Status-Responses
```

**Schritt 5:** Größen-Empfehlungen
```cpp
// Spotify API Response Größen:
// /me/player         → ~2-3KB → DynamicJsonDocument(3072)
// /me/player/devices → ~1-2KB → DynamicJsonDocument(2048)  
// /search           → ~4-8KB → DynamicJsonDocument(8192)
// Simple status     → ~200B  → StaticJsonDocument<512>
```

### Quality Check
- [ ] Keine StaticJsonDocument > 2048 bytes
- [ ] DynamicJsonDocument für API Responses
- [ ] Error-Handling bei deserializeJson()
- [ ] Kein Stack Overflow bei API Calls

---

## Bug #7: KRIT-01 - Memory Leak HTTP in downloadImage()

### Problem
`src/spotify/SpotifyClient.cpp` Zeile ~300-350: `http.end()` wird bei early return nicht aufgerufen.

### GLM Fix-Anleitung

**Schritt 1:** Finde downloadImage Funktion
```bash
grep -rn "downloadImage" src/
```

**Schritt 2:** Analysiere alle Return-Pfade
```cpp
// PROBLEM:
bool SpotifyClient::downloadImage(const String& url, const String& path) {
    HTTPClient http;
    http.begin(url);
    
    int code = http.GET();
    if (code != 200) {
        return false;  // ← http.end() FEHLT!
    }
    
    WiFiClient* stream = http.getStreamPtr();
    File file = LittleFS.open(path, "w");
    if (!file) {
        return false;  // ← http.end() FEHLT!
    }
    
    // ... download ...
    
    file.close();
    http.end();  // ← Nur hier wird es aufgerufen
    return true;
}
```

**Schritt 3:** RAII Guard Pattern implementieren
```cpp
// In einem Header (z.B. include/utils/HttpGuard.h):
struct HttpGuard {
    HTTPClient& http;
    HttpGuard(HTTPClient& h) : http(h) {}
    ~HttpGuard() { 
        http.end(); 
        Serial.println("🔒 HTTP connection closed");
    }
};

// Verwendung:
bool SpotifyClient::downloadImage(const String& url, const String& path) {
    HTTPClient http;
    HttpGuard guard(http);  // ← Garantiert http.end() bei JEDEM return!
    
    http.begin(url);
    int code = http.GET();
    
    if (code != 200) {
        Serial.printf("❌ Download failed: HTTP %d\n", code);
        return false;  // HttpGuard ruft http.end() auf!
    }
    
    // ... rest of function ...
    return true;  // HttpGuard ruft http.end() auf!
}
```

**Schritt 4:** Anwenden auf ALLE HTTPClient Verwendungen
```bash
grep -rn "HTTPClient" src/
```

Für jede Funktion:
1. Prüfen ob alle return-Pfade `http.end()` haben
2. HttpGuard verwenden OR goto-cleanup pattern

**Schritt 5:** Alternative ohne extra Klasse
```cpp
bool SpotifyClient::downloadImage(const String& url, const String& path) {
    HTTPClient http;
    bool success = false;
    
    http.begin(url);
    
    do {  // Fake-Loop für break-basiertes Cleanup
        int code = http.GET();
        if (code != 200) break;
        
        WiFiClient* stream = http.getStreamPtr();
        File file = LittleFS.open(path, "w");
        if (!file) break;
        
        // ... download ...
        
        file.close();
        success = true;
    } while(false);
    
    http.end();  // IMMER aufgerufen!
    return success;
}
```

### Quality Check
- [ ] Jede HTTPClient-Funktion hat guaranteed http.end()
- [ ] HttpGuard Klasse erstellt (oder alternatives Pattern)
- [ ] Keine Memory Leaks bei Error-Returns
- [ ] Getestet mit fehlerhaften URLs

---

## Bug #8: KRIT-03 - Use-After-Free in CoverManager

### Problem
`src/cover_manager.cpp` Zeile ~200: Stream wird nach http.end() noch referenziert.

### GLM Fix-Anleitung

**Schritt 1:** Finde CoverManager download Code
```bash
grep -rn "getStreamPtr" src/
```

**Schritt 2:** Identifiziere das Problem
```cpp
// FALSCH:
WiFiClient* stream = http.getStreamPtr();
// ... use stream ...
http.end();  // ← Stream wird UNGÜLTIG!
// stream darf danach NICHT mehr verwendet werden!
```

**Schritt 3:** Korrekte Reihenfolge sicherstellen
```cpp
// RICHTIG:
WiFiClient* stream = http.getStreamPtr();
if (!stream) {
    http.end();
    return false;
}

// Stream komplett lesen BEVOR http.end()
uint8_t buffer[512];
while (stream->available()) {
    int bytesRead = stream->readBytes(buffer, sizeof(buffer));
    file.write(buffer, bytesRead);
}

file.close();
http.end();  // ← ERST JETZT, nachdem Stream fertig ist!
```

**Schritt 4:** Prüfe Exception-Safety
```cpp
bool CoverManager::downloadCover(const String& url, const String& path) {
    HTTPClient http;
    http.begin(url);
    
    int code = http.GET();
    if (code != HTTP_CODE_OK) {
        http.end();
        return false;
    }
    
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();
    
    File file = LittleFS.open(path, "w");
    if (!file) {
        http.end();  // ← Cleanup!
        return false;
    }
    
    uint8_t buffer[512];
    int totalRead = 0;
    
    while (http.connected() && (contentLength > 0 || contentLength == -1)) {
        size_t available = stream->available();
        if (available) {
            int bytesRead = stream->readBytes(buffer, 
                min(available, sizeof(buffer)));
            file.write(buffer, bytesRead);
            totalRead += bytesRead;
            if (contentLength > 0) contentLength -= bytesRead;
        }
        yield();  // ← Wichtig!
    }
    
    file.close();
    http.end();  // ← Am Ende
    
    Serial.printf("✅ Downloaded %d bytes to %s\n", totalRead, path.c_str());
    return true;
}
```

### Quality Check
- [ ] stream wird NICHT nach http.end() verwendet
- [ ] Alle Fehler-Pfade rufen http.end() vor return
- [ ] yield() in download-Loop
- [ ] Erfolgreich große Bilder (>50KB) downloaden

---

## Bug #9: KRIT-06 - MenuManager Overlay Memory Leak

### Problem
`src/ui/screens/MenuManager.cpp`: LVGL Objekte werden erstellt aber nie gelöscht.

### GLM Fix-Anleitung

**Schritt 1:** Finde MenuManager Klasse
```bash
cat src/ui/screens/MenuManager.cpp
cat include/ui/screens/MenuManager.h
```

**Schritt 2:** Identifiziere erstellte LVGL Objekte
```cpp
// Diese werden erstellt:
void MenuManager::init(lv_obj_t* rootObj) {
    root = rootObj;
    loadingOverlay = lv_obj_create(root);
    messageOverlay = lv_obj_create(root);
    // ... weitere Overlays ...
}
```

**Schritt 3:** Destruktor hinzufügen
```cpp
// In MenuManager.h:
class MenuManager {
public:
    MenuManager();
    ~MenuManager();  // ← HINZUFÜGEN
    // ...
private:
    lv_obj_t* root = nullptr;
    lv_obj_t* loadingOverlay = nullptr;
    lv_obj_t* messageOverlay = nullptr;
    // ... alle LVGL Objekte mit = nullptr initialisieren!
};

// In MenuManager.cpp:
MenuManager::~MenuManager() {
    // LVGL Objekte löschen (nur wenn sie existieren und root noch valid)
    if (loadingOverlay) {
        lv_obj_del(loadingOverlay);
        loadingOverlay = nullptr;
    }
    if (messageOverlay) {
        lv_obj_del(messageOverlay);
        messageOverlay = nullptr;
    }
    // ... für alle erstellten Objekte ...
    
    // WICHTIG: root NICHT löschen wenn es extern übergeben wurde!
    // root = nullptr;  // Nur Pointer nullen, nicht löschen
}
```

**Schritt 4:** Prüfe alle Screen-Klassen
```bash
ls src/ui/screens/
```

Jede Screen-Klasse braucht:
1. Destruktor mit `lv_obj_del()` für eigene Objekte
2. nullptr Initialisierung im Header
3. Null-Check vor `lv_obj_del()`

**Schritt 5:** Base Class für Screens (optional aber empfohlen)
```cpp
// include/ui/screens/BaseScreen.h
class BaseScreen {
protected:
    lv_obj_t* screen = nullptr;
    
public:
    virtual ~BaseScreen() {
        if (screen) {
            lv_obj_del(screen);
            screen = nullptr;
        }
    }
    
    virtual void show() {
        if (screen) lv_scr_load(screen);
    }
    
    virtual void hide() {
        // Optional: Animation oder cleanup
    }
};
```

### Quality Check
- [ ] Alle Screen-Klassen haben Destruktor
- [ ] Alle LVGL Pointer sind mit nullptr initialisiert
- [ ] `lv_obj_del()` nur wenn Pointer nicht null
- [ ] Kein Memory-Wachstum nach mehrmaligem Screen-Wechsel

---

## Bug #10: KRIT-09 - Token Expiry millis() Overflow

### Problem
`src/spotify/SpotifyClient.cpp` Zeile ~400: millis() Overflow nach 49 Tagen nicht behandelt.

### GLM Fix-Anleitung

**Schritt 1:** Finde Token-Expiry Code
```bash
grep -rn "tokenExpir" src/
grep -rn "millis()" src/spotify/
```

**Schritt 2:** Identifiziere das Problem
```cpp
// FALSCH:
tokenExpiryTime = millis() + 3600000;  // 1 Stunde ab jetzt
// ...später...
if (millis() >= tokenExpiryTime) {  // BUG: Overflow!
    refreshToken();
}
```

Das Problem: Nach ~49 Tagen macht millis() einen Overflow (zurück auf 0).
Wenn `tokenExpiryTime = 4294967290` und `millis() = 5`, dann ist die Bedingung false obwohl Token längst expired ist!

**Schritt 3:** Overflow-sichere Implementierung
```cpp
// In SpotifyClient.h:
class SpotifyClient {
private:
    unsigned long tokenAcquiredAt = 0;   // Wann Token geholt wurde
    unsigned long tokenValidForMs = 0;    // Wie lange gültig (ms)
    // NICHT: tokenExpiryTime (absoluter Zeitpunkt)

    bool isTokenExpired() const {
        // Overflow-sicherer Vergleich:
        // (millis() - tokenAcquiredAt) gibt korrekte Differenz auch bei Overflow
        unsigned long elapsed = millis() - tokenAcquiredAt;
        return elapsed >= tokenValidForMs;
    }
};

// In SpotifyClient.cpp:
bool SpotifyClient::refreshAccessToken() {
    // ... API Call ...
    
    if (success) {
        tokenAcquiredAt = millis();  // Jetzt
        tokenValidForMs = expiresIn * 1000UL;  // API gibt Sekunden
        
        // Safety margin: 5 Minuten früher refreshen
        if (tokenValidForMs > 300000UL) {
            tokenValidForMs -= 300000UL;
        }
        
        Serial.printf("🔑 Token valid for %lu seconds\n", tokenValidForMs / 1000);
    }
    return success;
}

bool SpotifyClient::ensureValidToken() {
    if (isTokenExpired()) {
        Serial.println("🔄 Token expired, refreshing...");
        return refreshAccessToken();
    }
    return true;
}
```

**Schritt 4:** Suche alle millis() Vergleiche im Projekt
```bash
grep -rn "millis() [><=]" src/
grep -rn "[><=] millis()" src/
```

Für jeden Fund prüfen: Wird ein absoluter Zeitpunkt verglichen? → Umbauen auf Elapsed-Pattern!

### Quality Check
- [ ] Kein `millis() >= absoluteTime` Pattern
- [ ] Elapsed-Zeit-Berechnung: `millis() - startTime`
- [ ] Token wird korrekt vor Ablauf refreshed
- [ ] Test: Manuell `tokenAcquiredAt` auf hohen Wert setzen

---

## Bug #11: KRIT-10 - LittleFS Race Condition

### Problem
`src/config/Config.cpp`: Bei Stromausfall zwischen delete und rename ist Config verloren.

### GLM Fix-Anleitung

**Schritt 1:** Finde Config save Code
```bash
grep -rn "LittleFS.rename" src/
grep -rn "CONFIG_FILE" src/
```

**Schritt 2:** Identifiziere das Problem
```cpp
// GEFÄHRLICH:
bool ConfigManager::save() {
    File file = LittleFS.open(tempFile, "w");
    // ... schreiben ...
    file.close();
    
    LittleFS.remove(CONFIG_FILE);  // ← Original weg!
    // STROMAUSFALL HIER = DATEN VERLOREN!
    LittleFS.rename(tempFile, CONFIG_FILE);
}
```

**Schritt 3:** Atomic Write mit Backup
```cpp
bool ConfigManager::save() {
    const char* CONFIG_FILE = "/config.json";
    const char* TEMP_FILE = "/config.tmp";
    const char* BACKUP_FILE = "/config.bak";
    
    // 1. In Temp-Datei schreiben
    File file = LittleFS.open(TEMP_FILE, "w");
    if (!file) {
        Serial.println("❌ Cannot open temp file for writing");
        return false;
    }
    
    // JSON serialisieren
    DynamicJsonDocument doc(1024);
    // ... populate doc ...
    
    if (serializeJson(doc, file) == 0) {
        file.close();
        LittleFS.remove(TEMP_FILE);
        Serial.println("❌ Failed to serialize config");
        return false;
    }
    file.close();
    
    // 2. Altes Backup löschen (falls vorhanden)
    if (LittleFS.exists(BACKUP_FILE)) {
        LittleFS.remove(BACKUP_FILE);
    }
    
    // 3. Aktuelle Config → Backup (falls vorhanden)
    if (LittleFS.exists(CONFIG_FILE)) {
        if (!LittleFS.rename(CONFIG_FILE, BACKUP_FILE)) {
            Serial.println("⚠️ Could not create backup");
            // Weitermachen - nicht kritisch
        }
    }
    
    // 4. Temp → Config
    if (!LittleFS.rename(TEMP_FILE, CONFIG_FILE)) {
        Serial.println("❌ Failed to rename temp to config");
        // Versuche Backup wiederherzustellen
        if (LittleFS.exists(BACKUP_FILE)) {
            LittleFS.rename(BACKUP_FILE, CONFIG_FILE);
        }
        return false;
    }
    
    Serial.println("✅ Config saved successfully");
    return true;
}

bool ConfigManager::load() {
    // Versuche Config zu laden
    if (LittleFS.exists(CONFIG_FILE)) {
        if (loadFromFile(CONFIG_FILE)) {
            return true;
        }
        Serial.println("⚠️ Config corrupted, trying backup...");
    }
    
    // Fallback auf Backup
    if (LittleFS.exists(BACKUP_FILE)) {
        if (loadFromFile(BACKUP_FILE)) {
            Serial.println("✅ Restored from backup");
            // Backup als neue Config speichern
            LittleFS.rename(BACKUP_FILE, CONFIG_FILE);
            return true;
        }
    }
    
    Serial.println("⚠️ No valid config found, using defaults");
    setDefaults();
    return false;
}
```

### Quality Check
- [ ] Backup-Datei wird erstellt
- [ ] Bei Load wird Backup als Fallback versucht
- [ ] Kein Datenverlust bei Stromausfall-Simulation
- [ ] Temp-Datei wird bei Fehler gelöscht

---

## Bug #12: HOCH-07 - UART Buffer Overflow/Sync Loss

### Problem
`include/touch_manager_uart.h`: Bei Sync-Verlust werden endlos falsche Daten gelesen.

### GLM Fix-Anleitung

**Schritt 1:** Finde UART Touch Code
```bash
grep -rn "getTouchPoint" src/ include/
cat include/touch_manager_uart.h
```

**Schritt 2:** Identifiziere das Problem
```cpp
// PROBLEM:
TouchPoint getTouchPoint() {
    if (serial->available() >= 7) {
        uint8_t header = serial->read();
        // Wenn header != 0xAA, werden trotzdem 6 Bytes gelesen
        // → Sync verloren, alle weiteren Reads sind Müll!
```

**Schritt 3:** Robuste Sync-Implementierung
```cpp
TouchPoint TouchManagerUART::getTouchPoint() {
    TouchPoint point = {0, 0, false};
    
    // Sync-Byte suchen
    while (serial->available() > 0) {
        uint8_t byte = serial->peek();  // Nur schauen, nicht lesen
        
        if (byte == TOUCH_HEADER_BYTE) {  // 0xAA
            break;  // Header gefunden!
        }
        
        // Falsches Byte → verwerfen
        serial->read();
        syncLostCount++;
        
        if (syncLostCount > 100) {
            Serial.println("⚠️ UART sync lost, flushing buffer");
            while (serial->available()) serial->read();
            syncLostCount = 0;
            return point;
        }
    }
    
    // Brauchen mindestens 7 Bytes: Header + X(2) + Y(2) + Pressure(1) + Checksum(1)
    if (serial->available() < 7) {
        return point;  // Noch nicht genug Daten
    }
    
    // Header lesen
    uint8_t header = serial->read();
    if (header != TOUCH_HEADER_BYTE) {
        return point;  // Sollte nicht passieren nach obigem Check
    }
    
    // Daten lesen
    uint8_t data[6];
    serial->readBytes(data, 6);
    
    // Checksum validieren
    uint8_t checksum = header;
    for (int i = 0; i < 5; i++) {
        checksum ^= data[i];
    }
    
    if (checksum != data[5]) {
        Serial.printf("⚠️ Touch checksum error: expected %02X, got %02X\n", 
            checksum, data[5]);
        return point;  // Ungültige Daten
    }
    
    // Daten parsen
    point.x = (data[0] << 8) | data[1];
    point.y = (data[2] << 8) | data[3];
    point.touched = (data[4] > 10);  // Pressure threshold
    
    // Bounds Check (Bug #15)
    if (point.x > DISPLAY_WIDTH || point.y > DISPLAY_HEIGHT) {
        Serial.printf("⚠️ Touch out of bounds: %d,%d\n", point.x, point.y);
        point.touched = false;
    }
    
    syncLostCount = 0;
    return point;
}
```

**Schritt 4:** Header-Konstanten definieren
```cpp
// In touch_manager_uart.h:
#define TOUCH_HEADER_BYTE 0xAA
#define TOUCH_PACKET_SIZE 7
```

### Quality Check
- [ ] Sync-Byte wird gesucht, nicht angenommen
- [ ] Checksum-Validierung implementiert
- [ ] Buffer wird bei Sync-Verlust geflushed
- [ ] Bounds-Check für Touch-Koordinaten

---

## Bug #13: HOCH-06 - Uninitialized Variables

### Problem
`src/ui/screens/WiFiSettingsScreen.cpp`: Struct Members nicht initialisiert.

### GLM Fix-Anleitung

**Schritt 1:** Finde alle Structs
```bash
grep -rn "struct " src/ include/
```

**Schritt 2:** Prüfe auf fehlende Initialisierung
```cpp
// FALSCH:
struct NetworkInfo {
    String ssid;
    int rssi;           // ← Undefiniert!
    bool secured;       // ← Undefiniert!
    bool connected;     // ← Undefiniert!
};

// RICHTIG:
struct NetworkInfo {
    String ssid = "";
    int rssi = 0;
    bool secured = false;
    bool connected = false;
};
```

**Schritt 3:** Alle Structs im Projekt fixen
```cpp
// TrackInfo (falls vorhanden):
struct TrackInfo {
    String title = "";
    String artist = "";
    String album = "";
    String coverUrl = "";
    unsigned long duration = 0;
    unsigned long progress = 0;
    bool isPlaying = false;
};

// TouchPoint:
struct TouchPoint {
    int x = 0;
    int y = 0;
    bool touched = false;
};

// Config structs:
struct WiFiConfig {
    String ssid = "";
    String password = "";
    bool autoConnect = true;
};
```

**Schritt 4:** Konstruktor Alternative
```cpp
struct NetworkInfo {
    String ssid;
    int rssi;
    bool secured;
    bool connected;
    
    // Default constructor mit Initialisierung
    NetworkInfo() : ssid(""), rssi(0), secured(false), connected(false) {}
    
    // Convenience constructor
    NetworkInfo(const String& s, int r, bool sec)
        : ssid(s), rssi(r), secured(sec), connected(false) {}
};
```

### Quality Check
- [ ] Alle Structs haben Default-Werte
- [ ] Keine uninitialisierten bool/int Members
- [ ] Compiler-Warnings für uninitialized aktiviert (-Wuninitialized)

---

## Bug #14: HOCH-08 - Missing volatile für ISR

### Problem
`include/touch_manager.h`: Variable die von ISR gelesen wird fehlt `volatile`.

### GLM Fix-Anleitung

**Schritt 1:** Finde ISR und geteilte Variablen
```bash
grep -rn "IRAM_ATTR" src/ include/
grep -rn "attachInterrupt" src/
```

**Schritt 2:** Identifiziere geteilte Variablen
```cpp
// Wenn ISR eine Variable setzt/liest die auch von main loop verwendet wird:
class TouchManager {
private:
    bool touchDetected;  // ← FEHLT volatile!
    
    static void IRAM_ATTR touchISR() {
        touchDetected = true;  // ISR schreibt
    }
    
    void loop() {
        if (touchDetected) {  // Main loop liest
            // ...
        }
    }
};
```

**Schritt 3:** volatile hinzufügen
```cpp
class TouchManager {
private:
    volatile bool touchDetected = false;  // ← volatile für ISR-Zugriff!
    volatile unsigned long lastTouchTime = 0;
    
    static TouchManager* instance;  // Für ISR-Zugriff
    
    static void IRAM_ATTR touchISR() {
        if (instance) {
            instance->touchDetected = true;
            instance->lastTouchTime = millis();
        }
    }
    
public:
    bool hasPendingTouch() {
        bool result = touchDetected;
        touchDetected = false;  // Atomic auf ESP32 für bool
        return result;
    }
};
```

**Schritt 4:** Für komplexere Daten - Mutex verwenden
```cpp
// Für größere Strukturen die atomar sein müssen:
#include <freertos/semphr.h>

class SafeData {
private:
    volatile TouchPoint lastTouch;
    SemaphoreHandle_t mutex;
    
public:
    SafeData() {
        mutex = xSemaphoreCreateMutex();
    }
    
    void setFromISR(const TouchPoint& p) {
        // ISR-safe version
        BaseType_t higher;
        if (xSemaphoreTakeFromISR(mutex, &higher) == pdTRUE) {
            lastTouch = p;
            xSemaphoreGiveFromISR(mutex, &higher);
        }
    }
    
    TouchPoint get() {
        TouchPoint result;
        xSemaphoreTake(mutex, portMAX_DELAY);
        result = lastTouch;
        xSemaphoreGive(mutex);
        return result;
    }
};
```

### Quality Check
- [ ] Alle ISR-geteilten Variablen sind volatile
- [ ] Keine komplexen Operationen in ISR
- [ ] IRAM_ATTR für alle ISR-Funktionen
- [ ] Keine String-Operationen in ISR

---

## Bug #15: HOCH-02 - Touch Handler Bounds Check

### Problem
`src/main_uart.cpp` Zeile ~250: Touch-Koordinaten werden nicht validiert.

### GLM Fix-Anleitung

**Schritt 1:** Finde Touch-Handler
```bash
grep -rn "handleTouch" src/
grep -rn "uint16_t x, uint16_t y" src/
```

**Schritt 2:** Füge Bounds Check hinzu
```cpp
// VORHER:
void handleTouchPoint(uint16_t x, uint16_t y) {
    if (x >= 25 && x <= 75 && y >= 365 && y <= 415) {
        // Button hit
    }
}

// NACHHER:
void handleTouchPoint(uint16_t x, uint16_t y) {
    // Bounds validation
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        Serial.printf("⚠️ Touch out of bounds: %u,%u\n", x, y);
        return;
    }
    
    // Jetzt sicher mit Koordinaten arbeiten
    if (x >= 25 && x <= 75 && y >= 365 && y <= 415) {
        // Button hit
    }
}
```

**Schritt 3:** Zentrale Validation-Funktion
```cpp
// In utils.h oder touch_manager.h:
inline bool isValidTouchPoint(uint16_t x, uint16_t y) {
    return (x < DISPLAY_WIDTH) && (y < DISPLAY_HEIGHT);
}

// Verwendung überall:
void onTouch(uint16_t x, uint16_t y) {
    if (!isValidTouchPoint(x, y)) return;
    // ...
}
```

**Schritt 4:** Display-Konstanten definieren (falls nicht vorhanden)
```cpp
// In config.h:
#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 480
```

### Quality Check
- [ ] Bounds Check am Anfang jeder Touch-Funktion
- [ ] DISPLAY_WIDTH/HEIGHT Konstanten definiert
- [ ] Kein Crash bei Out-of-Bounds Touch

---

## Bug #16: HOCH-04 - Fehlende Error Propagation

### Problem
Mehrere Dateien: Fehler werden nicht nach oben propagiert.

### GLM Fix-Anleitung

**Schritt 1:** Error Enum definieren
```cpp
// In include/errors.h:
#pragma once

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
    UNKNOWN
};

const char* errorToString(SpotifyError err) {
    switch (err) {
        case SpotifyError::OK: return "OK";
        case SpotifyError::NETWORK_ERROR: return "Network error";
        case SpotifyError::AUTH_EXPIRED: return "Auth expired";
        case SpotifyError::AUTH_FAILED: return "Auth failed";
        case SpotifyError::RATE_LIMITED: return "Rate limited";
        case SpotifyError::API_ERROR: return "API error";
        case SpotifyError::PARSE_ERROR: return "Parse error";
        case SpotifyError::NOT_PLAYING: return "Nothing playing";
        case SpotifyError::DEVICE_NOT_FOUND: return "Device not found";
        default: return "Unknown error";
    }
}
```

**Schritt 2:** Result Template (optional aber empfohlen)
```cpp
// In include/result.h:
template<typename T>
struct Result {
    T value;
    SpotifyError error;
    String message;
    
    bool ok() const { return error == SpotifyError::OK; }
    
    static Result<T> success(T val) {
        return {val, SpotifyError::OK, ""};
    }
    
    static Result<T> failure(SpotifyError err, const String& msg = "") {
        return {T{}, err, msg};
    }
};

// Spezialisierung für void-ähnliche Operationen:
struct Status {
    SpotifyError error;
    String message;
    
    bool ok() const { return error == SpotifyError::OK; }
    
    static Status success() {
        return {SpotifyError::OK, ""};
    }
    
    static Status failure(SpotifyError err, const String& msg = "") {
        return {err, msg};
    }
};
```

**Schritt 3:** SpotifyClient Methoden aktualisieren
```cpp
// VORHER:
bool SpotifyClient::play() {
    if (!ensureValidToken()) return false;
    return httpPut("/me/player/play");
}

// NACHHER:
Status SpotifyClient::play() {
    if (!ensureValidToken()) {
        return Status::failure(SpotifyError::AUTH_EXPIRED, "Token refresh failed");
    }
    
    auto result = httpPut("/me/player/play");
    if (!result.ok()) {
        return Status::failure(result.error, "Play command failed");
    }
    
    return Status::success();
}

// Aufruf:
auto status = spotify->play();
if (!status.ok()) {
    showError(errorToString(status.error));
    Serial.printf("❌ Play failed: %s\n", status.message.c_str());
}
```

**Schritt 4:** HTTP Methoden mit Error Details
```cpp
struct HttpResult {
    int statusCode;
    String body;
    SpotifyError error;
    
    bool ok() const { return statusCode >= 200 && statusCode < 300; }
};

HttpResult SpotifyClient::httpGet(const String& endpoint) {
    HttpResult result = {0, "", SpotifyError::OK};
    
    // ... HTTP setup ...
    
    int code = http.GET();
    result.statusCode = code;
    
    if (code < 0) {
        result.error = SpotifyError::NETWORK_ERROR;
        return result;
    }
    
    if (code == 401) {
        result.error = SpotifyError::AUTH_EXPIRED;
        return result;
    }
    
    if (code == 429) {
        result.error = SpotifyError::RATE_LIMITED;
        return result;
    }
    
    if (code >= 400) {
        result.error = SpotifyError::API_ERROR;
        return result;
    }
    
    result.body = http.getString();
    return result;
}
```

### Quality Check
- [ ] SpotifyError Enum existiert
- [ ] Methoden geben Status/Result zurück statt bool
- [ ] Fehler werden geloggt mit Details
- [ ] UI zeigt aussagekräftige Fehlermeldungen

---

## Bug #17: HOCH-01 - String Memory Fragmentation

### Problem
Mehrere Dateien: String-Konkatenation ohne `reserve()` führt zu Memory Fragmentation.

### GLM Fix-Anleitung

**Schritt 1:** Finde String-Konkatenationen
```bash
grep -rn "+=" src/*.cpp | grep "String"
grep -rn "url +=" src/
grep -rn "String url" src/
```

**Schritt 2:** Pattern identifizieren
```cpp
// SCHLECHT (viele Reallokationen):
String url = SPOTIFY_AUTH_URL;
url += "?client_id=";
url += clientId;
url += "&response_type=code";
url += "&redirect_uri=";
url += redirectUri;
// Jedes += kann reallokieren → Fragmentation!
```

**Schritt 3:** Mit reserve() optimieren
```cpp
// GUT:
String url;
url.reserve(512);  // Einmal allokieren für erwartete Größe
url = SPOTIFY_AUTH_URL;
url += "?client_id=";
url += clientId;
url += "&response_type=code";
url += "&redirect_uri=";
url += redirectUri;
```

**Schritt 4:** Größen-Schätzungen
```cpp
// Typische URL-Längen:
// Auth URL: ~300-500 Zeichen → reserve(512)
// API Endpoint: ~100-200 Zeichen → reserve(256)
// JSON Body: ~200-1000 Zeichen → reserve(1024)

// Helper-Funktion:
String buildAuthUrl(const String& clientId, const String& redirectUri) {
    String url;
    url.reserve(512);
    
    url += SPOTIFY_AUTH_URL;
    url += "?client_id=";
    url += clientId;
    url += "&response_type=code";
    url += "&redirect_uri=";
    url += urlEncode(redirectUri);
    url += "&scope=";
    url += SPOTIFY_SCOPES;
    
    return url;  // Move semantics in C++11
}
```

**Schritt 5:** Alternative - snprintf für konstante Strings
```cpp
// Für bekannte Formate - sehr effizient:
char url[512];
snprintf(url, sizeof(url), 
    "%s?client_id=%s&response_type=code&redirect_uri=%s",
    SPOTIFY_AUTH_URL, clientId.c_str(), redirectUri.c_str());

// Dann bei Bedarf zu String konvertieren:
String urlString(url);
```

**Schritt 6:** Dateien zu prüfen
```bash
# Diese Dateien haben wahrscheinlich String-Konkatenation:
src/spotify/SpotifyClient.cpp
src/spotify/AuthManager.cpp
src/network/WiFiManager.cpp
src/utils.cpp
```

### Quality Check
- [ ] Alle String-Konkatenationen haben reserve()
- [ ] Heap-Fragmentierung reduziert (ESP.getMaxAllocHeap() monitoren)
- [ ] Keine "String too long" Errors
- [ ] Memory-Verbrauch stabil über Zeit

---

# ✅ QUALITY GATE CHECKLIST

Opus reviewed jeden Fix mit dieser Checklist:

## Für JEDEN Bug-Fix prüfen:

### Compilation
- [ ] Kompiliert ohne Errors
- [ ] Keine neuen Warnings
- [ ] Alle Includes vorhanden

### Funktionalität  
- [ ] Bug ist tatsächlich behoben
- [ ] Keine Regression in verwandter Funktionalität
- [ ] Edge Cases behandelt

### Code Quality
- [ ] Konsistenter Code-Style
- [ ] Sinnvolle Variablennamen
- [ ] Kommentare wo nötig

### Memory Safety
- [ ] Keine neuen Memory Leaks
- [ ] Alle Pointer nullptr-initialisiert
- [ ] RAII wo anwendbar

### ESP32 Specific
- [ ] yield() in langen Loops
- [ ] Watchdog-freundlich
- [ ] Stack-Nutzung angemessen

## Nach ALLEN Fixes:

### Integration Test
- [ ] ESP32 bootet
- [ ] WiFi verbindet
- [ ] Spotify Auth funktioniert
- [ ] Playback Control funktioniert
- [ ] Touch funktioniert
- [ ] Screens wechseln ohne Crash
- [ ] 1h Dauertest ohne Memory-Leak

### Performance Check
- [ ] `ESP.getFreeHeap()` stabil
- [ ] `ESP.getMaxAllocHeap()` nicht degeneriert
- [ ] UI reagiert flüssig

---

# 📅 ZEITPLAN

| Batch | Bugs | Zeit | Cumulative |
|-------|------|------|------------|
| A: Quick Wins | #1, #4, #5 | 25 min | 25 min |
| B: Events | #2, #3 | 35 min | 1h |
| C: Memory | #6, #7, #8, #9 | 75 min | 2h 15min |
| D: Stability | #10-#15 | 95 min | 3h 50min |
| E: Quality | #16, #17 | 105 min | 5h 35min |
| Testing | All | 30 min | ~6h |

---

*Strategie erstellt von Claude Opus am 12.02.2026*
