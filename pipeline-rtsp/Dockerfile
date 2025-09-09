# Pipeline 1: MP4 to RTSP Server - Single Stage
FROM ubuntu:24.04

# Avoid interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install all dependencies (build + runtime) - Complete GStreamer setup
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    pkg-config \
    gstreamer1.0-libav \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-tools \
    libgstreamer-plugins-bad1.0-0 \
    libgstreamer-plugins-bad1.0-dev \
    libgstreamer-plugins-base1.0-0 \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-good1.0-0 \
    libgstreamer-plugins-good1.0-dev \
    libgstreamer1.0-0 \
    libgstreamer1.0-dev \
    libgstrtspserver-1.0-0 \
    libgstrtspserver-1.0-dev \
    gstreamer1.0-x \
    && rm -rf /var/lib/apt/lists/* \
    && gst-inspect-1.0 > /dev/null

WORKDIR /app

# Copy source files and build configuration
COPY pipeline-rtsp/src/ ./pipeline-rtsp/src/
COPY pipeline-rtsp/Makefile ./pipeline-rtsp/
COPY utils/ ./utils/

# Set working directory and build
WORKDIR /app/pipeline-rtsp
RUN make clean && make

# Copy binary to standard location
RUN cp pipeline-rtsp /usr/local/bin/

# Set working directory back
WORKDIR /app

# Expose RTSP port
EXPOSE 8555

# Default command
CMD ["pipeline-rtsp"]