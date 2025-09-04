.PHONY: all build run start clean stop status cleanport checkport checkvideo

CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2
PKGS = gstreamer-1.0 gstreamer-rtsp-server-1.0

CXXFLAGS += $(shell pkg-config --cflags $(PKGS))
LDLIBS   = $(shell pkg-config --libs $(PKGS))

PORT ?= 8554

all: main.out

build: main.out

main.out: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

start: main.out checkvideo
	./main.out &

status:
	@echo "Pipeline 1 Status:"
	@pids=$$(pgrep -f "main.out" 2>/dev/null || true); \
	if [ -n "$$pids" ]; then echo "  Running (PID: $$pids)"; else echo "  Not running"; fi
	@lsof -iTCP:$(PORT) -sTCP:LISTEN 2>/dev/null | grep -q ":$(PORT)" && echo "  Port $(PORT): Available" || echo "  Port $(PORT): Not listening"

run: main.out checkvideo
	-@pids=$$(lsof -t -iTCP:$(PORT) -sTCP:LISTEN); \
	 if [ -n "$$pids" ]; then echo "Killing PIDs on port $(PORT): $$pids"; kill -9 $$pids || true; else echo "No listeners on $(PORT)"; fi
	./main.out

clean:
	rm -f main.out

stop:
	-@pkill -f "/Users/bcoelho/Documents/workspace/paladium/pipeline-rtsp/main.out" || true
	-@pids=$$(lsof -t -iTCP:$(PORT) -sTCP:LISTEN); if [ -n "$$pids" ]; then kill -9 $$pids || true; fi

cleanport:
	-@pids=$$(lsof -t -iTCP:$(PORT) -sTCP:LISTEN); \
	 if [ -n "$$pids" ]; then echo "Killing PIDs on port $(PORT): $$pids"; kill -9 $$pids || true; else echo "No listeners on $(PORT)"; fi

checkport:
	@lsof -iTCP:$(PORT) -sTCP:LISTEN || true

checkvideo:
	@if [ ! -f "assets/test.mp4" ]; then \
		echo "📹 Video file not found, creating test video..."; \
		./create_test_video.sh; \
	else \
		echo "✅ Video file exists: assets/test.mp4"; \
	fi
