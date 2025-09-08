#pragma once

#include <expected>
#include <string>
#include <memory>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

namespace paladium {

class MediaPipeline {
public:
    explicit MediaPipeline(const std::string& media_file);
    ~MediaPipeline();

    std::expected<void, std::string> create_factory();
    GstRTSPMediaFactory* get_factory() const { return factory_; }

private:
    std::string media_file_;
    GstRTSPMediaFactory* factory_;

    std::string build_pipeline_string() const;
    static void on_media_configure(GstRTSPMediaFactory* factory, 
                                   GstRTSPMedia* media, gpointer user_data);
};

} // namespace paladium
