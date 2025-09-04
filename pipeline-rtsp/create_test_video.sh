#!/bin/bash

# Script to create a test MP4 video for the RTSP server
# This creates a simple test pattern video that can be used for testing

ASSETS_DIR="assets"
OUTPUT_FILE="$ASSETS_DIR/test.mp4"

echo "Creating test video for RTSP server..."

# Create assets directory if it doesn't exist
mkdir -p "$ASSETS_DIR"

# Check if ffmpeg is available
if command -v ffmpeg >/dev/null 2>&1; then
    echo "Using FFmpeg to create test video..."
    
    # Create a 30-second test video with color bars and moving pattern
    ffmpeg -f lavfi -i "testsrc=duration=30:size=640x480:rate=30" \
           -c:v libx264 -preset fast -pix_fmt yuv420p \
           -y "$OUTPUT_FILE"
    
    if [ $? -eq 0 ]; then
        echo "✅ Test video created successfully: $OUTPUT_FILE"
        echo "📊 Video info:"
        ffprobe -v quiet -show_format -show_streams "$OUTPUT_FILE" 2>/dev/null || echo "Run 'ffprobe $OUTPUT_FILE' to see video details"
    else
        echo "❌ Failed to create test video with FFmpeg"
        exit 1
    fi

elif command -v gst-launch-1.0 >/dev/null 2>&1; then
    echo "Using GStreamer to create test video..."
    
    # Create test video using GStreamer
    gst-launch-1.0 videotestsrc num-buffers=900 pattern=smpte \
        ! "video/x-raw,width=640,height=480,framerate=30/1" \
        ! x264enc preset=fast \
        ! mp4mux \
        ! filesink location="$OUTPUT_FILE"
    
    if [ $? -eq 0 ]; then
        echo "✅ Test video created successfully: $OUTPUT_FILE"
    else
        echo "❌ Failed to create test video with GStreamer"
        exit 1
    fi

else
    echo "⚠️  Neither FFmpeg nor GStreamer found."
    echo "Please install one of them to create a test video, or"
    echo "copy your own MP4 file to: $OUTPUT_FILE"
    echo ""
    echo "Install options:"
    echo "  macOS: brew install ffmpeg"
    echo "  Ubuntu/Debian: sudo apt install ffmpeg"
    echo ""
    exit 1
fi

echo ""
echo "🎬 Test video is ready!"
echo "📁 Location: $OUTPUT_FILE"
echo "🔗 You can now start the RTSP server with: ./build/rtsp_server"
echo ""