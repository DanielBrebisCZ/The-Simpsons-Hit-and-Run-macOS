# 🐛 Known Issues & Technical Limitations

This document tracks known issues, edge cases, and future roadmap items for *The Simpsons: Hit & Run* macOS and multiplatform port.

---

## 🍎 macOS Specific Notes

- **Pause Settings Screen (In-Game):**
  - Opening the Settings menu while paused in gameplay can crash on some builds because the original PC Scrooby assets lack the console-specific `InvertCamControl` and `Vibration` UI text strings. (Accessing settings from the Main Menu frontend works properly).
- **ProMotion / High Refresh Rate Displays:**
  - The game is designed around 60 FPS. Running beyond 60 FPS may affect physics, AI vehicle timing, or vehicle jump velocity. The engine caps rendering to match the display VSync (60 Hz target recommended).
- **Window Resizing:**
  - Dynamic in-game window resizing is not supported by the original Pure3D viewport pipeline; full-screen or fixed desktop resolution is recommended.

---

## 🎮 Gameplay & Visuals

- **Race Banner Alignment:**
  - In rare instances when starting a race, HUD banners for adjacent street races or bonus missions may briefly appear offset to the right. Restarting the mission or reloading the area resets banner positions.
- **Bonus Missions:**
  - Bonus race checkpoints with time bets may occasionally display their initial start sign slightly shifted on wide aspect ratios.
- **FMV Cutscene Aspect Ratio:**
  - Cutscenes maintain their original 4:3 aspect ratio with pillarboxing to prevent visual distortion.

---

## 🤝 Reporting Bugs

If you find a new bug or regression on macOS, please feel free to open an issue on the GitHub repository with:
1. Your macOS version and Mac model (e.g. M1–M6 Apple Silicon or Intel).
2. Exact steps to reproduce the issue.
3. Relevant terminal log or crash report.
