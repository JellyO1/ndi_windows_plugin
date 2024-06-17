# ndi_windows_player

A native windows implementation of NDI® stream decoding leveraging the `flutter::TextureRegistrar`.

## Usage

```
import 'package:ndi_windows_player/ndi_windows_player.dart';

...

 @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('NDIPlayer example'),
        ),
        body: const Center(
          child: AspectRatio(
            aspectRatio: 16 / 9,
            child: NdiPlayer(
              name: "MACHINENAME (NDINAME)",
              bandwidth: Bandwidth.high,
            ),
          ),
        )
      ),
    );
  }
```

NDI® is a registered trademark of Vizrt NDI AB
