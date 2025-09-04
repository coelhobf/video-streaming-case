.PHONY: all build run start clean stop status test test-output test-vlc help

CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2
PKGS = gstreamer-1.0

SRT_PORT = 8890
RTSP_PORT = 8554

CXXFLAGS += $(shell pkg-config --cflags $(PKGS))
LDLIBS   = $(shell pkg-config --libs $(PKGS))

help:
	@echo "Pipeline 2: RTSP → SRT"
	@echo "======================"
	@echo ""
	@echo "Commands:"
	@echo "  make              - Build pipeline 2"
	@echo "  make run          - Run pipeline 2"
	@echo "  make test         - Run with prerequisite checks"
	@echo "  make test-output  - Test SRT output (without Pipeline 3)"
	@echo "  make test-vlc     - Test with VLC player"
	@echo "  make clean        - Clean build files"
	@echo "  make stop         - Stop any running processes"
	@echo ""
	@echo "Prerequisites:"
	@echo "  - Pipeline 1: RTSP server on port $(RTSP_PORT)"
	@echo "  - For full pipeline: Pipeline 3 on port $(SRT_PORT)"
	@echo ""

all: main.out

build: main.out

main.out: main.cpp
	@echo "Building Pipeline 2..."
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)
	@echo "✅ Build complete"

start: main.out
	@echo "Starting Pipeline 2: RTSP → SRT"
	./main.out &

status:
	@echo "Pipeline 2 Status:"
	@pids=$$(pgrep -f "main.out" 2>/dev/null || true); \
	if [ -n "$$pids" ]; then echo "  Running (PID: $$pids)"; else echo "  Not running"; fi
	@echo "  SRT Port $(SRT_PORT): $$(lsof -iUDP:$(SRT_PORT) 2>/dev/null | wc -l | tr -d ' ') connections"

run: main.out
	@echo "Starting Pipeline 2: RTSP → SRT"
	./main.out

test: main.out
	@echo "Running Pipeline 2 with checks..."
	./test_pipeline2.sh

test-output:
	@echo "Testing Pipeline 2 SRT output..."
	./test_srt_output.sh

test-vlc:
	@echo "Opening VLC to test SRT stream..."
	@echo "VLC will try to connect to: srt://127.0.0.1:$(SRT_PORT)?streamid=publish:cam1"
	@echo "Make sure Pipeline 2 is running first!"
	vlc srt://127.0.0.1:$(SRT_PORT)?streamid=publish:cam1 --intf dummy --play-and-exit --run-time=10 || echo "VLC test completed"

stop:
	@echo "Stopping any processes using SRT port $(SRT_PORT)..."
	-@pids=$$(lsof -t -iUDP:$(SRT_PORT) -iTCP:$(SRT_PORT) 2>/dev/null); if [ -n "$$pids" ]; then echo "Killing PIDs: $$pids"; kill -9 $$pids || true; else echo "No processes found"; fi

clean:
	@echo "Cleaning build files..."
	rm -f main.out
	@echo "✅ Clean complete"
