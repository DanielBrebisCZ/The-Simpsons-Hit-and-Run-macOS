#!/bin/bash
# ==============================================================================
# The Simpsons: Hit & Run - macOS Release Packager
# ==============================================================================
# Creates an asset-free, clean release ZIP/DMG ready for GitHub Releases.
# ==============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$REPO_ROOT/build"
RELEASE_DIR="$BUILD_DIR/release"
APP_NAME="The Simpsons Hit & Run.app"

echo "=========================================================="
echo "  Packaging The Simpsons: Hit & Run (macOS Release)"
echo "=========================================================="

# Ensure the app is built
if [ ! -d "$BUILD_DIR/$APP_NAME" ]; then
    echo "App bundle not found in build directory. Building now..."
    "$SCRIPT_DIR/build_app.sh"
fi

rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR/payload"

echo "📦 Staging clean app bundle (excluding all proprietary assets)..."
cp -Rc "$BUILD_DIR/$APP_NAME" "$RELEASE_DIR/payload/$APP_NAME" 2>/dev/null || cp -R "$BUILD_DIR/$APP_NAME" "$RELEASE_DIR/payload/$APP_NAME"

# Ensure NO game assets or saves exist in the payload
rm -rf "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/art"
rm -rf "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/sound"
rm -rf "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/scripts"
rm -rf "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/movies"
rm -f "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/"*.rcf
rm -f "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/"Save*
rm -f "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/"save*
rm -f "$RELEASE_DIR/payload/$APP_NAME/Contents/Resources/simpsons.ini"

# Copy installer scripts
cp -f "$SCRIPT_DIR/install_assets.sh" "$RELEASE_DIR/payload/"
cp -f "$SCRIPT_DIR/Install The Simpsons Hit & Run.command" "$RELEASE_DIR/payload/"
chmod +x "$RELEASE_DIR/payload/install_assets.sh"
chmod +x "$RELEASE_DIR/payload/Install The Simpsons Hit & Run.command"

# Create simple user instructions
cat << 'EOF' > "$RELEASE_DIR/payload/README_INSTALL.txt"
================================================================================
The Simpsons: Hit & Run - Native macOS Port (Apple Silicon & Intel)
================================================================================

HOW TO INSTALL:
1. Double-click "Install The Simpsons Hit & Run.command".
2. When prompted, select your original PC installation folder of The Simpsons: Hit & Run.
   (The folder must contain the original game assets: art/, sound/, scripts/, or *.rcf).
3. The script will copy the assets into ~/Library/Application Support/The Simpsons Hit and Run/.
4. Launch the game from /Applications or directly from here!

LEGAL NOTICE:
This release contains NO proprietary game assets (textures, sounds, music, dialog).
You must provide your own legally obtained copy of the PC version of the game.
================================================================================
EOF

# Sign the payload app bundle
codesign --force --deep --sign - "$RELEASE_DIR/payload/$APP_NAME"

# Create ZIP archive
VERSION="v1.0.0"
ZIP_NAME="The.Simpsons.Hit.and.Run.macOS.arm64.$VERSION.zip"
echo "🗜️  Creating $ZIP_NAME..."
(cd "$RELEASE_DIR/payload" && zip -r -y -q "../$ZIP_NAME" .)

echo ""
echo "=========================================================="
echo "✅ Release package created successfully:"
echo "   $RELEASE_DIR/$ZIP_NAME"
echo "=========================================================="
