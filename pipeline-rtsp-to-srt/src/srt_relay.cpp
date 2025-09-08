#include "srt_relay.hpp"
#include "../../utils/logger.hpp"
#include <format>
#include <chrono>
#include <thread>

namespace paladium {

SRTRelay::SRTRelay(const std::string& rtsp_url, const std::string& srt_url) 
    : rtsp_url_(rtsp_url), srt_url_(srt_url) {
    gst_init(nullptr, nullptr);
}

SRTRelay::~SRTRelay() {
    shutdown();
}

std::expected<void, std::string> SRTRelay::initialize() {
    if (!gst_is_initialized()) {
        return std::unexpected("Failed to initialize GStreamer");
    }

    loop_.reset(g_main_loop_new(nullptr, FALSE));
    if (!loop_) {
        return std::unexpected("Failed to create main loop");
    }

    return {};
}

std::expected<void, std::string> SRTRelay::create_pipeline() {
    GError* error = nullptr;
    auto pipeline_str = build_pipeline_string();
    
    Logger::info("Creating pipeline: {}", pipeline_str);
    
    pipeline_.reset(gst_parse_launch(pipeline_str.c_str(), &error));
    
    if (!pipeline_) {
        std::string error_msg = error ? error->message : "Unknown error";
        if (error) g_error_free(error);
        return std::unexpected(std::format("Failed to create pipeline: {}", error_msg));
    }

    if (error) {
        Logger::warn("Pipeline created with warnings: {}", error->message);
        g_error_free(error);
    }

    bus_.reset(gst_element_get_bus(pipeline_.get()));
    if (!bus_) {
        return std::unexpected("Failed to get pipeline bus");
    }

    gst_bus_add_watch(bus_.get(), on_bus_message, this);
    return {};
}

std::string SRTRelay::build_pipeline_string() const {
    return std::format(
        // Connect to RTSP source with aggressive disconnection detection
        "rtspsrc location={} "
        // Use TCP for RTSP, enable keep-alive, reduce latency for faster detection
        "protocols=tcp do-rtsp-keep-alive=true latency=100 "
        // Add timeouts for faster disconnection detection
        "timeout=5000000 tcp-timeout=5000000 retry=3 ! "
        // Depayload RTP packets to extract H.264 video
        "rtph264depay ! "
        // Parse H.264 stream, send SPS/PPS with every keyframe
        "h264parse config-interval=-1 ! "
        // Ensure H.264 is in byte-stream format and AU-aligned 
        "video/x-h264,stream-format=byte-stream,alignment=au ! "
        // Mux video into MPEG-TS container
        "mpegtsmux ! "
        // Output MPEG-TS over SRT to the given URI
        "srtclientsink uri={} "
        // Set SRT streamid, caller mode, don't wait for connection
        "streamid=publish:cam1 mode=caller wait-for-connection=false",
        rtsp_url_, srt_url_
    );
}

bool SRTRelay::run_single_iteration() {
    restart_requested_ = false;

    if (auto result = create_pipeline(); !result) {
        Logger::error("Pipeline creation failed: {}", result.error());
        return false;
    }

    Logger::info("Starting pipeline...");
    GstStateChangeReturn ret = gst_element_set_state(pipeline_.get(), GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        Logger::error("Failed to start pipeline");
        return false;
    }

    Logger::info("Pipeline started - relaying RTSP to SRT");
    Logger::info("Reading from: {}", rtsp_url_);
    Logger::info("Publishing to: {}", srt_url_);
    
    g_main_loop_run(loop_.get());

    return restart_requested_;
}

int SRTRelay::run(const std::atomic<bool>& stop_requested) {
    while (!stop_requested.load()) {
        bool should_restart = run_single_iteration();
        
        if (stop_requested.load()) {
            break;
        }
        
        if (should_restart) {
            Logger::warn("Restarting pipeline in 2 seconds...");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
            Logger::info("Pipeline stopped normally");
            break;
        }
    }

    Logger::info("SRT Relay shutting down");
    return 0;
}

void SRTRelay::shutdown() {
    Logger::info("SRT Relay shutting down");
    
    if (pipeline_) {
        gst_element_set_state(pipeline_.get(), GST_STATE_NULL);
    }
    
    if (loop_ && g_main_loop_is_running(loop_.get())) {
        g_main_loop_quit(loop_.get());
    }
}

gboolean SRTRelay::on_bus_message(GstBus* /*bus*/, GstMessage* message, gpointer user_data) {
    SRTRelay* relay = static_cast<SRTRelay*>(user_data);
    GError* error = nullptr;
    gchar* debug = nullptr;

    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR: {
            gst_message_parse_error(message, &error, &debug);
            std::string error_msg = error ? error->message : "Unknown error";
            std::string debug_info = debug ? debug : "";
            
            // Detect RTSP source failures specifically
            if (debug_info.find("rtspsrc") != std::string::npos || 
                error_msg.find("Could not open resource") != std::string::npos ||
                error_msg.find("Failed to connect") != std::string::npos) {
                Logger::error("RTSP source disconnected: {}", error_msg);
                Logger::debug("RTSP debug info: {}", debug_info);
            } else {
                Logger::error("Pipeline error: {}", error_msg);
                if (debug) Logger::debug("Debug info: {}", debug_info);
            }
            
            relay->restart_requested_ = true;
            g_main_loop_quit(relay->loop_.get());
            break;
        }
        case GST_MESSAGE_EOS: {
            Logger::warn("End of stream - restarting...");
            relay->restart_requested_ = true;
            g_main_loop_quit(relay->loop_.get());
            break;
        }
        case GST_MESSAGE_WARNING: {
            gst_message_parse_warning(message, &error, &debug);
            std::string warning_msg = error ? error->message : "Unknown warning";
            
            // If SRT keeps failing to reconnect, restart the entire pipeline
            if (warning_msg.find("Socket is broken or closed") != std::string::npos) {
                static int srt_warning_count = 0;
                srt_warning_count++;
                
                if (srt_warning_count % 5 == 1) {
                    Logger::warn("SRT connection lost - attempting reconnection (attempt {})", srt_warning_count);
                }
                
                // After 10 failed attempts (30 seconds), restart entire pipeline
                if (srt_warning_count >= 10) {
                    Logger::warn("SRT reconnection failed repeatedly - restarting entire pipeline");
                    srt_warning_count = 0;
                    relay->restart_requested_ = true;
                    g_main_loop_quit(relay->loop_.get());
                }
            } else {
                Logger::warn("Pipeline warning: {}", warning_msg);
                if (debug) Logger::debug("Debug info: {}", debug);
            }
            break;
        }
        case GST_MESSAGE_INFO: {
            gst_message_parse_info(message, &error, &debug);
            Logger::info("Pipeline info: {}", error ? error->message : "Unknown info");
            break;
        }
        case GST_MESSAGE_STATE_CHANGED: {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
            
            // Monitor for unexpected state drops from PLAYING
            if (old_state == GST_STATE_PLAYING && new_state < GST_STATE_PLAYING) {
                Logger::warn("Pipeline dropped from PLAYING to {} - possible source failure", 
                           gst_element_state_get_name(new_state));
            }
            break;
        }
        default:
            break;
    }

    if (error) g_error_free(error);
    if (debug) g_free(debug);
    
    return TRUE;
}

} // namespace paladium
