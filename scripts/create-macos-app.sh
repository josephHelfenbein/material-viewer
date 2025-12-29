#!/bin/bash
set -e

# Change to project root (script is in scripts/)
cd "$(dirname "$0")/.." || exit 1
PROJECT_ROOT="$(pwd)"

APP_NAME="Material Viewer"
APP_DIR="${PROJECT_ROOT}/build/${APP_NAME}.app"
BINARY_NAME="materialviewer"
ICON_PATH="${PROJECT_ROOT}/src/resources/materialviewer-logo.png"

echo "Building materialviewer..."
cmake -S "${PROJECT_ROOT}" -B "${PROJECT_ROOT}/build"
cmake --build "${PROJECT_ROOT}/build"

echo "Creating macOS app bundle..."

# Clean and create bundle structure
rm -rf "${APP_DIR}"
mkdir -p "${APP_DIR}/Contents/MacOS"
mkdir -p "${APP_DIR}/Contents/Resources"

# Copy binary
cp "${PROJECT_ROOT}/build/${BINARY_NAME}" "${APP_DIR}/Contents/MacOS/"

# Copy resources (shaders, textures, etc.)
cp -r "${PROJECT_ROOT}/src/shaders" "${APP_DIR}/Contents/Resources/"
cp -r "${PROJECT_ROOT}/src/environments" "${APP_DIR}/Contents/Resources/"
cp -r "${PROJECT_ROOT}/src/material" "${APP_DIR}/Contents/Resources/"
cp -r "${PROJECT_ROOT}/src/models" "${APP_DIR}/Contents/Resources/"
cp -r "${PROJECT_ROOT}/src/resources" "${APP_DIR}/Contents/Resources/"
cp -r "${PROJECT_ROOT}/src/ui" "${APP_DIR}/Contents/Resources/"

# Copy icon if it exists
if [ -f "${ICON_PATH}" ]; then
    # Convert PNG to ICNS if sips is available
    mkdir -p "${APP_DIR}/Contents/Resources/icon.iconset"
    sips -z 16 16     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_16x16.png" 2>/dev/null || true
    sips -z 32 32     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_16x16@2x.png" 2>/dev/null || true
    sips -z 32 32     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_32x32.png" 2>/dev/null || true
    sips -z 64 64     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_32x32@2x.png" 2>/dev/null || true
    sips -z 128 128   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_128x128.png" 2>/dev/null || true
    sips -z 256 256   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_128x128@2x.png" 2>/dev/null || true
    sips -z 256 256   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_256x256.png" 2>/dev/null || true
    sips -z 512 512   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_256x256@2x.png" 2>/dev/null || true
    sips -z 512 512   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_512x512.png" 2>/dev/null || true
    sips -z 1024 1024 "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/icon.iconset/icon_512x512@2x.png" 2>/dev/null || true
    iconutil -c icns "${APP_DIR}/Contents/Resources/icon.iconset" -o "${APP_DIR}/Contents/Resources/materialviewer.icns" 2>/dev/null || true
    rm -rf "${APP_DIR}/Contents/Resources/icon.iconset"
fi

# Create Info.plist
cat > "${APP_DIR}/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>${BINARY_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>com.materialviewer.app</string>
    <key>CFBundleName</key>
    <string>Material Viewer</string>
    <key>CFBundleDisplayName</key>
    <string>Material Viewer</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleIconFile</key>
    <string>materialviewer</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSSupportsAutomaticGraphicsSwitching</key>
    <true/>
</dict>
</plist>
EOF

# Bundle Qt frameworks and fix library paths
echo "Bundling Qt frameworks..."
if command -v /opt/homebrew/opt/qt@5/bin/macdeployqt &> /dev/null; then
    /opt/homebrew/opt/qt@5/bin/macdeployqt "${APP_DIR}" -always-overwrite
elif command -v macdeployqt &> /dev/null; then
    macdeployqt "${APP_DIR}" -always-overwrite
else
    echo "Warning: macdeployqt not found. Qt frameworks not bundled."
    echo "Install Qt5 via Homebrew: brew install qt@5"
fi

# Ad-hoc code sign (required for Apple Silicon)
echo "Code signing..."
codesign --deep --force --sign - "${APP_DIR}"

echo ""
echo "Created ${APP_DIR}"
echo ""
echo "To create a DMG for distribution, run:"
echo "  hdiutil create -volname 'Material Viewer' -srcfolder '${APP_DIR}' -ov -format UDZO ${PROJECT_ROOT}/build/MaterialViewer.dmg"
