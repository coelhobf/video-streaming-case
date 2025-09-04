# Paladium Video Pipeline

A video streaming pipeline that converts MP4 files to RTSP, then to SRT, and serves via WebRTC/HLS.

## Architecture

```
Pipeline 1: File → RTSP (C++)
Pipeline 2: RTSP → SRT (C++)  
Pipeline 3: SRT → WebRTC/HLS (MediaMTX)
```

## Quick Start

### Prerequisites
- Docker and Docker Compose
- GStreamer: `brew install gstreamer pkgconf`
- C++ compiler with C++23 support
- VLC Media Player (for testing)

### Run Demo
```bash
make demo
```

### Manual Control
```bash
make build    # Build all components
make start    # Start all pipelines
make status   # Check status
make stop     # Stop all pipelines
make test     # Test all streams
```

## Test URLs

**Web Interface:** http://localhost:8080
**VLC RTSP:** rtsp://localhost:8554/cam1
**VLC SRT:** srt://localhost:8890?streamid=read:cam1

## Project Structure

```
paladium/
├── Makefile                    # Main pipeline control
├── pipeline-rtsp/              # Pipeline 1: File → RTSP
│   ├── main.cpp               # C++ GStreamer RTSP server
│   ├── Makefile               # Build and run commands
│   ├── create_test_video.sh   # Auto-create test video
│   └── assets/test.mp4        # Test video file (auto-created)
├── pipeline-rtsp-to-srt/       # Pipeline 2: RTSP → SRT
│   ├── main.cpp               # C++ GStreamer SRT publisher
│   └── Makefile               # Build and run commands
└── server/                     # Pipeline 3: SRT → WebRTC/HLS
    ├── docker-compose.yml     # MediaMTX and web UI containers
    ├── mediamtx.yml           # MediaMTX configuration
    └── web/index.html         # Browser playback interface
```

## Monitoring

**Available Endpoints:**
- Health Check: `curl http://localhost:9997/v3/paths/list`
- Stream Info: `curl http://localhost:9997/v3/paths/get/cam1`
- Metrics: `curl http://localhost:9998/metrics`

See `docs/Monitoring_Strategy.md` for current monitoring capabilities.

## Troubleshooting

**Pipeline 2 won't connect:**
```bash
make pipeline1    # Start Pipeline 1 first
sleep 3           # Wait for RTSP to be ready
make pipeline2    # Then start Pipeline 2
```

**Port conflicts:**
```bash
lsof -i :8554     # RTSP
lsof -i :8890     # SRT  
lsof -i :8080     # Web UI
```

**Check logs:**
```bash
docker logs mediamtx
```

## Technical Details

- **Pipeline 1**: Creates RTSP stream from MP4 file (auto-creates test video if missing)
- **Pipeline 2**: Consumes RTSP, publishes to SRT (auto-restart on errors)
- **Pipeline 3**: MediaMTX server exposes SRT as WebRTC/HLS
- **Resilience**: Pipeline 2 has automatic restart, others require manual restart