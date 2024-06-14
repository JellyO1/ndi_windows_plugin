#pragma once
#include <functional>
#include <map>
#include <thread>
#include "include/ndi_windows_player/Processing.NDI.Lib.h"
#include <string>

class NdiPlayer
{
public:
    int videoWidth = 1920;
    int videoHeight = 1080;
    bool active = false;

    NdiPlayer(std::string name, NDIlib_recv_bandwidth_e bandwidth) : name(name), bandwidth(bandwidth) {
        init();
    };
    ~NdiPlayer() {
        this->active = false;
        this->receiver_thread.join();
    }

    void onFrame(std::function<void(uint8_t* frame)> callback) {
        _videoCallback = callback;
    }

    void dispose() {
        this->active = false;
    }

private:
    std::thread receiver_thread = {};
    NDIlib_recv_instance_t pNDI_recv = NULL;
    std::string name = NULL;
    NDIlib_recv_bandwidth_e bandwidth;
    std::function<void(uint8_t* frame)> _videoCallback = [=](uint8_t* frame) -> void {};

    void init() {
        if (!NDIlib_is_supported_CPU()) {
            OutputDebugString(L"CPU incompatible with NDI");
            //throw;
        }

        if (!NDIlib_initialize()) {
            OutputDebugString(L"Unable to initialize NDI");
            //throw;
        }

        NDIlib_source_t* source = (NDIlib_source_t*)calloc(1, sizeof(NDIlib_source_t));
        source->p_ndi_name = name.c_str();

        NDIlib_recv_create_v3_t* recv_t = (NDIlib_recv_create_v3_t*)calloc(1, sizeof(NDIlib_recv_create_v3_t));
        recv_t->color_format = NDIlib_recv_color_format_RGBX_RGBA;
        recv_t->bandwidth = this->bandwidth;
        recv_t->allow_video_fields = false;

        this->pNDI_recv = NDIlib_recv_create_v3(recv_t);
        NDIlib_recv_connect(this->pNDI_recv, source);

        receiver_thread = std::thread(&NdiPlayer::receiverThread, this);

        free(source);
        free(recv_t);
    }

    void receiverThread() {
        auto v_frame = (NDIlib_video_frame_v2_t*)malloc(sizeof(NDIlib_video_frame_v2_t));

        this->active = true;

        while (active) {
            switch (NDIlib_recv_capture_v3(this->pNDI_recv, v_frame, NULL, NULL, 1000)) {
            case NDIlib_frame_type_none:
                break;
            case NDIlib_frame_type_video:
                int yres = v_frame->yres;
                int xres = v_frame->xres;

                if (this->videoHeight != yres && this->videoWidth != xres) {
                    this->videoHeight = yres;
                    this->videoWidth = xres;
                }

                _videoCallback(v_frame->p_data);

                NDIlib_recv_free_video_v2(pNDI_recv, v_frame);

                break;
            }
        }

        free(v_frame);

        NDIlib_recv_destroy(this->pNDI_recv);
        //free(pNDI_recv);
    }
};

class Players {
public:
    NdiPlayer* get(std::string name, NDIlib_recv_bandwidth_e bandwidth = NDIlib_recv_bandwidth_highest) {
        if (this->players.find(name) == this->players.end()) {
            this->players[name] = new NdiPlayer(name, bandwidth);
        }
        return this->players[name];
    }

    void dispose(std::string name, std::function<void()> callback = []() -> void {}) {
        delete this->players[name];
        this->players.erase(name);
        callback();
    }

private:
    std::map<std::string, NdiPlayer*> players;
};


Players* players = new Players();


