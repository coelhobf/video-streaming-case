#!/bin/bash

# Duration in seconds (default 300s = 5 minutes, parameterizable)
DURATION=${1:-300}
OUTPUT_DIR="media"
OUTPUT_FILE="$OUTPUT_DIR/sample.mp4"

mkdir -p "$OUTPUT_DIR"

ffmpeg \
    -f lavfi \
    -i "testsrc=duration=$DURATION:size=1920x1080:rate=30" \
    -c:v libx264 \
    -preset fast \
    -y "$OUTPUT_FILE"