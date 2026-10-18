# GPTM — GPT Mod Menu

GPTM is the planned all-in-one quality-of-life and testing menu for the ArtAgentMobile / C-to-Triggers project. It is an original project and does not copy proprietary source code or assets from Eclipse, Mega Hack, or QOLmod.

## Current prototype

The Android overlay now displays a GPTM panel with grouped switches for:
- Practice/player: practice checkpoints, auto-retry, practice music sync, noclip testing, ignore-damage testing, speed control.
- Visuals/HUD: FPS, hitboxes, attempt count, screen shake, progress.
- Editor/tools: C-to-Triggers translator, object ID inspector, level stats, layout helper.

**These are UI placeholders.** Their switches currently only change their displayed state and show a notice; they do not alter Geometry Dash gameplay. The current launcher is not a native mod loader and does not inject into the game's process.

## Compatibility gate

Before native integration is attempted, verify the actual installed game's manifest/version, package ID, ABI, native library build and symbols. The uploaded APK filename is not authoritative evidence of the in-game version. Current repository code has not yet established a tested native hook path for GD 2.2090.

## Implementation roadmap

1. **GPTM shell:** categories, search, favourites, setting persistence, clear “not connected” status. (Current UI is an early shell.)
2. **Safe standalone utilities:** FPS display where measurable, attempt counter where the game exposes the relevant state, and configurable UI preferences.
3. **Native integration research:** identify supported extension points and confirm the target game's version/ABI. Do not assume overlay permission grants process access.
4. **Feature adapters:** implement each option against a verified API, with a capability check and a visible unsupported state when unavailable.
5. **C→T translator:** connect the existing restricted command parser to a reviewable trigger-plan preview; then implement actual editor-object creation only through verified game/editor APIs.
6. **Validation:** build in CI, test on a separate game copy, preserve original APK and levels, and record which features were actually tested.

## Engineering rules

- No arbitrary C/C++ source execution from user input. The current C-like parser is a constrained command translator.
- Never label a feature “working” until it is connected to game code and tested on the target version.
- Keep experimental gameplay-affecting features opt-in and separate from visual/QoL settings.
- A successful APK build verifies compilation only, not in-game compatibility.
