#!/bin/bash

#MacOS Icon builder
sips -z 16 16 player.png --out icon_16x16.png
sips -z 32 32 player.png --out icon_32x32.png
sips -z 64 64 player.png --out icon_64x64.png
sips -z 128 128 player.png --out icon_128x128.png
sips -z 256 256 player.png --out icon_256x256.png
sips -z 512 512 player.png --out icon_512x512.png
sips -z 1024 1024 player.png --out icon_1024x1024.png
cd ..
iconutil -c icns icon.iconset