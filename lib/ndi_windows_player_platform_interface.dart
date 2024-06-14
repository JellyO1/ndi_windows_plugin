import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'ndi_windows_player_method_channel.dart';

abstract class NdiWindowsPlayerPlatform extends PlatformInterface {
  /// Constructs a NdiWindowsPlayerPlatform.
  NdiWindowsPlayerPlatform() : super(token: _token);

  static final Object _token = Object();

  static NdiWindowsPlayerPlatform _instance = MethodChannelNdiWindowsPlayer();

  /// The default instance of [NdiWindowsPlayerPlatform] to use.
  ///
  /// Defaults to [MethodChannelNdiWindowsPlayer].
  static NdiWindowsPlayerPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [NdiWindowsPlayerPlatform] when
  /// they register themselves.
  static set instance(NdiWindowsPlayerPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }


  Future<int> initialize(String name, int bandwidth) {
    throw UnimplementedError('initialize() has not been implemented.');
  }

  Future dispose(String name) {
    throw UnimplementedError('dispose() has not been implemented.');
  }
}
