#!/bin/bash
set -e

echo "Setting up Paladium Video Pipeline dependencies for macOS..."

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "Error: Homebrew not found. Please install Homebrew first:"
    echo "   /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

echo "Installing GStreamer and dependencies..."
brew install \
    gstreamer \
    gst-plugins-base \
    gst-plugins-good \
    gst-plugins-bad \
    gst-rtsp-server

echo "Dependencies installed successfully!"