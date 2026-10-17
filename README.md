# Art Agent Mobile

Native Geode mod for Geometry Dash Android 2.2081, targeting Geode 5.6.1.

## Current implementation

- Editor-only Art Agent shortcut and keyboard binding configuration.
- Prompt field and in-editor panel.
- Preview/insert workflow for a simple, fixed pattern of native block objects.
- GitHub Actions workflow targeting Android32 and Android64.

## Known limitations — not a complete release

This repository does NOT yet implement the complete Art Agent specification. The current preview is only a message describing a fixed pattern; it is not a true rendered preview of the exact inserted objects. The current insert operation creates a fixed pattern of block objects at a fixed coordinate, regardless of prompt meaning. It does not capture the current selection, adapt decoration to existing geometry, provide a target-layer picker, preserve Group IDs or Hide relationships, create a draggable floating button, or provide robust undo/transaction handling. These capabilities must not be represented as finished.

The current code is a development build and must be compiled and tested on both Android architectures and then tested in Geometry Dash before it can be described as ready to install.

## Build

Open Actions → Build Android Geode mod. Successful jobs upload .geode artifacts for Android32 and Android64. A successful compile alone does not prove in-game behavior.

## Design requirements

The intended finished mod should provide prompt-based generation of editable native Geometry Dash objects, preview the generated objects before insertion, operate on user-selected geometry, and preserve relevant Group IDs and Hide/layout behavior without damaging unrelated level content. It should be free to use and not require paid APIs or ads.
