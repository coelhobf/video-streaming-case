#include <iostream>
#include <memory>
#include <atomic>
#include <signal.h>
#include <glib.h>
#include <cstdlib>
#include "srt_relay.hpp"
#include "../../utils/logger.hpp"

using namespace paladium;

static SRTRelay* g_relay = nullptr;
static std::atomic<bool> g_stop_requested{false};

void signal_handler(int signal) {
    static int signal_count = 0;
    signal_count++;
    if (signal_count >= 2) {
        std::cout << "Force shutdown - exiting immediately" << std::endl;
        std::exit(1);
    }
    
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_stop_requested = true;
    
    // Schedule shutdown from main thread context (GLib-safe)
    g_idle_add([](gpointer user_data) -> gboolean {
        auto* relay = static_cast<SRTRelay*>(user_data);
        if (relay) {
            relay->shutdown();
        }
        return G_SOURCE_REMOVE;
    }, g_relay);
}

int main(int /*argc*/, char* /*argv*/[]) {
    // Use environment variables or fall back to defaults
    const char* env_rtsp_url = std::getenv("RTSP_URL");
    const char* env_srt_url = std::getenv("SRT_URL");
    
    const std::string rtsp_url = env_rtsp_url ? env_rtsp_url : "rtsp://127.0.0.1:8555/cam1";
    const std::string srt_url = env_srt_url ? env_srt_url : "srt://127.0.0.1:8890?streamid=publish:cam1";

    std::cout << "srt_url: " << srt_url << std::endl;

    auto relay = std::make_unique<SRTRelay>(rtsp_url, srt_url);
    g_relay = relay.get();
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (auto result = relay->initialize(); !result) {
        Logger::error("Relay initialization failed: {}", result.error());
        return 1;
    }

    Logger::info("Starting RTSP to SRT relay");
    Logger::info("Input: {}", rtsp_url);
    Logger::info("Output: {}", srt_url);
    
    return relay->run(g_stop_requested);
}