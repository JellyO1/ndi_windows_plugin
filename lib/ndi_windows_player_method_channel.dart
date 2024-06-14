import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'ndi_windows_player_platform_interface.dart';

/// An implementation of [NdiWindowsPlayerPlatform] that uses method channels.
class MethodChannelNdiWindowsPlayer extends NdiWindowsPlayerPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('ndi_windows_player');

  @override
  Future<int> initialize(String name, int bandwidth) async {
    final textureId = await methodChannel.invokeMethod<int>('Ndi.init', { 'playerId': name, 'bandwidth': bandwidth });
    return textureId!;
  }

  @override
  Future dispose(String name) async {
    await methodChannel.invokeMethod('Ndi.dispose', {'playerId': name});
  }
}
