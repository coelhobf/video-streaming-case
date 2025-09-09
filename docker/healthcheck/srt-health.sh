#!/bin/bash
# Health check for SRT relay

# Simple process check
if pgrep -f "pipeline-rtsp-to-srt" > /dev/null; then
    exit 0
else
    exit 1
fi
