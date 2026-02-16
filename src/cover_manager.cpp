/**
 * @file cover_manager.cpp
 * @brief Cover Image Download and Cache Manager Implementation
 */

#include "cover_manager.h"
#include <LittleFS.h>

CoverManager::CoverManager()
    : littleFsReady(false)
    , lastCleanupTime(0) {
}

CoverManager::~CoverManager() {
    if (http.connected()) {
        http.end();
    }
}

bool CoverManager::init() {
    Serial.println("🖼️  Initializing CoverManager...");
    
    // Mount LittleFS
    if (!LittleFS.begin(true)) { // true = format if needed
        Serial.println("❌ Failed to mount LittleFS");
        return false;
    }
    
    littleFsReady = true;
    
    // Create cache directory
    if (!LittleFS.exists(COVER_CACHE_DIR)) {
        Serial.println("📁 Creating cache directory...");
        LittleFS.mkdir(COVER_CACHE_DIR);
    }
    
    // Load cache index
    loadCacheIndex();
    
    // Perform initial cleanup
    cleanupCache();
    
    // Print stats
    CacheStats stats = getCacheStats();
    Serial.printf("✅ CoverManager ready: %d files, %d KB\n", 
                  stats.fileCount, stats.totalSize / 1024);
    
    return true;
}

CoverResult CoverManager::getCover(const spotify::TrackInfo& trackInfo, int targetSize) {
    if (trackInfo.id.isEmpty() || !trackInfo.isValid()) {
        CoverResult result;
        result.success = false;
        result.errorMessage = "Invalid track info";
        return result;
    }
    
    return getCover(trackInfo.getBestCoverUrl(), trackInfo.id, targetSize);
}

CoverResult CoverManager::getCover(const String& imageUrl, const String& trackId, int targetSize) {
    CoverResult result;
    
    if (!littleFsReady) {
        result.errorMessage = "LittleFS not ready";
        return result;
    }
    
    // Check if URL is valid
    if (imageUrl.isEmpty()) {
        result.errorMessage = "Empty image URL";
        return result;
    }
    
    // Check cache first
    String cachedPath = getCachedCoverPath(trackId);
    if (!cachedPath.isEmpty()) {
        Serial.printf("📦 Cache hit: %s\n", trackId.c_str());
        
        result.success = true;
        result.filePath = cachedPath;
        result.fileSize = getFileSize(cachedPath);
        
        // Read dimensions
        readImageDimensions(cachedPath, result.width, result.height, result.format);
        
        // Update access count in index
        for (auto& entry : cacheIndex) {
            if (entry.trackId == trackId) {
                entry.accessCount++;
                break;
            }
        }
        
        return result;
    }
    
    // Not cached, download it
    Serial.printf("📥 Cache miss: %s, downloading...\n", trackId.c_str());
    return downloadCover(imageUrl, trackId, targetSize);
}

CoverResult CoverManager::downloadCover(const String& imageUrl, const String& trackId, int targetSize) {
    CoverResult result;
    
    if (!littleFsReady) {
        result.errorMessage = "LittleFS not ready";
        return result;
    }
    
    // Generate cache path
    String cachePath = getCachePath(trackId);
    
    Serial.printf("🌐 Downloading: %s\n", imageUrl.c_str());
    Serial.printf("   To: %s\n", cachePath.c_str());
    
    // Download image
    if (!downloadImage(imageUrl, cachePath)) {
        result.errorMessage = "Download failed";
        return result;
    }
    
    // Get file size
    size_t fileSize = getFileSize(cachePath);
    if (fileSize == 0) {
        deleteFile(cachePath);
        result.errorMessage = "Empty file";
        return result;
    }
    
    // Detect format
    result.format = detectImageFormat(imageUrl);
    
    // Read dimensions
    readImageDimensions(cachePath, result.width, result.height, result.format);
    
    // Resize if needed (placeholder - requires image decoder)
    if (result.width > targetSize || result.height > targetSize) {
        Serial.printf("🔄 Image resize needed (placeholder)\n");
        // TODO: Implement actual resizing
        // Requires JPEG/PNG decoder libraries
    }
    
    // Update cache index
    updateCacheIndex(trackId, imageUrl, fileSize);
    
    // Success
    result.success = true;
    result.filePath = cachePath;
    result.fileSize = fileSize;
    
    Serial.printf("✅ Cover downloaded: %d bytes\n", fileSize);
    
    return result;
}

bool CoverManager::isCoverCached(const String& trackId) {
    return !getCachedCoverPath(trackId).isEmpty();
}

