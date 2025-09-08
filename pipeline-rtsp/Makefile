CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -O2
INCLUDES := -I./src $(shell pkg-config --cflags gstreamer-1.0 gstreamer-rtsp-server-1.0)
LIBS := $(shell pkg-config --libs gstreamer-1.0 gstreamer-rtsp-server-1.0)

SRCDIR := src
OBJDIR := build
SOURCES := $(shell find $(SRCDIR) -name "*.cpp")
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET := pipeline-rtsp

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

.PHONY: all clean run
