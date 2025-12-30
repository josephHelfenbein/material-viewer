#!/bin/bash
set -e

# Change to project root (script is in scripts/)
cd "$(dirname "$0")/.." || exit 1
PROJECT_ROOT="$(pwd)"

APP_NAME="Material Viewer"
APP_DIR="${PROJECT_ROOT}/build/${APP_NAME}.app"
DMG_NAME="MaterialViewer"
DMG_PATH="${PROJECT_ROOT}/build/${DMG_NAME}.dmg"
ICON_PATH="${PROJECT_ROOT}/src/resources/materialviewer-logo.png"
BINARY_NAME="materialviewer"

# Check if app bundle exists
if [ ! -d "${APP_DIR}" ]; then
    echo "Error: App bundle not found at ${APP_DIR}"
    echo "Please run scripts/create-macos-app.sh first."
    exit 1
fi

echo "Setting up document handler for .mat files..."

# Move the real binary and create a wrapper script
REAL_BINARY="${APP_DIR}/Contents/MacOS/${BINARY_NAME}"
REAL_BINARY_RENAMED="${APP_DIR}/Contents/MacOS/${BINARY_NAME}-bin"

# Only rename if not already done
if [ -f "${REAL_BINARY}" ] && [ ! -f "${REAL_BINARY_RENAMED}" ]; then
    mv "${REAL_BINARY}" "${REAL_BINARY_RENAMED}"
fi

# Create wrapper script that handles both direct launch and document open events
cat > "${REAL_BINARY}" << 'WRAPPER_EOF'
#!/bin/bash

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REAL_BINARY="${SCRIPT_DIR}/materialviewer-bin"

# If arguments were passed (command line), use them directly
if [ $# -gt 0 ]; then
    exec "${REAL_BINARY}" "$@"
fi

# No arguments - just launch the app
exec "${REAL_BINARY}"
WRAPPER_EOF

chmod +x "${REAL_BINARY}"

echo "Updating Info.plist with .mat file association..."

# Create document icon (same as app icon for .mat files)
DOC_ICON_PATH="${APP_DIR}/Contents/Resources/mat-document.icns"
if [ -f "${APP_DIR}/Contents/Resources/materialviewer.icns" ]; then
    cp "${APP_DIR}/Contents/Resources/materialviewer.icns" "${DOC_ICON_PATH}"
elif [ -f "${ICON_PATH}" ]; then
    # Generate document icon from PNG
    mkdir -p "${APP_DIR}/Contents/Resources/doc.iconset"
    sips -z 16 16     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_16x16.png" 2>/dev/null || true
    sips -z 32 32     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_16x16@2x.png" 2>/dev/null || true
    sips -z 32 32     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_32x32.png" 2>/dev/null || true
    sips -z 64 64     "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_32x32@2x.png" 2>/dev/null || true
    sips -z 128 128   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_128x128.png" 2>/dev/null || true
    sips -z 256 256   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_128x128@2x.png" 2>/dev/null || true
    sips -z 256 256   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_256x256.png" 2>/dev/null || true
    sips -z 512 512   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_256x256@2x.png" 2>/dev/null || true
    sips -z 512 512   "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_512x512.png" 2>/dev/null || true
    sips -z 1024 1024 "${ICON_PATH}" --out "${APP_DIR}/Contents/Resources/doc.iconset/icon_512x512@2x.png" 2>/dev/null || true
    iconutil -c icns "${APP_DIR}/Contents/Resources/doc.iconset" -o "${DOC_ICON_PATH}" 2>/dev/null || true
    rm -rf "${APP_DIR}/Contents/Resources/doc.iconset"
fi

# Create Info.plist with document type association for .mat files
# Note: NSServices and CFBundleURLTypes are NOT used - we handle files via command line
cat > "${APP_DIR}/Contents/Info.plist" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>materialviewer</string>
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
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>mat</string>
            </array>
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

echo "Info.plist updated with .mat file association."

# Create a drop script that handles files dropped on the app or opened via Finder
# This uses AppleScript to create a droplet behavior
cat > "${APP_DIR}/Contents/MacOS/droplet.scpt" << 'APPLESCRIPT_EOF'
on open theFiles
    set filePaths to ""
    repeat with aFile in theFiles
        set filePaths to filePaths & " " & quoted form of POSIX path of aFile
    end repeat
    
    set appPath to POSIX path of (path to me)
    set binPath to appPath & "Contents/MacOS/materialviewer-bin"
    
    do shell script quoted form of binPath & filePaths & " &> /dev/null &"
end open
APPLESCRIPT_EOF

# Compile the AppleScript
osacompile -o "${APP_DIR}/Contents/Resources/Scripts/droplet.scpt" "${APP_DIR}/Contents/MacOS/droplet.scpt" 2>/dev/null || true
rm -f "${APP_DIR}/Contents/MacOS/droplet.scpt"

# Create a proper launcher that handles Apple Events for document opening
# This is a small C program that will be compiled
LAUNCHER_SRC="${APP_DIR}/Contents/MacOS/launcher.m"
cat > "${LAUNCHER_SRC}" << 'OBJC_EOF'
#import <Cocoa/Cocoa.h>
#include <spawn.h>

extern char **environ;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic, strong) NSMutableArray *filesToOpen;
@property (nonatomic, assign) BOOL appLaunched;
@end

