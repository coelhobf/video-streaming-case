#include "media_pipeline.hpp"
#include "../../utils/logger.hpp"
#include <format>
#include <filesystem>

namespace paladium {

MediaPipeline::MediaPipeline(const std::string& media_file) 
    : media_file_(media_file), factory_(nullptr) {}

MediaPipeline::~MediaPipeline() {
    if (factory_) {
        g_object_unref(factory_);
    }
}

std::expected<void, std::string> MediaPipeline::create_factory() {
    if (!std::filesystem::exists(media_file_)) {
        return std::unexpected(std::format("Media file not found: {}", media_file_));
    }

    factory_ = gst_rtsp_media_factory_new();
    if (!factory_) {
        return std::unexpected("Failed to create media factory");
    }

    auto pipeline_str = build_pipeline_string();
    gst_rtsp_media_factory_set_launch(factory_, pipeline_str.c_str());
    gst_rtsp_media_factory_set_shared(factory_, TRUE);

    g_signal_connect(factory_, "media-configure", 
                     G_CALLBACK(on_media_configure), this);

    Logger::info("Media pipeline created: {}", pipeline_str);
    return {};
}

std::string MediaPipeline::build_pipeline_string() const {
    return std::format(
        // Cross-platform file-based pipeline: works on both macOS and Ubuntu Docker
        "( "
            // Read video file from filesystem
            "filesrc location={} "
        "! "
            // Demultiplex MP4 container - separates video/audio streams  
            // Uses explicit naming 'd' for cross-platform pad referencing
            "qtdemux name=d "
            // CRITICAL: Explicitly connect to video_0 pad
            // This solves "not-linked" errors in Ubuntu Docker containers
            // macOS auto-links, but Ubuntu requires explicit pad connection
            "d.video_0 "
        "! "
            // Buffer video data to smooth playback and prevent underruns
            // Essential for network streaming to handle timing variations
            "queue "
        "! "
            // Parse H.264 video stream structure
            // Extracts SPS/PPS headers and ensures proper stream formatting
            "h264parse "
        "! "
            // Package H.264 into RTP packets for RTSP streaming
            // pt=96: RTP payload type for H.264 video
            // name=pay0: Named element required by RTSP media factory
            "rtph264pay name=pay0 pt=96 "
        ")",
        media_file_
    );
}

void MediaPipeline::on_media_configure(GstRTSPMediaFactory* /*factory*/, 
                                       GstRTSPMedia* media, gpointer /*user_data*/) {
    Logger::debug("Media configured for streaming");
    
    // Configure media for multiple client support (vlc and pipeline 2 at the same time)
    gst_rtsp_media_set_reusable(media, TRUE);
    gst_rtsp_media_set_shared(media, TRUE);
    
    // Set media to use one pipeline for all clients (prevents tee issues)
    gst_rtsp_media_set_eos_shutdown(media, TRUE);
    
    Logger::debug("Media configured for multi-client streaming");
}

} // namespace paladium
