#include "rtsp_server.hpp"
#include "../../utils/logger.hpp"
#include <format>

namespace paladium {

RTSPServer::RTSPServer(const std::string& media_file, uint16_t rtsp_port) 
    : media_file_(media_file), rtsp_port_(rtsp_port) {
    gst_init(nullptr, nullptr);
}

RTSPServer::~RTSPServer() {
    if (loop_ && g_main_loop_is_running(loop_.get())) {
        g_main_loop_quit(loop_.get());
    }
}

std::expected<void, std::string> RTSPServer::initialize() {
    server_.reset(gst_rtsp_server_new());
    if (!server_) {
        return std::unexpected("Failed to create RTSP server");
    }

    gst_rtsp_server_set_service(server_.get(), std::to_string(rtsp_port_).c_str());

    if (auto result = setup_mount_points(); !result) {
        return result;
    }

    loop_.reset(g_main_loop_new(nullptr, FALSE));
    if (!loop_) {
        return std::unexpected("Failed to create main loop");
    }

    g_signal_connect(server_.get(), "client-connected", 
                     G_CALLBACK(on_client_connected), this);

    return {};
}

std::expected<void, std::string> RTSPServer::setup_mount_points() {
    mounts_.reset(gst_rtsp_server_get_mount_points(server_.get()));
    
    pipeline_ = std::make_unique<MediaPipeline>(media_file_);
    if (auto result = pipeline_->create_factory(); !result) {
        return std::unexpected(std::format("Pipeline creation failed: {}", result.error()));
    }

    gst_rtsp_mount_points_add_factory(mounts_.get(), "/cam1", pipeline_->get_factory());
    
    return {};
}

int RTSPServer::run() {
    if (!gst_rtsp_server_attach(server_.get(), nullptr)) {
        Logger::error("Failed to attach RTSP server");
        return 1;
    }

    Logger::info("RTSP server ready at rtsp://localhost:{}/cam1", rtsp_port_);
    g_main_loop_run(loop_.get());
    
    return 0;
}

void RTSPServer::shutdown() {
    if (loop_ && g_main_loop_is_running(loop_.get())) {
        g_main_loop_quit(loop_.get());
    }
}

gboolean RTSPServer::on_client_connected(GstRTSPClient* /*client*/, gpointer /*user_data*/) {
    Logger::info("New RTSP client connected");
    return TRUE;
}

} // namespace paladium
