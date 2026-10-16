# ArtAgentMobile

Free, Android-first, AI-assisted decoration workflow for Geometry Dash **2.2081**, built as a native Geode mod.

## Current status

This repository is an early scaffold, not a finished decoration generator.

- [x] Geode project metadata for Android / Geometry Dash 2.2081
- [x] CMake project and C++ mod entry point
- [x] GitHub Actions workflow for Android 32-bit and 64-bit build attempts
- [x] Main-menu entry point with a prototype information dialog
- [ ] Confirm successful cloud build and download the `.geode` artifact
- [ ] Prompt-to-decoration specification parser
- [ ] Preview generated objects before insertion
- [ ] Insert native Geometry Dash editor objects
- [ ] Image-to-decoration workflow
- [ ] Group ID preservation and Hide-layout compatibility

## Build on GitHub Actions

Open the **Actions** tab and select **Build Android Geode mod**. The workflow attempts Android32 and Android64 builds and uploads each resulting `.geode` package as an artifact.

If a build fails, open the failed job and inspect its first compiler/configuration error before changing SDK versions.

## Design goals

- Android-first; do not require a local PC.
- Generate ordinary Geometry Dash objects rather than a custom level format.
- Preview before insertion and avoid silently overwriting level content.
- Preserve the original layout's Hide property and reuse corresponding Group IDs for decoration, when applicable.
- Keep text/image generation modular so a local or remote generation backend can be added later.

## Important

The current C++ code only adds a small entry point and an informational dialog. It does **not** generate or insert decoration yet. Do not treat a successful build as proof that the planned features are complete.
