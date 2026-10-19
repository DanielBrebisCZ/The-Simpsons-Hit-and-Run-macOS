#!/bin/bash
# ==============================================================================
# The Simpsons: Hit & Run - macOS Asset Installer
# ==============================================================================
# This script copies your original PC game assets into the macOS app bundle.
# No copyrighted game assets are distributed with this port.
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
APP_NAME="The Simpsons Hit & Run.app"

echo "=========================================================="
echo "  The Simpsons: Hit & Run - macOS Asset Installer"
echo "=========================================================="
echo ""

# 1. Locate the App Bundle
TARGET_APP=""
if [ -d "$SCRIPT_DIR/$APP_NAME" ]; then
    TARGET_APP="$SCRIPT_DIR/$APP_NAME"
elif [ -d "$SCRIPT_DIR/../../build/$APP_NAME" ]; then
    TARGET_APP="$SCRIPT_DIR/../../build/$APP_NAME"
elif [ -d "/Applications/$APP_NAME" ]; then
    TARGET_APP="/Applications/$APP_NAME"
fi

# 2. Get Source PC Assets Directory
SRC_DIR="$1"

if [ -z "$SRC_DIR" ]; then
    echo "No asset folder path provided via command line."
    echo "Opening native folder selection dialog..."
    
    # Prompt via AppleScript GUI
    SRC_DIR=$(osascript -e 'try
        set selectedFolder to choose folder with prompt "Select your original The Simpsons: Hit & Run (PC) game folder:"
        POSIX path of selectedFolder
    on error
        return ""
    end try')
fi

# Clean trailing slash
SRC_DIR="${SRC_DIR%/}"

if [ -z "$SRC_DIR" ] || [ ! -d "$SRC_DIR" ]; then
    echo "❌ Error: No valid game folder selected. Installation cancelled."
    exit 1
fi

echo "📁 Selected asset source: $SRC_DIR"

# 3. Validate Assets
# A valid PC installation must contain at least scripts, dialog, or rcf archives
HAS_RCF=$(find "$SRC_DIR" -maxdepth 2 -iname "*.rcf" 2>/dev/null | head -n 1)
HAS_SCRIPTS=$(find "$SRC_DIR" -maxdepth 2 -iname "scripts" 2>/dev/null | head -n 1)
HAS_ART=$(find "$SRC_DIR" -maxdepth 2 -iname "art" 2>/dev/null | head -n 1)

if [ -z "$HAS_RCF" ] && [ -z "$HAS_SCRIPTS" ] && [ -z "$HAS_ART" ]; then
    echo "❌ Error: The selected folder does not appear to contain valid PC game assets."
    echo "Expected files/folders like: art/, scripts/, sound/, or *.rcf archives."
    osascript -e 'display alert "Invalid Game Folder" message "The selected folder does not appear to contain original PC game files.\n\nPlease select the folder containing art/, scripts/, sound/, or *.rcf files." as critical' 2>/dev/null || true
    exit 1
fi

# 4. If Target App doesn't exist, check if we need to copy from /Applications or fail
if [ -z "$TARGET_APP" ] || [ ! -d "$TARGET_APP" ]; then
    echo "❌ Error: '$APP_NAME' could not be found."
    echo "Please place this script next to '$APP_NAME' or install it to /Applications first."
    osascript -e 'display alert "App Bundle Not Found" message "Could not find The Simpsons Hit & Run.app.\nPlease place this installer next to the .app bundle." as critical' 2>/dev/null || true
    exit 1
fi

APP_SUPPORT_DIR="$HOME/Library/Application Support/The Simpsons Hit and Run"
mkdir -p "$APP_SUPPORT_DIR"

echo "📂 Target destination: $APP_SUPPORT_DIR"
echo "⏳ Installing game assets (this may take 10-30 seconds)..."

# Fast APFS clone / copy
copy_item() {
    local src="$1"
    local dest="$2"
    if [ -e "$src" ]; then
        # Try APFS copy-on-write clone first (-c), fallback to recursive copy
        cp -Rc "$src" "$dest" 2>/dev/null || cp -R "$src" "$dest"
    fi
}

# Copy top-level RCF archives
find "$SRC_DIR" -maxdepth 1 -iname "*.rcf" -exec cp -f {} "$APP_SUPPORT_DIR/" \;

# Copy directories if present (case-insensitive search)
for dir_name in "art" "scripts" "sound" "movies"; do
    FOUND_DIR=$(find "$SRC_DIR" -maxdepth 1 -iname "$dir_name" 2>/dev/null | head -n 1)
    if [ -n "$FOUND_DIR" ] && [ -d "$FOUND_DIR" ]; then
        echo "  -> Copying $dir_name/..."
        # Lowercase destination directory name
        DEST_SUBDIR="$APP_SUPPORT_DIR/$(echo "$dir_name" | tr '[:upper:]' '[:lower:]')"
        mkdir -p "$DEST_SUBDIR"
        cp -Rc "$FOUND_DIR"/* "$DEST_SUBDIR/" 2>/dev/null || cp -R "$FOUND_DIR"/* "$DEST_SUBDIR/" 2>/dev/null || true
    fi
done

# Copy simpsons.ini if present
INI_FILE=$(find "$SRC_DIR" -maxdepth 1 -iname "simpsons.ini" 2>/dev/null | head -n 1)
if [ -n "$INI_FILE" ]; then
    cp -f "$INI_FILE" "$APP_SUPPORT_DIR/simpsons.ini"
fi

# Note: App bundle remains clean, untouched, and ready for Gatekeeper / Notarization!

# 7. Offer to move to /Applications if not already there
if [ "$TARGET_APP" != "/Applications/$APP_NAME" ]; then
    echo "📋 Would you like to install the game to /Applications? (y/n)"
    read -r -t 10 INSTALL_TO_APPS || INSTALL_TO_APPS="y"
    if [[ "$INSTALL_TO_APPS" =~ ^[Yy] ]]; then
        echo "  -> Installing to /Applications/$APP_NAME..."
        rm -rf "/Applications/$APP_NAME"
        cp -Rc "$TARGET_APP" "/Applications/$APP_NAME" 2>/dev/null || cp -R "$TARGET_APP" "/Applications/$APP_NAME"
        codesign --force --deep --sign - "/Applications/$APP_NAME"
        echo "  ✓ Installed to /Applications!"
    fi
fi

echo ""
echo "=========================================================="
echo "🎉 SUCCESS! The Simpsons: Hit & Run is ready to play!"
echo "=========================================================="
echo ""

# Native notification
osascript -e 'display notification "Game assets installed successfully! You can now launch The Simpsons: Hit & Run." with title "Installation Complete" sound name "Glass"' 2>/dev/null || true

exit 0
