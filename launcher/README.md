# C to Triggers Launcher (Android prototype)

This is a separate Android app project in the ArtAgentMobile repository. It is designed as a launcher shell for Geometry Dash 2.2090 and does not require Geode to be installed.

## What it currently does

- Provides a launcher screen with an **Open Geometry Dash** button.
- Can request Android's explicit **Display over other apps** permission and show a movable floating **GPTM** button.
- Tapping GPTM opens a panel with practice/player, visuals/HUD, and editor/tools categories.
- The switches are interface placeholders only; they do not change Geometry Dash gameplay yet.
- The launcher tries to open the installed package `com.robtopx.geometryjump`. If your build uses a different package ID, the launch button will report that it cannot find the app.

See [GPTM roadmap](../docs/GPTM_ROADMAP.md) for the planned feature set and integration stages.

## Important: native integration is not implemented

Android apps are sandboxed. A normal launcher cannot grant itself “absolute permission” over Geometry Dash or inject native code into its process just by starting the game. The overlay permission only allows a window to be drawn over other apps; it does not grant process access. This prototype does **not** patch the game, inject a library, execute arbitrary C code, or create Geometry Dash triggers yet. GPTM's current switches only demonstrate the interface and are not functional game mods. Opening Geometry Dash directly will not activate any injection because there is no injector in this build.

The next stage is to establish a device-compatible, explicitly opt-in integration path and connect the C-like command parser to a safe trigger-plan UI. Any implementation that requires root, a repackaged game, or a specific ABI must be designed and tested separately; this source does not claim compatibility with every Android device.

## Build the APK

The GitHub Actions workflow `.github/workflows/build-launcher.yml` builds a debug APK and uploads it as an artifact. Open the repository's **Actions** tab, select **Build C to Triggers Launcher APK**, and download the `c-to-triggers-launcher-debug-apk` artifact after a successful run.

A successful build only verifies that the Android project compiles. Test installation and overlay permission behavior on your device before relying on it.
