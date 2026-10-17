#!/bin/bash
# ==============================================================================
# The Simpsons: Hit & Run - macOS Build & Bundle Script
# ==============================================================================
set -e

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="$REPO_ROOT/build"
APP_NAME="The Simpsons Hit & Run.app"
APP_DIR="$BUILD_DIR/$APP_NAME"

echo "=========================================================="
echo "  Building The Simpsons: Hit & Run for macOS"
echo "=========================================================="

# 1. Check prerequisites
command -v cmake >/dev/null 2>&1 || { echo "❌ CMake not found. Run: brew install cmake"; exit 1; }
command -v ninja >/dev/null 2>&1 || { echo "❌ Ninja not found. Run: brew install ninja"; exit 1; }

echo "⚙️  Configuring CMake (Release build)..."
cmake -B "$BUILD_DIR" -S "$REPO_ROOT" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="arm64"

echo "🔨 Compiling SRR2 executable..."
ninja -C "$BUILD_DIR" SRR2

echo "📦 Creating macOS App Bundle ($APP_NAME)..."
rm -rf "$APP_DIR"
mkdir -p "$APP_DIR/Contents/MacOS"
mkdir -p "$APP_DIR/Contents/Resources"
mkdir -p "$APP_DIR/Contents/Frameworks"

# Copy binary
cp "$BUILD_DIR/code/SRR2" "$APP_DIR/Contents/MacOS/SRR2"

# Create launcher script
cat << 'EOF' > "$APP_DIR/Contents/MacOS/The Simpsons Hit & Run"
#!/bin/bash
# Automatically disable macOS diacritics press-and-hold popup in game
export SDL_MAC_PRESS_AND_HOLD=0
defaults write com.radical.simpsonshitandrun ApplePressAndHoldEnabled -bool false 2>/dev/null || true

DIR="$(cd "$(dirname "$0")/../Resources" && pwd)"
cd "$DIR"
exec "$DIR/../MacOS/SRR2" "$@"
EOF
chmod +x "$APP_DIR/Contents/MacOS/The Simpsons Hit & Run"

# Create Info.plist
cat << 'EOF' > "$APP_DIR/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleDisplayName</key>
    <string>The Simpsons Hit &amp; Run</string>
    <key>CFBundleExecutable</key>
    <string>The Simpsons Hit &amp; Run</string>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
    <key>CFBundleIdentifier</key>
    <string>com.radical.simpsonshitandrun</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>The Simpsons Hit &amp; Run</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSSupportsAutomaticGraphicsSwitching</key>
    <true/>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
</dict>
</plist>
EOF

# Build AppIcon.icns if iconutil is available and source icon exists
ICON_SRC="$REPO_ROOT/sce_sys/icon0.png"
if [ -f "$ICON_SRC" ]; then
    ICONSET_DIR="/tmp/SimpsonsIcon.iconset"
    rm -rf "$ICONSET_DIR"
    mkdir -p "$ICONSET_DIR"
    sips -z 16 16     "$ICON_SRC" --out "$ICONSET_DIR/icon_16x16.png" >/dev/null 2>&1 || true
    sips -z 32 32     "$ICON_SRC" --out "$ICONSET_DIR/icon_16x16@2x.png" >/dev/null 2>&1 || true
    sips -z 32 32     "$ICON_SRC" --out "$ICONSET_DIR/icon_32x32.png" >/dev/null 2>&1 || true
    sips -z 64 64     "$ICON_SRC" --out "$ICONSET_DIR/icon_32x32@2x.png" >/dev/null 2>&1 || true
    sips -z 128 128   "$ICON_SRC" --out "$ICONSET_DIR/icon_128x128.png" >/dev/null 2>&1 || true
    sips -z 256 256   "$ICON_SRC" --out "$ICONSET_DIR/icon_128x128@2x.png" >/dev/null 2>&1 || true
    sips -z 256 256   "$ICON_SRC" --out "$ICONSET_DIR/icon_256x256.png" >/dev/null 2>&1 || true
    sips -z 512 512   "$ICON_SRC" --out "$ICONSET_DIR/icon_256x256@2x.png" >/dev/null 2>&1 || true
    sips -z 512 512   "$ICON_SRC" --out "$ICONSET_DIR/icon_512x512.png" >/dev/null 2>&1 || true
    iconutil -c icns "$ICONSET_DIR" -o "$APP_DIR/Contents/Resources/AppIcon.icns" >/dev/null 2>&1 || true
    rm -rf "$ICONSET_DIR"
fi

# Bundle Dynamic Libraries if dylibbundler is installed
if command -v dylibbundler >/dev/null 2>&1; then
    echo "📚 Bundling dynamic libraries via dylibbundler..."
    dylibbundler -of -cd -b -x "$APP_DIR/Contents/MacOS/SRR2" \
        -d "$APP_DIR/Contents/Frameworks/" \
        -p "@executable_path/../Frameworks/"
    
    # Fix SDL3 dynamic loader dependency for sdl2-compat
    BREW_PREFIX="$(brew --prefix 2>/dev/null || echo '/opt/homebrew')"
    if [ -f "$BREW_PREFIX/lib/libSDL3.0.dylib" ]; then
        cp -f "$BREW_PREFIX/lib/libSDL3.0.dylib" "$APP_DIR/Contents/Frameworks/libSDL3.0.dylib"
        cp -f "$BREW_PREFIX/lib/libSDL3.dylib" "$APP_DIR/Contents/Frameworks/libSDL3.dylib" 2>/dev/null || true
        cp -f "$BREW_PREFIX/lib/libSDL3.0.dylib" "$APP_DIR/Contents/MacOS/libSDL3.0.dylib"
        cp -f "$BREW_PREFIX/lib/libSDL3.dylib" "$APP_DIR/Contents/MacOS/libSDL3.dylib" 2>/dev/null || true
    fi
fi

# Ad-hoc sign bundle
echo "🔏 Signing application bundle..."
codesign --force --deep --sign - "$APP_DIR"

echo "✅ App bundle created at: $APP_DIR"
