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
    
    # Also create document icon for .mat files (same as app icon)
    cp "${APP_DIR}/Contents/Resources/materialviewer.icns" "${APP_DIR}/Contents/Resources/mat-document.icns" 2>/dev/null || true
fi

# Create Info.plist with document type association for .mat files
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
    
    <!-- Document Types - Associates .mat files with Material Viewer -->
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeName</key>
            <string>Material Viewer Document</string>
            <key>CFBundleTypeRole</key>
            <string>Editor</string>
            <key>CFBundleTypeIconFile</key>
            <string>mat-document</string>
            <key>LSItemContentTypes</key>
            <array>
                <string>com.materialviewer.mat</string>
            </array>
            <key>LSHandlerRank</key>
            <string>Owner</string>
        </dict>
    </array>
    
    <!-- Exported UTI - Declares the .mat file type -->
    <key>UTExportedTypeDeclarations</key>
    <array>
        <dict>
            <key>UTTypeIdentifier</key>
            <string>com.materialviewer.mat</string>
            <key>UTTypeDescription</key>
            <string>Material Viewer Document</string>
            <key>UTTypeConformsTo</key>
            <array>
                <string>public.data</string>
            </array>
            <key>UTTypeIconFile</key>
            <string>mat-document</string>
            <key>UTTypeTagSpecification</key>
            <dict>
                <key>public.filename-extension</key>
                <array>
                    <string>mat</string>
                </array>
                <key>public.mime-type</key>
                <string>application/x-materialviewer</string>
            </dict>
        </dict>
    </array>
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

# Bundle non-Qt dynamic libraries
echo "Bundling other dynamic libraries..."
mkdir -p "${APP_DIR}/Contents/Frameworks"

BINARY="${APP_DIR}/Contents/MacOS/${BINARY_NAME}"

# Function to get the library install name
get_lib_id() {
    otool -D "$1" 2>/dev/null | tail -1
}

# Function to fix a library's dependencies recursively
fix_lib_deps() {
    local lib="$1"
    local deps=$(otool -L "$lib" 2>/dev/null | grep -E "^\s+/(opt/homebrew|usr/local)" | awk '{print $1}')
    
    for dep in $deps; do
        local dep_name=$(basename "$dep")
        local dest="${APP_DIR}/Contents/Frameworks/${dep_name}"
        
        # Copy if not already present
        if [ ! -f "$dest" ] && [ -f "$dep" ]; then
            echo "  Copying $dep_name"
            cp "$dep" "$dest"
            chmod 755 "$dest"
            
            # Fix the copied library's own id
            install_name_tool -id "@executable_path/../Frameworks/${dep_name}" "$dest" 2>/dev/null || true
            
            # Recursively fix its dependencies
            fix_lib_deps "$dest"
        fi
        
        # Update reference in the current library
        install_name_tool -change "$dep" "@executable_path/../Frameworks/${dep_name}" "$lib" 2>/dev/null || true
    done
}

# Fix the main binary
fix_lib_deps "$BINARY"

# Fix all libraries in Frameworks (including those added by macdeployqt)
for fw in "${APP_DIR}/Contents/Frameworks"/*.dylib; do
    if [ -f "$fw" ]; then
        fix_lib_deps "$fw"
    fi
done

# Ad-hoc code sign (required for Apple Silicon)
echo "Code signing..."
codesign --deep --force --sign - "${APP_DIR}"

echo ""
echo "Created ${APP_DIR}"
echo ""
echo "To create a DMG for distribution, run:"
echo "  scripts/create-macos-dmg.sh"
echo ""
echo "Or manually:"
echo "  hdiutil create -volname 'Material Viewer' -srcfolder '${APP_DIR}' -ov -format UDZO ${PROJECT_ROOT}/build/MaterialViewer.dmg"
