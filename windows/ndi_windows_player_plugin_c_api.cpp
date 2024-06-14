#include "include/ndi_windows_player/ndi_windows_player_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "ndi_windows_player_plugin.h"

void NdiWindowsPlayerPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  ndi_windows_player::NdiWindowsPlayerPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