String CoverManager::getCachedCoverPath(const String& trackId) {
    if (!littleFsReady || trackId.isEmpty()) {
        return "";
    }
    
    // Check cache index
    for (const auto& entry : cacheIndex) {
        if (entry.trackId == trackId) {
            String path = getCachePath(trackId);
            if (LittleFS.exists(path)) {
                // Check if expired
                if (spotify::CoverCacheInfo().isExpired(COVER_CACHE_MAX_AGE_MS)) {
                    // Delete expired entry
                    deleteCachedCover(trackId);
                    return "";
                }
                return path;
            } else {
                // File doesn't exist, remove from index
                removeFromCacheIndex(trackId);
            }
        }
    }
    
    return "";
}

bool CoverManager::deleteCachedCover(const String& trackId) {
    String path = getCachePath(trackId);
    
    if (LittleFS.exists(path)) {
        LittleFS.remove(path);
        Serial.printf("🗑️  Deleted cache: %s\n", trackId.c_str());
    }
    
    return removeFromCacheIndex(trackId);
}

int CoverManager::clearCache() {
    int count = 0;
    
    if (!littleFsReady) {
        return 0;
    }
    
    // Delete all files in cache directory
    File dir = LittleFS.open(COVER_CACHE_DIR);
    if (!dir || !dir.isDirectory()) {
        return 0;
    }
    
    File file = dir.openNextFile();
    while (file) {
        String path = file.path();
        file.close();
        
        if (path.indexOf("/index.json") == -1) {
            LittleFS.remove(path);
            count++;
        }
        
        file = dir.openNextFile();
    }
    
    dir.close();
    
    // Clear index
    cacheIndex.clear();
    saveCacheIndex();
    
    Serial.printf("🗑️  Cleared cache: %d files\n", count);
    return count;
}

int CoverManager::cleanupCache() {
    int deletedCount = 0;
    unsigned long now = millis();
    
    // Periodic cleanup (every hour)
    if (now - lastCleanupTime < CLEANUP_INTERVAL_MS && lastCleanupTime > 0) {
        return 0;
    }
    
    if (!littleFsReady) {
        return 0;
    }
    
    Serial.println("🧹 Running cache cleanup...");
    
    // Delete expired entries
    auto it = cacheIndex.begin();
    while (it != cacheIndex.end()) {
        if (it->timestamp > 0 && (now - it->timestamp) > COVER_CACHE_MAX_AGE_MS) {
            String path = getCachePath(it->trackId);
            if (LittleFS.exists(path)) {
                LittleFS.remove(path);
                deletedCount++;
            }
            it = cacheIndex.erase(it);
        } else {
            ++it;
        }
    }
    
    // Ensure we don't exceed file count
    while (cacheIndex.size() > COVER_CACHE_MAX_FILES) {
        // Delete oldest entry
        if (deleteOldestCacheEntry()) {
            deletedCount++;
        } else {
            break;
        }
    }
    
    // Ensure we don't exceed size limit
    CacheStats stats = getCacheStats();
    while (stats.totalSize > COVER_CACHE_MAX_SIZE) {
        if (deleteOldestCacheEntry()) {
            deletedCount++;
            stats = getCacheStats();
        } else {
            break;
        }
    }
    
    lastCleanupTime = now;
    saveCacheIndex();
    
    Serial.printf("✅ Cleanup complete: %d files deleted\n", deletedCount);
    return deletedCount;
}

CoverManager::CacheStats CoverManager::getCacheStats() {
    CacheStats stats = {0, 0, COVER_CACHE_MAX_SIZE, 0};
    
    if (!littleFsReady) {
        return stats;
    }
    
    File dir = LittleFS.open(COVER_CACHE_DIR);
    if (!dir || !dir.isDirectory()) {
        return stats;
    }
    
    File file = dir.openNextFile();
    while (file) {
        size_t size = file.size();
        
        if (strstr(file.path(), "/index.json") == nullptr) {
            stats.fileCount++;
            stats.totalSize += size;
        }
        
        file.close();
        file = dir.openNextFile();
    }
    
    dir.close();
    
    // Check expired count
    unsigned long now = millis();
    for (const auto& entry : cacheIndex) {
        if (entry.timestamp > 0 && (now - entry.timestamp) > COVER_CACHE_MAX_AGE_MS) {
            stats.expiredCount++;
        }
    }
    
    return stats;
}

