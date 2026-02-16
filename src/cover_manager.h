/**
 * @file cover_manager.h
 * @brief Cover Image Download and Cache Manager
 *
 * Handles downloading, caching, and resizing Spotify cover images.
 * Optimized for ESP32 with limited memory and storage.
 *
 * Features:
 * - JPEG/PNG download from Spotify URLs
 * - LittleFS caching with size limits
 * - Automatic cache cleanup
 * - Image resizing for display
 * - Memory-efficient loading
 */

#ifndef COVER_MANAGER_H
#define COVER_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "models.h"

// Cache configuration
#define COVER_CACHE_DIR "/covers"
#define COVER_CACHE_MAX_SIZE 5000000      // 5MB max cache size
#define COVER_CACHE_MAX_FILES 50          // Max files in cache
#define COVER_CACHE_MAX_AGE_MS 604800000  // 7 days (1 week)
#define COVER_CACHE_DEFAULT_NAME "cover.jpg"

// Display sizes
#define COVER_SIZE_SMALL 64              // 64x64 thumbnail
#define COVER_SIZE_MEDIUM 300            // 300x300 standard
#define COVER_SIZE_LARGE 480             // 480x480 full display

// Image formats
enum class ImageFormat {
    UNKNOWN,
    JPEG,
    PNG
};

/**
 * @brief Cover Image Download Result
 */
struct CoverResult {
    bool success;
    String filePath;
    ImageFormat format;
    int width;
    int height;
    size_t fileSize;
    String errorMessage;
    
    CoverResult()
        : success(false)
        , format(ImageFormat::UNKNOWN)
        , width(0)
        , height(0)
        , fileSize(0) {
    }
};

/**
 * @brief Cover Manager Class
 *
 * Manages cover image download, caching, and resizing.
 */
class CoverManager {
public:
    CoverManager();
    ~CoverManager();
    
    /**
     * @brief Initialize the cover manager
     * @return true if successful
     */
    bool init();
    
    /**
     * @brief Check if LittleFS is mounted
     */
    bool isReady() const { return littleFsReady; }
    
    /**
     * @brief Get cover image for a track
     * 
     * This will:
     * 1. Check cache for existing cover
     * 2. Download if not cached
     * 3. Cache the downloaded image
     * 4. Return file path
     * 
     * @param trackInfo Track information
     * @param targetSize Target display size (Small, Medium, Large)
     * @return CoverResult with file path or error
     */
    CoverResult getCover(const spotify::TrackInfo& trackInfo, int targetSize = COVER_SIZE_LARGE);
    
    /**
     * @brief Get cover image from URL
     * 
     * @param imageUrl URL of the cover image
     * @param trackId Track ID for cache key
     * @param targetSize Target display size
     * @return CoverResult with file path or error
     */
    CoverResult getCover(const String& imageUrl, const String& trackId, int targetSize = COVER_SIZE_LARGE);
    
    /**
     * @brief Download and cache cover image
     * 
     * @param imageUrl URL to download from
     * @param trackId Track ID for cache key
     * @param targetSize Target size for resizing
     * @return CoverResult with file path or error
     */
    CoverResult downloadCover(const String& imageUrl, const String& trackId, int targetSize = COVER_SIZE_LARGE);
    
    /**
     * @brief Check if cover is cached
     * 
     * @param trackId Track ID to check
     * @return true if cached and valid
     */
    bool isCoverCached(const String& trackId);
    
    /**
     * @brief Get cached cover file path
     * 
     * @param trackId Track ID
     * @return File path or empty string if not cached
     */
    String getCachedCoverPath(const String& trackId);
    
    /**
     * @brief Delete cached cover for track
     * 
     * @param trackId Track ID
     * @return true if deleted
     */
    bool deleteCachedCover(const String& trackId);
    
    /**
     * @brief Clear entire cover cache
     * 
     * @return Number of files deleted
     */
    int clearCache();
    
    /**
     * @brief Clean up expired cache entries
     * 
     * @return Number of files deleted
     */
    int cleanupCache();
    
    /**
     * @brief Get cache statistics
     */
    struct CacheStats {
        int fileCount;
        size_t totalSize;
        size_t totalCapacity;
        int expiredCount;
    };
    CacheStats getCacheStats();
    
    /**
     * @brief Preload cache metadata
     * 
     * Loads cache index on startup for faster lookups.
     * 
     * @return true if successful
     */
    bool loadCacheIndex();
    
    /**
     * @brief Save cache index to disk
     * 
     * @return true if successful
     */
    bool saveCacheIndex();
    
    /**
     * @brief Check if image format is supported
     * 
     * @param url Image URL
     * @return ImageFormat enum
     */
    ImageFormat detectImageFormat(const String& url);
    
    /**
     * @brief Get target cover URL based on size
     * 
     * @param trackInfo Track information
     * @param targetSize Desired size
     * @return Best URL for the requested size
     */
    String getBestCoverUrl(const spotify::TrackInfo& trackInfo, int targetSize);
    
    /**
     * @brief Resize image to target dimensions
     * 
     * Note: This is a placeholder. Full implementation requires
     * image decoder/encoder libraries (JPEG, PNG).
     * 
     * @param sourcePath Source image path
     * @param targetPath Target image path
     * @param targetWidth Target width
     * @param targetHeight Target height
     * @return true if successful
     */
    bool resizeImage(const String& sourcePath, const String& targetPath, 
                     int targetWidth, int targetHeight);
    
    /**
     * @brief Load image into memory buffer
     * 
     * Loads image file into a memory buffer for LVGL display.
     * 
     * @param filePath Path to image file
     * @param buffer Output buffer
     * @param bufferSize Buffer size
     * @return Number of bytes read, or -1 on error
     */
    int loadImageToBuffer(const String& filePath, uint8_t* buffer, size_t bufferSize);

private:
    /**
     * @brief Generate cache file path from track ID
     */
    String getCachePath(const String& trackId);
    
    /**
     * @brief Get cache index file path
     */
    String getCacheIndexPath() { return COVER_CACHE_DIR "/index.json"; }
    
    /**
     * @brief Download image from URL to file
     */
    bool downloadImage(const String& url, const String& filePath);
    
    /**
     * @brief Read image dimensions from file header
     */
    bool readImageDimensions(const String& filePath, int& width, int& height, ImageFormat& format);
    
    /**
     * @brief Update cache index with new entry
     */
    bool updateCacheIndex(const String& trackId, const String& url, size_t fileSize);
    
    /**
     * @brief Remove entry from cache index
     */
    bool removeFromCacheIndex(const String& trackId);
    
    /**
     * @brief Make space in cache if needed
     */
    bool ensureCacheSpace(size_t requiredSpace);
    
    /**
     * @brief Delete oldest cache entry
     */
    bool deleteOldestCacheEntry();
    
    /**
     * @brief Get file size
     */
    size_t getFileSize(const String& filePath);
    
    /**
     * @brief Delete file if exists
     */
    bool deleteFile(const String& filePath);
    
    // Cache index
    struct CacheEntry {
        String trackId;
        String url;
        unsigned long timestamp;
        size_t fileSize;
        int accessCount;
    };
    std::vector<CacheEntry> cacheIndex;
    
    // State
    bool littleFsReady;
    unsigned long lastCleanupTime;
    static constexpr unsigned long CLEANUP_INTERVAL_MS = 3600000; // 1 hour
    
    // HTTP client
    WiFiClientSecure client;
    HTTPClient http;
};

#endif // COVER_MANAGER_H
