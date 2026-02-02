#!/bin/bash
# Spotify Controller - Quick Build Script
# For rapid development and testing

set -e

PROJECT_DIR="/home/tod/clawd/projects/spotify-controller"
cd "$PROJECT_DIR"

echo "========================================="
echo "  🎵 Spotify Controller - Build Script"
echo "========================================="
echo ""

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    echo "❌ PlatformIO not found!"
    echo "   Install with: pip install platformio"
    exit 1
fi

# Default environment
ENV="${1:-esp32-wrover}"

echo "🔧 Building for environment: $ENV"
echo ""

# Clean previous build
echo "🧹 Cleaning previous build..."
pio run -e "$ENV" -t clean

# Build
echo "🔨 Building..."
pio run -e "$ENV"

echo ""
echo "✅ Build complete!"
echo ""
echo "📤 To upload to device:"
echo "   pio run -e $ENV -t upload"
echo ""
echo "📁 To upload filesystem (config):"
echo "   pio run -e $ENV -t uploadfs"
echo ""
echo "👀 To monitor serial output:"
echo "   pio device monitor"
echo ""
echo "🚀 To do everything (build + upload + fs):"
echo "   ./upload.sh $ENV"
