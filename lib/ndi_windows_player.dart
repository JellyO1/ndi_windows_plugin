import 'package:flutter/material.dart';

import 'ndi_windows_player_platform_interface.dart';

enum Bandwidth {
  low,
  high
}

class NdiPlayer extends StatefulWidget {
  // The name of the NDI stream
  final String name;

  // The quality of the NDI stream
  final Bandwidth bandwidth;

  const NdiPlayer({super.key, required this.name, this.bandwidth = Bandwidth.high});

  @override
  State<NdiPlayer> createState() => _NdiPlayerState();
}

class _NdiPlayerState extends State<NdiPlayer> {
  int? textureId;

  @override
  void initState() {
    super.initState();

    NdiWindowsPlayerPlatform.instance.initialize(widget.name, widget.bandwidth.index).then((textureId) {
      this.textureId = textureId;
      setState((){});
    });
  }

  @override
  void dispose() {
    NdiWindowsPlayerPlatform.instance.dispose(widget.name);

    super.dispose();
  }

  @override
  void didUpdateWidget(covariant NdiPlayer oldWidget) {
    super.didUpdateWidget(oldWidget);

    if(oldWidget.name != widget.name) {
      NdiWindowsPlayerPlatform.instance.dispose(oldWidget.name);
      NdiWindowsPlayerPlatform.instance.initialize(widget.name, widget.bandwidth.index).then((textureId) {
        this.textureId = textureId;
        setState(() {});
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return textureId != null ? Texture(
      textureId: textureId!,
      filterQuality: FilterQuality.medium,
    ) : Container(
      color: Colors.black,
    );
  }
}