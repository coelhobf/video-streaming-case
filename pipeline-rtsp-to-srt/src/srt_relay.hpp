#pragma once

#include <expected>
#include <string>
#include <memory>
#include <atomic>
#include <cstdint>
#include <gst/gst.h>

namespace paladium {

class SRTRelay {
public:
    SRTRelay(const std::string& rtsp_url, const std::string& srt_url);
    ~SRTRelay();

    std::expected<void, std::string> initialize();
    int run(const std::atomic<bool>& stop_requested);
    void shutdown();

private:
    struct GstDeleter {
        void operator()(GstElement* element) { 
            if (element) {
                gst_element_set_state(element, GST_STATE_NULL);
                gst_object_unref(element);
            }
        }
        void operator()(GstBus* bus) { if (bus) gst_object_unref(bus); }
        void operator()(GMainLoop* loop) { if (loop) g_main_loop_unref(loop); }
    };

    std::string rtsp_url_;
    std::string srt_url_;
    std::unique_ptr<GstElement, GstDeleter> pipeline_;
    std::unique_ptr<GstBus, GstDeleter> bus_;
    std::unique_ptr<GMainLoop, GstDeleter> loop_;
    std::atomic<bool> restart_requested_{false};

    std::expected<void, std::string> create_pipeline();
    std::string build_pipeline_string() const;
    static gboolean on_bus_message(GstBus* bus, GstMessage* message, gpointer user_data);
    bool run_single_iteration();
};

} // namespace paladium
