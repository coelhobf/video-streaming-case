#!/bin/bash
set -e

echo "Starting Paladium RTSP Server..."

if [ ! -f "media/sample.mp4" ]; then
    echo "Video file not found, creating test video..."
    cd pipeline-rtsp
    ./create_test_video.sh
    cd ..
fi

if [ ! -f "pipeline-rtsp/pipeline-rtsp" ]; then
    echo "Building pipeline..."
    cd pipeline-rtsp && make && cd ..
fi

cd pipeline-rtsp
echo "Starting RTSP server..."
./pipeline-rtsp
