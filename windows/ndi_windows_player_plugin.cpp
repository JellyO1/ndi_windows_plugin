#include "ndi_windows_player_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include "ndi_player.hpp"

#include <memory>
#include <sstream>

namespace ndi_windows_player {

// static
void NdiWindowsPlayerPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "ndi_windows_player",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<NdiWindowsPlayerPlugin>(registrar->texture_registrar());

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

//NdiWindowsPlayerPlugin::NdiWindowsPlayerPlugin() {};
NdiWindowsPlayerPlugin::NdiWindowsPlayerPlugin(flutter::TextureRegistrar *textureRegistar) : textureRegistrar(textureRegistar) {}

NdiWindowsPlayerPlugin::~NdiWindowsPlayerPlugin() {}

void NdiWindowsPlayerPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    /* Sets lambda for Ndi.init callbacks. Called after creating the widget. */
    if (method_call.method_name() == "Ndi.init") {
        flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
        std::string playerId = std::get<std::string>(arguments[flutter::EncodableValue("playerId")]);

        NDIlib_recv_bandwidth_e bandwidth = NDIlib_recv_bandwidth_highest;
        switch (std::get<int32_t>(arguments[flutter::EncodableValue("bandwidth")])) {
            case 0:
                bandwidth = NDIlib_recv_bandwidth_lowest;
                break;
            case 1:
                bandwidth = NDIlib_recv_bandwidth_highest;
                break;
        };

        NdiPlayer* player = players->get(playerId, bandwidth);

        // There's no state for this playerId yet so create, otherwise just return the existing textureId.
        if (this->states.find(playerId) == this->states.end()) {
            NdiState state;

            state.player_count = 1;
            state.frame = new uint8_t[player->videoHeight * player->videoWidth * 4];
            memset(state.frame, 0, player->videoHeight * player->videoWidth * 4 * sizeof(uint8_t));
            state.buffer = new FlutterDesktopPixelBuffer();
            state.buffer->height = player->videoHeight;
            state.buffer->width = player->videoWidth;
            state.buffer->buffer = state.frame;
            state.texture = std::make_unique<flutter::TextureVariant>(
                    flutter::PixelBufferTexture(
                            [=](size_t width, size_t height) -> const FlutterDesktopPixelBuffer* {
                                return this->states.at(playerId).buffer;
                            }
                    )
            );
            state.textureId = this->textureRegistrar->RegisterTexture(state.texture.get());
            this->textureRegistrar->MarkTextureFrameAvailable(state.textureId);
            this->states[playerId] = std::move(state);
            player->onFrame(
                    [=](uint8_t* frame) -> void {
                        // The resolution changed. Recreate frame buffer.
                        if (player->videoHeight != this->states[playerId].buffer->height || player->videoWidth != this->states[playerId].buffer->width) {
                            delete this->states[playerId].frame;

                            this->states[playerId].frame = new uint8_t[player->videoHeight * player->videoWidth * 4];
                            memset(this->states[playerId].frame, 0, player->videoHeight * player->videoWidth * 4 * sizeof(uint8_t));
                            this->states[playerId].buffer = new FlutterDesktopPixelBuffer();
                            this->states[playerId].buffer->width = player->videoWidth;
                            this->states[playerId].buffer->height = player->videoHeight;
                            this->states[playerId].buffer->buffer = this->states[playerId].frame;
                        }

                        memcpy(this->states[playerId].frame, frame, player->videoHeight * player->videoWidth * 4 * sizeof(uint8_t));
                        this->textureRegistrar->MarkTextureFrameAvailable(this->states[playerId].textureId);
                    }
            );
        }
        else {
            this->states[playerId].player_count++;
        }

        result->Success(flutter::EncodableValue(this->states[playerId].textureId));
    }
    else if (method_call.method_name() == "Ndi.dispose") {
        flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
        std::string playerId = std::get<std::string>(arguments[flutter::EncodableValue("playerId")]);

        this->states[playerId].player_count--;

        if (this->states[playerId].player_count == 0) {
            players->dispose(playerId);

            delete this->states[playerId].frame;
            delete this->states[playerId].buffer;
            this->states.erase(playerId);
        }

        result->Success(flutter::EncodableValue(nullptr));
    }
    else {
        result->NotImplemented();
    }
}

}  // namespace ndi_windows_player
