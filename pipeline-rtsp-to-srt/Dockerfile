# Pipeline 2: RTSP to SRT Relay - Single Stage
FROM ubuntu:24.04

# Avoid interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install all dependencies (build + runtime) - Complete GStreamer setup  
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    pkg-config \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-good1.0-dev \
    libgstreamer-plugins-bad1.0-dev \
    libsrt-gnutls-dev \
    libgstreamer1.0-0 \
    libgstreamer-plugins-base1.0-0 \
    libgstreamer-plugins-good1.0-0 \
    libgstreamer-plugins-bad1.0-0 \
    libsrt1.5-gnutls \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-tools \
    gstreamer1.0-x \
    procps \
    && rm -rf /var/lib/apt/lists/* \
    && gst-inspect-1.0 > /dev/null

WORKDIR /app

# Copy source files and build configuration  
COPY pipeline-rtsp-to-srt/src/ ./pipeline-rtsp-to-srt/src/
COPY pipeline-rtsp-to-srt/Makefile ./pipeline-rtsp-to-srt/
COPY utils/ ./utils/

# Set working directory and build
WORKDIR /app/pipeline-rtsp-to-srt
RUN make clean && make

# Copy binary to standard location
RUN cp pipeline-rtsp-to-srt /usr/local/bin/

# Set working directory back
WORKDIR /app

# Default command
CMD ["pipeline-rtsp-to-srt"]