@implementation AppDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        _filesToOpen = [NSMutableArray array];
        _appLaunched = NO;
    }
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    self.appLaunched = YES;
    [self launchRealApp];
}

- (void)application:(NSApplication *)sender openFiles:(NSArray<NSString *> *)filenames {
    [self.filesToOpen addObjectsFromArray:filenames];
    if (self.appLaunched) {
        [self launchRealApp];
    }
}

- (BOOL)application:(NSApplication *)sender openFile:(NSString *)filename {
    [self.filesToOpen addObject:filename];
    if (self.appLaunched) {
        [self launchRealApp];
    }
    return YES;
}

- (void)launchRealApp {
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *binPath = [[bundle executablePath] stringByDeletingLastPathComponent];
    NSString *realBinary = [binPath stringByAppendingPathComponent:@"materialviewer-bin"];
    
    NSMutableArray *args = [NSMutableArray arrayWithObject:realBinary];
    for (NSString *file in self.filesToOpen) {
        [args addObject:file];
    }
    
    // Clear files after processing
    [self.filesToOpen removeAllObjects];
    
    // Convert to C strings
    int argc = (int)[args count];
    char **argv = malloc((argc + 1) * sizeof(char *));
    for (int i = 0; i < argc; i++) {
        argv[i] = strdup([[args objectAtIndex:i] UTF8String]);
    }
    argv[argc] = NULL;
    
    pid_t pid;
    int status = posix_spawn(&pid, argv[0], NULL, NULL, argv, environ);
    
    // Free memory
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
    
    if (status != 0) {
        NSLog(@"Failed to launch: %s", strerror(status));
    }
    
    // Quit the launcher
    [NSApp terminate:nil];
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // If arguments are passed directly (not from Finder), just exec the real binary
        if (argc > 1) {
            NSBundle *bundle = [NSBundle mainBundle];
            NSString *binPath = [[bundle executablePath] stringByDeletingLastPathComponent];
            NSString *realBinary = [binPath stringByAppendingPathComponent:@"materialviewer-bin"];
            
            char **newArgv = malloc((argc + 1) * sizeof(char *));
            newArgv[0] = strdup([realBinary UTF8String]);
            for (int i = 1; i < argc; i++) {
                newArgv[i] = (char *)argv[i];
            }
            newArgv[argc] = NULL;
            
            execv(newArgv[0], newArgv);
            perror("execv failed");
            return 1;
        }
        
        // No arguments - set up as NSApplication to receive Apple Events
        NSApplication *app = [NSApplication sharedApplication];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    return 0;
}
OBJC_EOF

# Compile the Objective-C launcher
echo "Compiling document handler..."
clang -fobjc-arc -framework Cocoa -o "${APP_DIR}/Contents/MacOS/${BINARY_NAME}" "${LAUNCHER_SRC}"
rm -f "${LAUNCHER_SRC}"

# Re-sign the app bundle after modifications
echo "Code signing..."
codesign --deep --force --sign - "${APP_DIR}"

# Remove existing DMG if present
if [ -f "${DMG_PATH}" ]; then
    echo "Removing existing DMG..."
    rm -f "${DMG_PATH}"
fi

echo "Creating DMG..."
hdiutil create -volname "${APP_NAME}" -srcfolder "${APP_DIR}" -ov -format UDZO "${DMG_PATH}"

echo ""
echo "================================================"
echo "DMG created successfully!"
echo "Location: ${DMG_PATH}"
echo ""
echo "Features:"
echo "  - .mat files will use Material Viewer's icon"
echo "  - Double-clicking .mat files will open them in Material Viewer"
echo "  - The file path is passed as an argument to the app"
echo ""
echo "Note: Users may need to right-click and 'Open' the first time"
echo "      due to macOS Gatekeeper security."
echo "================================================"
