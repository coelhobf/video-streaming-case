#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <signal.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>


using namespace std;
using namespace std::literals;

void signalHandler(int sig_num) {
    cout << "Received signal: " << to_string(sig_num) << endl;
    exit(sig_num);
}

string createPipelineString(const string& video_file) {
    stringstream pipeline;
    filesystem::path absolute_path = filesystem::absolute(video_file);

    pipeline << "("
             << "filesrc location=" << quoted(absolute_path.string()) << " ! "
             << "qtdemux name=demux ! "
             << "queue ! "
             << "h264parse ! "
             << "rtph264pay name=pay0 pt=96 "
             << ")";

    return pipeline.str();
}

string getRTSPUrl(const string& port, const string& mount_point) {
    return "rtsp://localhost:" + port + mount_point;
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    gst_init (nullptr, nullptr);

    if (!gst_is_initialized()) {
        cerr << "Failed to initiate GStreamer" << endl;
        return 1;
    }

    GstRTSPServer* server = gst_rtsp_server_new();

    if (!server) {
        cerr << "Failed to create RSTSP server" << endl;
        return 1;
    }

    const string port{"8554"};
    gst_rtsp_server_set_service(server, port.c_str());
    
    GstRTSPMountPoints* mount_points = gst_rtsp_server_get_mount_points(server);
    
    GstRTSPMediaFactory* media_factory = gst_rtsp_media_factory_new();
    
    const string& video_file{"assets/test.mp4"};
    string pipeline = createPipelineString(video_file);

    gst_rtsp_media_factory_set_launch(media_factory, pipeline.c_str());

    gst_rtsp_media_factory_set_shared(media_factory, true);
    
    gst_rtsp_media_factory_set_eos_shutdown(media_factory, false);

    const string& mount_pount{"/cam1"};
    gst_rtsp_mount_points_add_factory(mount_points, mount_pount.c_str(), media_factory);

    GMainLoop* g_main_loop = g_main_loop_new(nullptr, false);

    if(!g_main_loop) {
        cerr << "Failed to create GLib main loop" << endl;
        return 1;
    }

    std::uint32_t server_id = gst_rtsp_server_attach(server, nullptr);
    if (!server_id) {
        cerr << "Failed to attach server" << endl;
    }

    cout << "Server url: " << getRTSPUrl(port, mount_pount) << endl;

    g_main_loop_run(g_main_loop);
    if(g_main_loop && g_main_loop_is_running(g_main_loop)) {
        g_main_loop_quit(g_main_loop);
    }

    if (media_factory) {
        g_object_unref(media_factory);
    }

    if (mount_points) {
        g_object_unref(mount_points);
    }

    if (server) {
        g_object_unref(server);
    }

    if (g_main_loop) {
        g_main_loop_unref(g_main_loop);
    }

    return 0;
}