#!/bin/bash

# Duration in seconds (default 300s = 5 minutes, parameterizable)
DURATION=${1:-300}
OUTPUT_DIR="media"
OUTPUT_FILE="$OUTPUT_DIR/sample.mp4"

mkdir -p "$OUTPUT_DIR"

# Generate MP4 compatible with both macOS and Ubuntu GStreamer
ffmpeg \
    -f lavfi \
    -i "testsrc=duration=$DURATION:size=1920x1080:rate=30" \
    -c:v libx264 \
    -preset fast \
    -profile:v baseline \
    -level 3.1 \
    -pix_fmt yuv420p \
    -movflags +faststart \
    -y "$OUTPUT_FILE"

echo "Created MP4 video: $OUTPUT_FILE"
echo "Duration: $DURATION seconds, Resolution: 1920x1080, 30fps"