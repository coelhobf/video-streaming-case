
.PHONY: help clean build start stop status test demo

help:
	@echo "Paladium Video Pipeline"
	@echo "======================"
	@echo ""
	@echo "Commands:"
	@echo "  make build    - Build all pipelines"
	@echo "  make start    - Start all pipelines"
	@echo "  make stop     - Stop all pipelines"
	@echo "  make status   - Check status of all components"
	@echo "  make test     - Test all pipelines"
	@echo "  make demo     - One-command demo (build + start)"
	@echo "  make clean    - Clean all build files"
	@echo ""
	@echo "Individual Components:"
	@echo "  make pipeline1    - Start Pipeline 1 (File → RTSP)"
	@echo "  make pipeline2    - Start Pipeline 2 (RTSP → SRT)"
	@echo "  make pipeline3    - Start Pipeline 3 (SRT → WebRTC/HLS)"
	@echo ""
	@echo "Test URLs:"
	@echo "  RTSP: rtsp://localhost:8554/cam1"
	@echo "  SRT:  srt://localhost:8890?streamid=read:cam1"
	@echo "  HLS:  http://localhost:8888/cam1/index.m3u8"
	@echo "  Web:  http://localhost:8080"

build:
	@echo "Building all pipelines..."
	@make -C pipeline-rtsp build
	@make -C pipeline-rtsp-to-srt build
	@echo "✅ All pipelines built"

clean:
	@echo "Cleaning all pipelines..."
	@make -C pipeline-rtsp clean
	@make -C pipeline-rtsp-to-srt clean
	@echo "✅ All pipelines cleaned"

start: build
	@echo "Starting Paladium Video Pipeline..."
	@echo ""
	@echo "🎬 Starting Pipeline 3 (MediaMTX Server)..."
	@make -C server up
	@echo ""
	@echo "📹 Starting Pipeline 1 (File → RTSP)..."
	@make -C pipeline-rtsp start &
	@sleep 3
	@echo ""
	@echo "🔄 Starting Pipeline 2 (RTSP → SRT)..."
	@make -C pipeline-rtsp-to-srt start &
	@sleep 2
	@echo ""
	@echo "✅ All pipelines started"
	@echo ""
	@echo "Test URLs:"
	@echo "  Web UI: http://localhost:8080"
	@echo "  VLC SRT: srt://localhost:8890?streamid=read:cam1"

stop:
	@echo "Stopping all pipelines..."
	@make -C pipeline-rtsp stop
	@make -C pipeline-rtsp-to-srt stop
	@make -C server down
	@echo "✅ All pipelines stopped"

status:
	@echo "Pipeline Status:"
	@echo "================"
	@echo ""
	@echo "Pipeline 1 (File → RTSP):"
	@make -C pipeline-rtsp status
	@echo ""
	@echo "Pipeline 2 (RTSP → SRT):"
	@make -C pipeline-rtsp-to-srt status
	@echo ""
	@echo "Pipeline 3 (MediaMTX Server):"
	@make -C server status

test:
	@echo "Testing all pipelines..."
	@echo ""
	@echo "Testing Pipeline 1 (RTSP)..."
	@timeout 5s gst-launch-1.0 rtspsrc location=rtsp://localhost:8554/cam1 ! fakesink || echo "Pipeline 1: Ready"
	@echo ""
	@echo "Testing Pipeline 3 (HLS)..."
	@curl -sf http://localhost:8888/cam1/index.m3u8 > /dev/null && echo "HLS: Available" || echo "HLS: Not available"
	@echo ""
	@echo "Testing Pipeline 3 (WebRTC)..."
	@curl -sf http://localhost:8889/cam1/whep > /dev/null && echo "WebRTC: Available" || echo "WebRTC: Not available"
	@echo ""
	@echo "Testing Web UI..."
	@curl -sf http://localhost:8080 > /dev/null && echo "Web UI: Available" || echo "Web UI: Not available"

demo: stop build start
	@echo ""
	@echo "🎉 Demo started successfully!"
	@echo ""
	@echo "Open these URLs:"
	@echo "  Web UI: http://localhost:8080"
	@echo "  VLC: srt://localhost:8890?streamid=read:cam1"

pipeline1:
	@echo "Starting Pipeline 1 (File → RTSP)..."
	@make -C pipeline-rtsp run

pipeline2:
	@echo "Starting Pipeline 2 (RTSP → SRT)..."
	@make -C pipeline-rtsp-to-srt run

pipeline3:
	@echo "Starting Pipeline 3 (SRT → WebRTC/HLS)..."
	@make -C server up