bool CoverManager::loadCacheIndex() {
    String indexPath = getCacheIndexPath();
    
    if (!LittleFS.exists(indexPath)) {
        Serial.println("📄 No cache index found");
        return true; // Not an error, just no index
    }
    
    File file = LittleFS.open(indexPath, "r");
    if (!file) {
        Serial.println("❌ Failed to open cache index");
        return false;
    }
    
    size_t size = file.size();
    if (size > 100000) { // 100KB limit
        Serial.println("⚠️  Cache index too large, clearing");
        file.close();
        LittleFS.remove(indexPath);
        return false;
    }
    
    // Read and parse JSON
    String content = file.readString();
    file.close();
    
    StaticJsonDocument<8192> doc;
    DeserializationError error = deserializeJson(doc, content);
    
    if (error) {
        Serial.printf("⚠️  Failed to parse cache index: %s\n", error.c_str());
        return false;
    }
    
    // Clear existing index
    cacheIndex.clear();
    
    // Parse entries
    JsonArray entries = doc["entries"];
    for (JsonObject entry : entries) {
        CacheEntry cacheEntry;
        cacheEntry.trackId = entry["track_id"] | "";
        cacheEntry.url = entry["url"] | "";
        cacheEntry.timestamp = entry["timestamp"] | 0;
        cacheEntry.fileSize = entry["file_size"] | 0;
        cacheEntry.accessCount = entry["access_count"] | 0;
        
        if (!cacheEntry.trackId.isEmpty()) {
            cacheIndex.push_back(cacheEntry);
        }
    }
    
    Serial.printf("📄 Loaded %d cache entries\n", cacheIndex.size());
    return true;
}

bool CoverManager::saveCacheIndex() {
    String indexPath = getCacheIndexPath();
    
    // Create JSON document
    StaticJsonDocument<8192> doc;
    JsonArray entries = doc.createNestedArray("entries");
    
    for (const auto& entry : cacheIndex) {
        JsonObject entryObj = entries.createNestedObject();
        entryObj["track_id"] = entry.trackId;
        entryObj["url"] = entry.url;
        entryObj["timestamp"] = entry.timestamp;
        entryObj["file_size"] = entry.fileSize;
        entryObj["access_count"] = entry.accessCount;
    }
    
    // Serialize
    String content;
    serializeJson(doc, content);
    
    // Write to file
    File file = LittleFS.open(indexPath, "w");
    if (!file) {
        Serial.println("❌ Failed to create cache index");
        return false;
    }
    
    size_t written = file.print(content);
    file.close();
    
    if (written != content.length()) {
        Serial.println("❌ Failed to write cache index");
        return false;
    }
    
    return true;
}

ImageFormat CoverManager::detectImageFormat(const String& url) {
    String lowerUrl = url;
    lowerUrl.toLowerCase();
    
    if (lowerUrl.endsWith(".jpg") || lowerUrl.endsWith(".jpeg")) {
        return ImageFormat::JPEG;
    } else if (lowerUrl.endsWith(".png")) {
        return ImageFormat::PNG;
    }
    
    return ImageFormat::UNKNOWN;
}

String CoverManager::getBestCoverUrl(const spotify::TrackInfo& trackInfo, int targetSize) {
    // Select best URL based on target size
    if (targetSize >= COVER_SIZE_LARGE) {
        if (!trackInfo.coverUrlLarge.isEmpty()) return trackInfo.coverUrlLarge;
        if (!trackInfo.coverUrl.isEmpty()) return trackInfo.coverUrl;
    } else if (targetSize >= COVER_SIZE_MEDIUM) {
        if (!trackInfo.coverUrl.isEmpty()) return trackInfo.coverUrl;
        if (!trackInfo.coverUrlLarge.isEmpty()) return trackInfo.coverUrlLarge;
    } else {
        if (!trackInfo.coverUrlSmall.isEmpty()) return trackInfo.coverUrlSmall;
        if (!trackInfo.coverUrl.isEmpty()) return trackInfo.coverUrl;
    }
    
    return trackInfo.getBestCoverUrl();
}

bool CoverManager::resizeImage(const String& sourcePath, const String& targetPath,
                               int targetWidth, int targetHeight) {
    // TODO: Implement image resizing
    // This requires JPEG/PNG decoder/encoder libraries
    // For now, just copy the file
    Serial.printf("⚠️  Image resizing not implemented (copying file)\n");
    
    File source = LittleFS.open(sourcePath, "r");
    File target = LittleFS.open(targetPath, "w");
    
    if (!source || !target) {
        if (source) source.close();
        if (target) target.close();
        return false;
    }
    
    // Copy file
    uint8_t buffer[1024];
    size_t bytesRead;
    while ((bytesRead = source.read(buffer, sizeof(buffer))) > 0) {
        target.write(buffer, bytesRead);
    }
    
    source.close();
    target.close();
    
    return true;
}

int CoverManager::loadImageToBuffer(const String& filePath, uint8_t* buffer, size_t bufferSize) {
    if (!littleFsReady || !LittleFS.exists(filePath)) {
        return -1;
    }
    
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        return -1;
    }
    
    size_t fileSize = file.size();
    size_t toRead = (fileSize < bufferSize) ? fileSize : bufferSize;
    
    size_t bytesRead = file.read(buffer, toRead);
    file.close();
    
    return bytesRead;
}

