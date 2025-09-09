#!/bin/bash
# Health check for RTSP server

# Simple process check - if pipeline-rtsp is running, consider it healthy
if pgrep -f "pipeline-rtsp" > /dev/null; then
    exit 0
else
    exit 1
fi
