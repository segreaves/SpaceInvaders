#!/bin/bash

#MacOS app release

APP_NAME="SpaceInvaders"
BUILD_DIR="."  # Adjust to your release build output directory
BUNDLE_DIR="${APP_NAME}.app"
CONTENTS_DIR="${BUNDLE_DIR}/Contents"
MACOS_DIR="${CONTENTS_DIR}/MacOS"
RESOURCES_DIR="${CONTENTS_DIR}/Resources"

# Create directories
mkdir -p "${MACOS_DIR}"
mkdir -p "${RESOURCES_DIR}/assets"
mkdir -p "${RESOURCES_DIR}/libraries"

# Copy executable
cp "${BUILD_DIR}/${APP_NAME}" "${MACOS_DIR}/"

# Copy assets (adjust source asset directory)
cp -R x64/Release/assets/* "${RESOURCES_DIR}/assets/"

# Copy SFML dylibs (adjust paths to your SFML dylibs)
cp /opt/homebrew/lib/*.dylib "${RESOURCES_DIR}/libraries/"

# Copy Info.plist
cp Info.plist "${CONTENTS_DIR}/"

# Copy icon.icns (adjust path to your icon file)
cp icon.icns "${RESOURCES_DIR}/"

# Fix dylib paths in executable and dylibs
install_name_tool -change /path/to/sfml/lib/libsfml-graphics.dylib @executable_path/../Resources/libraries/libsfml-graphics.dylib "${MACOS_DIR}/${APP_NAME}"
install_name_tool -change /path/to/sfml/lib/libsfml-window.dylib @executable_path/../Resources/libraries/libsfml-window.dylib "${MACOS_DIR}/${APP_NAME}"
install_name_tool -change /path/to/sfml/lib/libsfml-system.dylib @executable_path/../Resources/libraries/libsfml-system.dylib "${MACOS_DIR}/${APP_NAME}"
# Repeat for other dylibs as needed

echo "App bundle created at ${BUNDLE_DIR}"

hdiutil create -volname "SpaceInvaders" -srcfolder SpaceInvaders.app -ov -format UDZO SpaceInvaders.dmg

echo "Dmg file created at $(pwd)/SpaceInvaders.dmg"