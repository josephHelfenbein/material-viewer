#!/bin/bash
set -e

# Change to project root (script is in scripts/)
cd "$(dirname "$0")/.." || exit 1
PROJECT_ROOT="$(pwd)"

APP_NAME="materialviewer"
APP_DIR="${PROJECT_ROOT}/build/${APP_NAME}.AppDir"
BINARY_NAME="materialviewer"
ICON_PATH="${PROJECT_ROOT}/src/resources/materialviewer-logo.png"

echo "Building materialviewer..."
cmake -S "${PROJECT_ROOT}" -B "${PROJECT_ROOT}/build"
cmake --build "${PROJECT_ROOT}/build"

echo "Creating Linux AppDir..."

# Clean and create directory structure
rm -rf "${APP_DIR}"
mkdir -p "${APP_DIR}/usr/bin"
mkdir -p "${APP_DIR}/usr/lib"
mkdir -p "${APP_DIR}/src"

# Copy binary
cp "${PROJECT_ROOT}/build/${BINARY_NAME}" "${APP_DIR}/usr/bin/"

# Copy resources
cp -r "${PROJECT_ROOT}/src/shaders" "${APP_DIR}/src/"
cp -r "${PROJECT_ROOT}/src/environments" "${APP_DIR}/src/"
cp -r "${PROJECT_ROOT}/src/material" "${APP_DIR}/src/"
cp -r "${PROJECT_ROOT}/src/models" "${APP_DIR}/src/"
cp -r "${PROJECT_ROOT}/src/resources" "${APP_DIR}/src/"
cp -r "${PROJECT_ROOT}/src/ui" "${APP_DIR}/src/"

# Create AppRun script
cat > "${APP_DIR}/AppRun" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
exec "${HERE}/usr/bin/materialviewer" "$@"
EOF
chmod +x "${APP_DIR}/AppRun"

# Create .desktop file
cat > "${APP_DIR}/${APP_NAME}.desktop" << EOF
[Desktop Entry]
Name=MaterialViewer
Exec=materialviewer
Icon=materialviewer
Type=Application
Categories=Graphics;
EOF

# Copy icon if it exists
if [ -f "${ICON_PATH}" ]; then
    cp "${ICON_PATH}" "${APP_DIR}/materialviewer.png"
    ln -sf materialviewer.png "${APP_DIR}/.DirIcon"
fi

# Bundle shared libraries
echo "Bundling shared libraries..."

# Get list of dependencies
DEPS=$(ldd "${APP_DIR}/usr/bin/${BINARY_NAME}" 2>/dev/null | grep "=> /" | awk '{print $3}' | sort -u)

# Libraries to skip (provided by base system)
SKIP_LIBS="linux-vdso|ld-linux|libc.so|libm.so|libdl.so|libpthread|librt.so|libresolv|libnss|libnsl"

for lib in $DEPS; do
    libname=$(basename "$lib")
    # Skip system libraries that should not be bundled
    if echo "$libname" | grep -qE "$SKIP_LIBS"; then
        continue
    fi
    # Copy if not already present
    if [ ! -f "${APP_DIR}/usr/lib/${libname}" ]; then
        echo "  Copying $libname"
        cp "$lib" "${APP_DIR}/usr/lib/" 2>/dev/null || true
    fi
done

# Also bundle Qt plugins if Qt is used
QT_PLUGIN_PATH=$(qmake -query QT_INSTALL_PLUGINS 2>/dev/null || echo "")
if [ -n "$QT_PLUGIN_PATH" ] && [ -d "$QT_PLUGIN_PATH" ]; then
    echo "Bundling Qt plugins..."
    mkdir -p "${APP_DIR}/usr/lib/qt5/plugins"
    for plugin_type in platforms xcbglintegrations; do
        if [ -d "$QT_PLUGIN_PATH/$plugin_type" ]; then
            cp -r "$QT_PLUGIN_PATH/$plugin_type" "${APP_DIR}/usr/lib/qt5/plugins/"
        fi
    done
    
    # Create qt.conf to tell Qt where to find plugins
    cat > "${APP_DIR}/usr/bin/qt.conf" << EOF
[Paths]
Plugins = ../lib/qt5/plugins
EOF
fi

echo ""
echo "Created ${APP_DIR}"
echo ""
echo "To create an AppImage, install appimagetool and run:"
echo "  cd ${PROJECT_ROOT}/build && appimagetool ${APP_NAME}.AppDir MaterialViewer-x86_64.AppImage"
echo ""
echo "To install appimagetool:"
echo "  wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
echo "  chmod +x appimagetool-x86_64.AppImage"
