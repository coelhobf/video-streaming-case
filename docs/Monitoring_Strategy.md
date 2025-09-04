# Monitoring Strategy

## Current Implementation Status

This document describes the monitoring capabilities that **currently exist** in the Paladium video pipeline implementation.

## Existing Components

**Pipeline 1 (File → RTSP) - C++ GStreamer**
- Process: `pipeline-rtsp/main.out` executable
- RTSP server on port 8554
- Serves `assets/test.mp4` file
- No automatic restart capability
- No built-in monitoring endpoints

**Pipeline 2 (RTSP → SRT) - C++ GStreamer**
- Process: `pipeline-rtsp-to-srt/main.out` executable  
- Consumes RTSP from localhost:8554/cam1
- Publishes to SRT localhost:8890
- Built-in automatic restart on errors/EOS
- 2-second restart delay
- Console logging of restart events

**Pipeline 3 (MediaMTX Server) - Docker Container**
- MediaMTX container with API endpoints
- Web UI container serving static files
- Configured ports: 8554, 8888, 8889, 8890, 8189, 9997, 9998, 8080
- Built-in Prometheus metrics endpoint

## Available Monitoring Endpoints

**MediaMTX API (Currently Active)**
```bash
# Health check - lists all paths
curl http://localhost:9997/v3/paths/list

# Stream info for cam1
curl http://localhost:9997/v3/paths/get/cam1

# Global configuration
curl http://localhost:9997/v3/config/global/get

# Prometheus metrics
curl http://localhost:9998/metrics
```

## Existing Test Commands

**From Main Makefile**
```bash
# Test all components (existing command)
make test

# Check status (existing command)  
make status

# Individual pipeline status
cd pipeline-rtsp && make status
cd pipeline-rtsp-to-srt && make status
cd server && make status
```

**Stream Validation (Existing)**
```bash
# Test RTSP stream (from Makefile)
timeout 5s gst-launch-1.0 rtspsrc location=rtsp://localhost:8554/cam1 ! fakesink

# Test HLS availability (from Makefile)
curl -sf http://localhost:8888/cam1/index.m3u8

# Test WebRTC endpoint (from Makefile)
curl -sf http://localhost:8889/cam1/whep

# Test Web UI (from Makefile)
curl -sf http://localhost:8080
```

## Current Resilience Features

**Pipeline 1**
- No automatic restart
- Manual restart required: `cd pipeline-rtsp && make run`

**Pipeline 2** 
- Automatic restart on GStreamer errors
- Automatic restart on End-of-Stream
- Fixed 2-second delay between restarts
- Continuous operation until manual stop (Ctrl+C)

**Pipeline 3**
- Docker container restart policies (if configured)
- MediaMTX built-in health monitoring

## Basic Process Monitoring

**Check Running Processes**
```bash
# Pipeline 1
pgrep -f "pipeline-rtsp.*main.out"

# Pipeline 2  
pgrep -f "pipeline-rtsp-to-srt.*main.out"

# Docker containers
docker ps --filter "name=mediamtx"
docker ps --filter "name=webui"
```

**Port Status**
```bash
# RTSP port
lsof -iTCP:8554 -sTCP:LISTEN

# SRT port
lsof -iUDP:8890

# Web ports
lsof -iTCP:8080,8888,8889,9997,9998 -sTCP:LISTEN
```

## Current Limitations

**No Implementation For:**
- Automated alerting system
- Prometheus/Grafana integration
- Systemd service configuration
- Production deployment scripts
- Advanced health check scripts
- Centralized logging
- Performance metrics collection
- Automated failure recovery (except Pipeline 2)

**Manual Monitoring Required For:**
- Process crashes (Pipeline 1)
- Resource usage monitoring
- Network connectivity issues
- Stream quality assessment
- Error log analysis

## Available URLs for Manual Testing

**Streams:**
- RTSP: `rtsp://localhost:8554/cam1`
- SRT (VLC): `srt://localhost:8890?streamid=read:cam1`
- HLS: `http://localhost:8888/cam1/index.m3u8`
- Web UI: `http://localhost:8080`

**Management:**
- MediaMTX API: `http://localhost:9997/v3/paths/list`
- Metrics: `http://localhost:9998/metrics`