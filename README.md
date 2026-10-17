# 🍩 The Simpsons: Hit & Run — Native macOS Port

[![Platform: macOS](https://img.shields.io/badge/Platform-macOS%20%28Apple%20Silicon%20%7C%20Intel%29-blue.svg)](#)
[![Architecture: arm64 / x86_64](https://img.shields.io/badge/Architecture-arm64%20%7C%20x86__64-orange.svg)](#)
[![License](https://img.shields.io/badge/License-Non--Commercial%20Educational-lightgrey.svg)](#)

A native, modern macOS port of the classic 2003 action-adventure game ***The Simpsons: Hit & Run***, running smoothly at 60+ FPS on Apple Silicon (M1, M2, M3, M4, M5, M6) and modern Intel Macs.

> [!IMPORTANT]
> **LEGAL NOTICE & ASSET REQUIREMENT**  
> This repository contains **NO copyrighted game assets** (no textures, 3D models, sound effects, music, or dialog). To play the game, you **must provide your own copy of the original PC game files** (from CD, ISO, or digital backup).

---

## ✨ Features

- **🚀 Native Apple Silicon (M1–M6):** Compiled natively for `arm64`, running with maximum efficiency and zero Rosetta 2 translation overhead.
- **🖱️ Restored Frontend Mouse Cursor:** Full authentic yellow glove mouse cursor in all menus, options, and settings with mouse navigation and clicking.
- **🎮 Controller & Gamepad Support:** Plug-and-play support for Xbox Wireless, PlayStation DualShock 4 / DualSense, and Nintendo Switch Pro controllers via SDL.
- **🔊 Modern Audio & Video:** Powered by modern OpenAL Soft for 3D positional audio and FFmpeg for cutscene playback.
- **🍎 macOS Polish:** Automatically prevents the macOS "Press and Hold" diacritics popup when holding down WASD driving keys.
- **📦 Clean Standalone Bundle:** Self-contained macOS `.app` bundle with all necessary dynamic libraries bundled internally.
- **🛠️ One-Click Asset Installer:** Simple double-clickable script to import your PC assets and assemble the app in seconds.

---

## 🚀 Quick Start for Players (Prebuilt Release)

You do **not** need to compile any code to play!

1. Go to the **[Releases](https://github.com/daniiichan/The-Simpsons-Hit-and-Run-macOS/releases)** tab and download the latest `The.Simpsons.Hit.and.Run.macOS.arm64.v1.0.0.zip`.
2. Extract the ZIP file.
3. Double-click **`Install The Simpsons Hit & Run.command`**.
4. When the native macOS folder picker appears, select the folder containing your original **PC version** of *The Simpsons: Hit & Run* (must contain `art/`, `sound/`, `scripts/`, or `*.rcf` files).
5. The installer will copy and organize the assets, sign the app, and offer to place it into `/Applications`.
6. Launch and enjoy!

---

## 🛠️ Building from Source

If you want to contribute or build the port yourself from source on macOS:

### 1. Install Dependencies
Ensure you have [Homebrew](https://brew.sh) installed, then run:

```bash
brew install cmake ninja sdl2 sdl2-compat openal-soft ffmpeg libpng dylibbundler
```

### 2. Clone the Repository
```bash
git clone https://github.com/daniiichan/The-Simpsons-Hit-and-Run-macOS.git
cd The-Simpsons-Hit-and-Run-macOS
```

### 3. Build the App Bundle
Run the automated build script:

```bash
./tools/macos/build_app.sh
```

This compiles the `SRR2` executable, generates the Retina `AppIcon.icns`, sets up `Info.plist`, bundles all dynamic libraries via `dylibbundler`, and signs the `.app` bundle into `build/The Simpsons Hit & Run.app`.

### 4. Install Assets into Your Build
```bash
./tools/macos/install_assets.sh /path/to/your/PC/Simpsons/folder
```

---

## 🕹️ Controls & Cheats

### Default Keyboard Controls
| Action | Key |
|---|---|
| **Accelerate / Move Forward** | `W` / `Up Arrow` |
| **Brake / Reverse / Move Backward** | `S` / `Down Arrow` |
| **Steer Left** | `A` / `Left Arrow` |
| **Steer Right** | `D` / `Right Arrow` |
| **Handbrake / Jump** | `Space` |
| **Action / Enter Vehicle / Kick** | `E` / `Right Ctrl` |
| **Look Back / Reset Car** | `R` |
| **Horn / Nitro** | `H` / `Left Shift` |
| **Pause Menu** | `Esc` |

### Cheats
Enter cheats in the **Options** menu in the main frontend:
See [CHEATS.md](CHEATS.md) for the complete list of cheats, button combinations, and secret cars.

---

## 📂 Project Structure

```text
├── code/               # Core game engine source code (Radical Entertainment)
├── libs/               # Engine libraries (Pure3D, Scrooby, radcore, radsound, etc.)
├── tools/
│   └── macos/          # macOS build, packaging, and asset installer scripts
│       ├── build_app.sh
│       ├── install_assets.sh
│       ├── Install The Simpsons Hit & Run.command
│       └── package_release.sh
├── sce_sys/            # Icon and metadata assets
├── CMakeLists.txt      # Main CMake build system configuration
├── README.md           # Project documentation
├── CHEATS.md           # Cheats and secret vehicle codes
└── KNOWN_ISSUES.md     # Known issues and roadmap
```

---

## 🙏 Credits & Acknowledgments

- **Radical Entertainment & Vivendi Universal Games** — The original creators of *The Simpsons: Hit & Run* (2003).
- **ZenoArrows** — Creator of the original modern homebrew ports for Nintendo Switch and PS Vita.
- **daniiichan** — Native macOS Apple Silicon port, mouse cursor restoration, macOS fixes, and packaging.
- **The Simpsons: Hit & Run Modding Community** — For documentation, reverse engineering, and keeping this masterpiece alive.

---

## ⚖️ Legal Disclaimer

This project is a source port created for preservation, educational, and non-commercial purposes.  
*The Simpsons* and *The Simpsons: Hit & Run* are trademarks and copyrights of **20th Century Studios**, **Electronic Arts**, and **Radical Entertainment**. No proprietary game data, audio, music, dialog, or artwork is included or distributed in this repository.
