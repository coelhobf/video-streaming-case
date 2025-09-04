.PHONY: all run clean

CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2
PKGS = gstreamer-1.0 gstreamer-rtsp-server-1.0

CXXFLAGS += $(shell pkg-config --cflags $(PKGS))
LDLIBS   = $(shell pkg-config --libs $(PKGS))

all: main.out

main.out: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

run: main.out
	./main.out

clean:
	rm -f main.out
