#ifndef FLUTTER_PLUGIN_NDI_WINDOWS_PLAYER_PLUGIN_H_
#define FLUTTER_PLUGIN_NDI_WINDOWS_PLAYER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace ndi_windows_player {

struct NdiState {
    int64_t textureId;
    uint8_t player_count = 0;
    uint8_t* frame = NULL;
    FlutterDesktopPixelBuffer* buffer = NULL;
    std::unique_ptr<flutter::TextureVariant> texture;
};


class NdiWindowsPlayerPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  NdiWindowsPlayerPlugin(flutter::TextureRegistrar* textureRegistrar);
  NdiWindowsPlayerPlugin() = delete;

  virtual ~NdiWindowsPlayerPlugin();

  // Disallow copy and assign.
  NdiWindowsPlayerPlugin(const NdiWindowsPlayerPlugin&) = delete;
  NdiWindowsPlayerPlugin& operator=(const NdiWindowsPlayerPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
private:
    flutter::TextureRegistrar* textureRegistrar = nullptr;
    std::unordered_map<std::string, NdiState> states;
};

}  // namespace ndi_windows_player

#endif  // FLUTTER_PLUGIN_NDI_WINDOWS_PLAYER_PLUGIN_H_
