CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -O2
INCLUDES := -I./src $(shell pkg-config --cflags gstreamer-1.0)
LIBS := $(shell pkg-config --libs gstreamer-1.0)

SRCDIR := src
OBJDIR := build
SOURCES := $(shell find $(SRCDIR) -name "*.cpp")
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET := pipeline-rtsp-to-srt

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Docker commands
docker-build:
	docker build -t paladium-pipeline-srt .

docker-run: docker-build
	docker run --rm --network host paladium-pipeline-srt

# Aliases for consistency
build: all
start: run
stop:
	@pkill -f pipeline-rtsp-to-srt || true
status:
	@pgrep -f pipeline-rtsp-to-srt > /dev/null && echo "✅ Running" || echo "❌ Not running"

.PHONY: all clean run docker-build docker-run start stop status build
