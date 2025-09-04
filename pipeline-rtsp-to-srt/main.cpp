#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>
#include <signal.h>
#include <gst/gst.h>


using namespace std;
using namespace std::literals;

static GMainLoop* g_main_loop = nullptr;
static atomic<bool> g_stop_requested{false};
static atomic<bool> g_restart_pipeline{false};

void signalHandler(int sig_num) {
    cout << "Received signal: " << to_string(sig_num) << ", shutting down..." << endl;
    g_stop_requested = true;
    if (g_main_loop && g_main_loop_is_running(g_main_loop)) {
        g_main_loop_quit(g_main_loop);
    }
}

string createPipelineString(const string& srt_url) {
    stringstream pipeline;
    pipeline << "rtspsrc location=" << quoted("rtsp://127.0.0.1:8554/cam1") << " "
             << "protocols=tcp do-rtsp-keep-alive=true latency=300 ! "
             << "rtph264depay ! "
             << "h264parse config-interval=-1 ! "
             << "video/x-h264,stream-format=byte-stream,alignment=au ! "
             << "mpegtsmux ! "
             << "srtclientsink uri=" << quoted(srt_url) << " "
             << "streamid=publish:cam1 mode=caller wait-for-connection=false";

    return pipeline.str();
}

bool runPipeline() {
    g_restart_pipeline = false;

    GError* error = nullptr;
    const string& srt_url{"srt://127.0.0.1:8890"};
    string pipeline_str = createPipelineString(srt_url);
    
    cout << "Pipeline: " << pipeline_str << endl;
    
    GstElement* pipeline = gst_parse_launch(pipeline_str.c_str(), &error);
    
    if(!pipeline) {
        string error_msg = error ? error->message : "Unknown error";
        cerr << "Failed to create pipeline: " << error_msg << endl;
        if(error) {
            g_error_free(error);
        }
        return false;
    }

    if(error) {
        cerr << "Pipeline created with warnings: " << error->message << endl;
        g_error_free(error);
    }

    GstBus* bus = gst_element_get_bus(pipeline);
    if(!bus) {
        cerr << "Failed to get pipeline bus" << endl;
        gst_object_unref(pipeline);
        return false;
    }

    gst_bus_add_watch(bus, [](GstBus* bus, GstMessage* message, gpointer user_data) -> gboolean {
        GError* error = nullptr;
        gchar* debug = nullptr;

        switch (GST_MESSAGE_TYPE(message)) {
            case GST_MESSAGE_ERROR: {
                gst_message_parse_error(message, &error, &debug);
                cerr << "ERROR: " << (error ? error->message : "Unknown error") << endl;
                if (debug) cerr << "DEBUG: " << debug << endl;
                if (!g_stop_requested) {
                    cout << "Flagging pipeline for restart..." << endl;
                    g_restart_pipeline = true;
                }
                g_main_loop_quit(g_main_loop);
                break;
            }
            case GST_MESSAGE_EOS: {
                cout << "End of stream - restarting..." << endl;
                if (!g_stop_requested) {
                    g_restart_pipeline = true;
                }
                g_main_loop_quit(g_main_loop);
                break;
            }
            case GST_MESSAGE_WARNING: {
                gst_message_parse_warning(message, &error, &debug);
                cerr << "WARNING: " << (error ? error->message : "Unknown warning") << endl;
                if (debug) cerr << "DEBUG: " << debug << endl;
                break;
            }
            case GST_MESSAGE_INFO: {
                gst_message_parse_info(message, &error, &debug);
                cout << "INFO: " << (error ? error->message : "Unknown info") << endl;
                break;
            }
            default:
                break;
        }

        if (error) g_error_free(error);
        if (debug) g_free(debug);
        
        return TRUE;
    }, nullptr);

    cout << "Starting pipeline..." << endl;
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if(ret == GST_STATE_CHANGE_FAILURE) {
        cerr << "Failed to start pipeline" << endl;
        gst_object_unref(bus);
        gst_object_unref(pipeline);
        return false;
    }

    cout << "Pipeline started successfully" << endl;
    cout << "Publishing to SRT: " << srt_url << endl;
    cout << "Reading from RTSP: rtsp://127.0.0.1:8554/cam1" << endl;

    g_main_loop_run(g_main_loop);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return g_restart_pipeline; // Return true if we should restart
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    cout << "Starting Pipeline 2: RTSP -> SRT" << endl;

    gst_init(nullptr, nullptr);

    if (!gst_is_initialized()) {
        cerr << "Failed to initialize GStreamer" << endl;
        return 1;
    }

    g_main_loop = g_main_loop_new(nullptr, false);
    if(!g_main_loop) {
        cerr << "Failed to create GLib main loop" << endl;
        return 1;
    }

    cout << "Press Ctrl+C to stop..." << endl;

    while (!g_stop_requested) {
        bool should_restart = runPipeline();
        
        if (g_stop_requested) {
            break;
        }
        
        if (should_restart) {
            cout << "Restarting pipeline in 2 seconds..." << endl;
            this_thread::sleep_for(chrono::seconds(2));
        } else {
            cout << "Pipeline stopped without restart flag" << endl;
            break;
        }
    }

    cout << "Shutting down..." << endl;
    g_main_loop_unref(g_main_loop);
    cout << "Pipeline 2 stopped" << endl;
    return 0;
}