// Private methods

String CoverManager::getCachePath(const String& trackId) {
    return String(COVER_CACHE_DIR) + "/" + trackId + ".jpg";
}

bool CoverManager::downloadImage(const String& url, const String& filePath) {
    // Configure HTTPS client
    client.setInsecure(); // Spotify's certificate should be added in production
    
    http.begin(client, url);
    http.addHeader("User-Agent", "SpotifyController/1.0");
    
    int httpCode = http.GET();
    
    if (httpCode != 200) {
        Serial.printf("⚠️  HTTP error: %d\n", httpCode);
        http.end();
        return false;
    }
    
    // Get content length
    int contentLength = http.getSize();
    if (contentLength <= 0 || contentLength > 10000000) { // 10MB limit
        Serial.printf("⚠️  Invalid content length: %d\n", contentLength);
        http.end();
        return false;
    }
    
    // Check if we have enough space
    ensureCacheSpace(contentLength + 10000); // +10KB buffer
    
    // Open file for writing
    File file = LittleFS.open(filePath, "w");
    if (!file) {
        Serial.println("❌ Failed to create file");
        http.end();
        return false;
    }
    
    // Download data
    WiFiClient* stream = http.getStreamPtr();
    uint8_t buffer[1024];
    int totalRead = 0;
    int bytesRead;
    
    while (http.connected() && (bytesRead = stream->readBytes(buffer, sizeof(buffer))) > 0) {
        file.write(buffer, bytesRead);
        totalRead += bytesRead;
        
        // Progress every 10KB
        if (totalRead % 10000 == 0) {
            Serial.printf("   Downloaded: %d/%d bytes (%d%%)\n",
                         totalRead, contentLength, (totalRead * 100) / contentLength);
        }
    }
    
    file.close();
    http.end();
    
    Serial.printf("✅ Download complete: %d bytes\n", totalRead);
    return true;
}

bool CoverManager::readImageDimensions(const String& filePath, int& width, int& height, ImageFormat& format) {
    // TODO: Implement image header parsing
    // For now, return placeholder values
    width = 640;
    height = 640;
    format = ImageFormat::JPEG;
    return true;
}

bool CoverManager::updateCacheIndex(const String& trackId, const String& url, size_t fileSize) {
    // Check if entry already exists
    for (auto& entry : cacheIndex) {
        if (entry.trackId == trackId) {
            entry.url = url;
            entry.timestamp = millis();
            entry.fileSize = fileSize;
            entry.accessCount = 1;
            saveCacheIndex();
            return true;
        }
    }
    
    // Add new entry
    CacheEntry entry;
    entry.trackId = trackId;
    entry.url = url;
    entry.timestamp = millis();
    entry.fileSize = fileSize;
    entry.accessCount = 1;
    cacheIndex.push_back(entry);
    
    saveCacheIndex();
    return true;
}

bool CoverManager::removeFromCacheIndex(const String& trackId) {
    for (auto it = cacheIndex.begin(); it != cacheIndex.end(); ++it) {
        if (it->trackId == trackId) {
            cacheIndex.erase(it);
            saveCacheIndex();
            return true;
        }
    }
    return false;
}

bool CoverManager::ensureCacheSpace(size_t requiredSpace) {
    CacheStats stats = getCacheStats();
    
    while (stats.totalSize + requiredSpace > COVER_CACHE_MAX_SIZE) {
        if (!deleteOldestCacheEntry()) {
            return false;
        }
        stats = getCacheStats();
    }
    
    return true;
}

bool CoverManager::deleteOldestCacheEntry() {
    if (cacheIndex.empty()) {
        return false;
    }
    
    // Find oldest entry
    auto oldest = cacheIndex.begin();
    for (auto it = cacheIndex.begin(); it != cacheIndex.end(); ++it) {
        if (it->timestamp < oldest->timestamp || oldest->timestamp == 0) {
            oldest = it;
        }
    }
    
    if (oldest != cacheIndex.end()) {
        String path = getCachePath(oldest->trackId);
        if (LittleFS.exists(path)) {
            LittleFS.remove(path);
        }
        cacheIndex.erase(oldest);
        saveCacheIndex();
        return true;
    }
    
    return false;
}

size_t CoverManager::getFileSize(const String& filePath) {
    if (!LittleFS.exists(filePath)) {
        return 0;
    }
    
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        return 0;
    }
    
    size_t size = file.size();
    file.close();
    
    return size;
}

bool CoverManager::deleteFile(const String& filePath) {
    if (LittleFS.exists(filePath)) {
        return LittleFS.remove(filePath);
    }
    return false;
}
