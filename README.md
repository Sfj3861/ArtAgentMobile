# Art Agent Mobile

A free, Android-first decoration-generator workflow for Geometry Dash **2.2081**, built as a native Geode mod.

## Current status

This is an early prototype, not a finished generator.

- [x] Geode project metadata for Android / Geometry Dash 2.2081
- [x] Cloud build workflow for Android32 and Android64
- [x] Editor-only Art Agent floating entry button
- [x] Cyan/pink accent choice in mod settings
- [x] Prototype panel explaining the intended decoration workflow
- [ ] Draggable floating button and configurable keyboard shortcut
- [ ] Actual prompt input (English first; Spanish and French later)
- [ ] Target-layer picker and selected-object capture
- [ ] Prompt-to-decoration generator
- [ ] Preview generated objects before insertion
- [ ] Insert native Geometry Dash editor objects
- [ ] Group ID preservation and Hide-layout compatibility
- [ ] Image-to-decoration workflow

## Intended workflow

1. Open a level in the editor and press the floating Art Agent button.
2. Select the existing objects to decorate and choose a target editor layer.
3. Describe the desired decoration in English (initial language).
4. Review a preview, then insert generated native Geometry Dash objects.

Steps 2-4 describe the intended workflow; they are not implemented yet in this prototype. The current panel only explains them.

## Build on GitHub Actions

Open the **Actions** tab and select **Build Android Geode mod**. The workflow builds Android32 and Android64 packages and uploads the `.geode` files as artifacts.

## Design goals

- Android-first; do not require a local PC.
- Generate ordinary Geometry Dash objects rather than a custom level format.
- Preview before insertion and avoid silently overwriting level content.
- Preserve the original layout's Hide property and reuse corresponding Group IDs for decoration, when applicable.
- Start with English prompts; consider Spanish and French after the initial workflow is stable.
- Keep the generator modular so a local or remote generation backend can be added later.

## Important

The current build adds an editor button and a prototype information panel only. It does **not** generate or insert decoration, select layers, capture selected objects, support dragging, or handle a keyboard shortcut yet. A successful build does not prove those planned features are complete.
