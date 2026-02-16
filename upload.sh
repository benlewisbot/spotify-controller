#!/bin/bash
# Spotify Controller - Quick Upload Script
# Builds and uploads firmware + filesystem to device

set -e

PROJECT_DIR="/home/tod/.openclaw/workspace/projects/spotify-controller"
cd "$PROJECT_DIR"

echo "========================================="
echo "  🎵 Spotify Controller - Upload Script"
echo "========================================="
echo ""

# Default environment
ENV="${1:-esp32-wrover}"

echo "🔨 Building firmware for: $ENV"
pio run -e "$ENV"

echo ""
echo "📤 Uploading firmware..."
pio run -e "$ENV" -t upload

echo ""
echo "📁 Uploading filesystem (config)..."
pio run -e "$ENV" -t uploadfs

echo ""
echo "✅ Upload complete!"
echo ""
echo "👀 Opening serial monitor..."
echo "   Press Ctrl+C to exit monitor"
echo ""

sleep 1
pio device monitor
