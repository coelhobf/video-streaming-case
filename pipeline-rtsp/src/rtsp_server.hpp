#pragma once

#include <expected>
#include <string>
#include <memory>
#include <cstdint>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include "media_pipeline.hpp"

namespace paladium {

class RTSPServer {
public:
    RTSPServer(const std::string& media_file, uint16_t rtsp_port);
    ~RTSPServer();

    std::expected<void, std::string> initialize();
    int run();
    void shutdown();

private:
    struct GstDeleter {
        void operator()(GstRTSPServer* server) { if (server) g_object_unref(server); }
        void operator()(GstRTSPMountPoints* mounts) { if (mounts) g_object_unref(mounts); }
        void operator()(GMainLoop* loop) { if (loop) g_main_loop_unref(loop); }
    };

    std::string media_file_;
    uint16_t rtsp_port_;
    std::unique_ptr<GstRTSPServer, GstDeleter> server_;
    std::unique_ptr<GstRTSPMountPoints, GstDeleter> mounts_;
    std::unique_ptr<GMainLoop, GstDeleter> loop_;
    std::unique_ptr<MediaPipeline> pipeline_;

    std::expected<void, std::string> setup_mount_points();
    static gboolean on_client_connected(GstRTSPClient* client, gpointer user_data);
};

} // namespace paladium
