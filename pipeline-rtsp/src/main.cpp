#include <iostream>
#include <memory>
#include <signal.h>
#include <filesystem>
#include <cstdlib>
#include "rtsp_server.hpp"

using namespace paladium;

static RTSPServer* g_server = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    if (g_server) {
        g_server->shutdown();
    }
}

int main(int /*argc*/, char* /*argv*/[]) {
    // Use environment variable or fall back to default
    const char* env_media_file = std::getenv("MEDIA_FILE");
    const std::string media_file = env_media_file ? env_media_file : "../media/sample.mp4";
    const uint16_t rtsp_port = 8555;

    if (!std::filesystem::exists(media_file)) {
        std::cerr << "Video file not found: " << media_file << std::endl;
        std::cerr << "Run './create_test_video.sh' to create a test video" << std::endl;
        return 1;
    }

    auto server = std::make_unique<RTSPServer>(media_file, rtsp_port);
    g_server = server.get();
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    if (auto result = server->initialize(); !result) {
        std::cerr << "Server failed: " << result.error() << std::endl;
        return 1;
    }

    std::cout << "RTSP server at rtsp://localhost:" << rtsp_port << "/cam1" << std::endl;
    return server->run();
}