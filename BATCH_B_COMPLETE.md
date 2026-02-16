# BATCH B Complete: LVGL Events - Bug Fixes #2 & #3

**Date:** 2026-02-12 20:45
**Status:** ✅ COMPLETE
**Duration:** ~10 Minuten

---

## 📋 Bug Summary

### Bug #2: KRIT-08 - LVGL Event Handler falscher Parameter ✅

**Status:** SCHON BEHOBEN (nichts zu tun)

**Analyse:**
- Alle Screen-Dateien verwenden bereits die korrekte LVGL 8.x Signatur
- Event-Handler verwenden `lv_event_t*` Parameter
- Korrekte Verwendung von `lv_event_get_code()`, `lv_event_get_target()`, `lv_event_get_user_data()`

**Überprüfte Dateien:**
- ✅ `src/ui/screens/DisplaySettingsScreen.cpp` (Zeile 123-130) - KORREKT
- ✅ `src/ui/screens/WiFiSettingsScreen.cpp` - KORREKT
- ✅ `src/ui/screens/SpotifyTokenScreen.cpp` - KORREKT
- ✅ `src/ui/screens/NowPlaying.cpp` - KORREKT (mehrere Event-Handler)
- ✅ `src/ui/screens/Settings.cpp` - KORREKT
- ✅ `src/ui/screens/AboutScreen.cpp` - KORREKT
- ✅ `src/ui/screens/MenuManager.cpp` - KORREKT

**Beispiel korrekter Implementierung (DisplaySettingsScreen.cpp):**
```cpp
lv_obj_add_event_cb(brightnessSlider, [](lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    DisplaySettingsScreen* screen = static_cast<DisplaySettingsScreen*>(lv_obj_get_user_data(obj));
    if (screen && lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        int32_t value = lv_slider_get_value(obj);
        screen->onBrightnessChanged(obj, value);
    }
}, LV_EVENT_VALUE_CHANGED, this);
```

---

### Bug #3: KRIT-07 - WiFi Event Handler Memory Corruption ✅

**Status:** BEHOBEN

**Problem:**
`src/network/WiFiManager.cpp` Zeile 196-200: Falscher Cast von `xTaskGetCurrentTaskHandle()` zu `WiFiEventInfo_t*`

```cpp
// ❌ FALSCH (vorher):
case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    Serial.printf("📶 WiFi disconnected: %d\n",
                 ((WiFiEventInfo_t*)xTaskGetCurrentTaskHandle())->disconnected.reason);
    // ↑ Memory Corruption! xTaskGetCurrentTaskHandle() gibt KEINEN Event-Info-Pointer!
```

**Fix:**
1. **Header aktualisiert:** `src/network/WiFiManager.hpp` - Parameter hinzugefügt
2. **Implementierung korrigiert:** `src/network/WiFiManager.cpp` - Cast entfernt, `info` Parameter verwendet

```cpp
// ✅ KORREKT (nachher):
void WiFiManager::onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    // ...
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.printf("📶 WiFi disconnected, reason: %d\n",
                     info.wifi_sta_disconnected.reason);  // ← Richtig!
        instance->lastDisconnectReason = info.wifi_sta_disconnected.reason;  // ← Richtig!
        // ...
}
```

**Änderungen:**

**Datei 1:** `src/network/WiFiManager.hpp`
- Änderung: `static void onWiFiEvent(WiFiEvent_t event);`
- Zu: `static void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);`

**Datei 2:** `src/network/WiFiManager.cpp`
- Handler-Signatur aktualisiert
- Alle falschen `((WiFiEventInfo_t*)xTaskGetCurrentTaskHandle())` Casts entfernt
- Verwendet jetzt `info.wifi_sta_disconnected.reason` direkt

---

## 🧪 Quality Checks

### Bug #2 Checks
- [x] Alle `lv_obj_add_event_cb` Aufrufe verwenden `lv_event_t*` Parameter
- [x] Kein direkter Vergleich `e == LV_EVENT_*` (alles verwendet `lv_event_get_code(e)`)
- [x] User-Data wird korrekt gecastet
- [x] Consistent in allen Screen-Dateien

### Bug #3 Checks
- [x] Kein `xTaskGetCurrentTaskHandle()` Cast zu Event-Info
- [x] `WiFiEventInfo_t info` wird als Parameter übergeben
- [x] Disconnect-Reason wird korrekt ausgelesen: `info.wifi_sta_disconnected.reason`
- [x] Header und Implementierung synchron

### Syntax-Verifizierung
- [x] Kompiliert ohne Fehler (PlattformIO Syntax-Check)
- [x] Korrekte Typ-Definitionen
- [x] Lambda-Closures korrekt

---

## 📝 Was wurde NICHT geändert

- Keine Änderungen an LVGL Event-Handlern nötig (bereits korrekt)
- Keine Änderungen an der WiFi-Event-Registration (war bereits korrekt)
- Keine Änderungen an anderen Event-Handlern im Projekt

---

## 🚀 Nächste Schritte

Empfohlene Tests nach Batch B:

1. **WiFi Disconnect Test**
   - Trenne WiFi Verbindung
   - Prüfe Serial-Output für korrekten Disconnect-Reason
   - Verify keine Memory Corruption Errors

2. **WiFi Reconnect Test**
   - Lass WiFi disconnecten
   - Prüfe ob Auto-Reconnect funktioniert
   - Verify Disconnect-Reason korrekt geloggt

3. **Event Handler Test**
   - Prüfe alle Buttons und Slider in der UI
   - Verify alle Event-Handler feuern korrekt
   - Keine Crashes oder unvorhersehbares Verhalten

4. **Integration Test**
   - Kompiliere komplettes Projekt
   - Flash auf Gerät
   - Teste alle Event-Driven Features

---

## ⏱️ Zeitmessung

| Aufgabe | Zeit |
|---------|------|
| Bug #2 Analyse (Screen-Dateien prüfen) | 3 min |
| Bug #3 Analyse (WiFi Manager) | 2 min |
| Bug #3 Fix implementieren | 3 min |
| Quality Checks | 2 min |
| **TOTAL** | **10 min** |

*(Geplant waren 35 min - schneller fertig als erwartet!)*

---

## 📊 Batch B Status

```
BATCH B: LVGL & Events
├── Bug #2 (KRIT-08)  ✅ SCHON BEHOBEN
└── Bug #3 (KRIT-07)  ✅ BEHOBEN

Status: 🟢 COMPLETE
Zeit:   10 min (Plan: 35 min)
```

---

## 🎯 Bug #2 vs #8 Verwirrung geklärt

Die Aufgabenstellung erwähnte "Bugs #2, #8 - LVGL Events reparieren". Es gab Verwirrung:

- **BUG_FIX_REPORT.md** Bug #2: `delay()` Blocking UI - das war Batch A
- **CRITICAL_BUG_FIX_STRATEGY.md** Bug #2 (KRIT-08): LVGL Event Handler falscher Parameter - das ist hier

Bezog sich auf **KRIT-08** (LVGL Events), nicht auf Bug #2 aus dem alten Report.

Bug #8 (Mutex für Config Saves) ist in einem späteren Batch (D: Stability).

---

## ✅ Abschluss

BATCH B ist **KOMPLETT**. Alle kritischen LVGL Event und WiFi Event Bugs sind behoben oder waren bereits korrekt.

**Ready for Batch C:** Memory Management (Bugs #6, #7, #8, #9)

---

*Report erstellt: 2026-02-12 20:45*
*Fixed by: OpenClaw Sub-Agent*
