#!/bin/bash
# Wait for all services to become healthy

set -e

MAX_RETRIES=30
RETRY_DELAY=2

services=("pipeline-rtsp" "pipeline-srt-relay" "mediamtx" "webui")

echo "Waiting for services to become healthy..."

for service in "${services[@]}"; do
    echo -n "Checking $service..."
    retries=0
    
    while [ $retries -lt $MAX_RETRIES ]; do
        # Check if service is healthy using docker-compose
        if docker-compose ps "$service" | grep -q "healthy\|Up"; then
            echo " Healthy"
            break
        fi
        
        echo -n "."
        sleep $RETRY_DELAY
        ((retries++))
    done
    
    if [ $retries -eq $MAX_RETRIES ]; then
        echo " Failed to become healthy"
        echo "Service logs for $service:"
        docker-compose logs --tail=10 "$service"
        exit 1
    fi
done

echo ""
echo "All services are healthy!"
echo ""
echo "Access Points:"
echo "  Web Interface:  http://localhost:8080"
echo "  RTSP (VLC):     rtsp://localhost:8555/cam1"  
echo "  SRT (VLC):      srt://127.0.0.1:8890?streamid=publish:cam1"
echo "  HLS:            http://localhost:8888/cam1/index.m3u8"
echo "  MediaMTX API:   http://localhost:9997"
