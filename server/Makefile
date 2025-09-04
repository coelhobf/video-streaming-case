.PHONY: up down restart logs status open print test-srt demo help

PORT_SRT=8890
PORT_HLS=8888
PORT_WEBRTC=8889
PORT_API=9997
STREAM=cam1

help:
	@echo "Paladium Video Pipeline - MediaMTX Server"
	@echo ""
	@echo "Commands:"
	@echo "  up          - Start MediaMTX server"
	@echo "  down        - Stop MediaMTX server" 
	@echo "  restart     - Restart MediaMTX server"
	@echo "  logs        - Show MediaMTX logs"
	@echo "  status      - Check MediaMTX status and streams"
	@echo "  test-srt    - Start SRT emulator (test stream)"
	@echo "  open        - Open web interface"
	@echo "  print       - Show all URLs"
	@echo "  demo        - Full demo (start server + test stream)"
	@echo ""

up:
	@echo "Starting MediaMTX server..."
	docker compose -f docker-compose.yml up -d
	@echo "Waiting for MediaMTX to be ready..."
	@sleep 3
	@$(MAKE) status

down:
	@echo "Stopping MediaMTX server..."
	docker compose -f docker-compose.yml down

restart: down up

logs:
	docker logs -f mediamtx

status:
	@echo "=== MediaMTX Status ==="
	@docker ps --filter "name=mediamtx" --format "table {{.Names}}\t{{.Status}}\t{{.Ports}}"
	@echo ""
	@echo "=== API Health Check ==="
	@curl -s http://localhost:$(PORT_API)/v3/config/global/get > /dev/null && echo "✓ API responding" || echo "✗ API not responding"
	@echo ""
	@echo "=== Stream Status ==="
	@curl -s http://localhost:$(PORT_API)/v3/paths/list | python3 -m json.tool 2>/dev/null | grep -A5 -B5 "cam1" || echo "No cam1 stream found"

test-srt:
	@echo "Starting SRT test stream emulator..."
	@echo "Publishing to: srt://localhost:$(PORT_SRT)?streamid=publish:$(STREAM)"
	python3 srt_emulator.py "srt://localhost:$(PORT_SRT)?streamid=publish:$(STREAM)"

open:
	@echo "Opening web interface..."
	open http://localhost:8080/ || xdg-open http://localhost:8080/ || echo "Please visit: http://localhost:8080/"

print:
	@echo "=== Paladium Video Pipeline URLs ==="
	@echo ""
	@echo "📡 SRT Publishing (Pipeline 2):"
	@echo "   srt://localhost:$(PORT_SRT)?streamid=publish:$(STREAM)"
	@echo ""
	@echo "📺 VLC SRT Playback:"
	@echo "   srt://localhost:$(PORT_SRT)?streamid=read:$(STREAM)"
	@echo ""
	@echo "🌐 Web Interfaces:"
	@echo "   Web UI:  http://localhost:8080/"
	@echo "   HLS:     http://localhost:$(PORT_HLS)/$(STREAM)/index.m3u8"
	@echo "   WebRTC:  http://localhost:$(PORT_WEBRTC)/whep?path=$(STREAM)"
	@echo ""
	@echo "🔧 Management:"
	@echo "   API:     http://localhost:$(PORT_API)/v3/paths/list"
	@echo "   Metrics: http://localhost:9998/metrics"
	@echo ""

demo: up
	@echo ""
	@echo "🚀 Starting full demo..."
	@$(MAKE) print
	@echo ""
	@echo "Wait 5 seconds for MediaMTX to fully initialize..."
	@sleep 5
	@echo ""
	@echo "Starting SRT test stream in the background..."
	@echo "You can now test the web interface!"
	@$(MAKE) open
	@echo ""
	@echo "To start test stream manually: make test-srt"
	@echo "To stop everything: make down"