# Art Agent Mobile

Native Geode mod for Geometry Dash Android 2.2081, targeting Geode 5.6.1.

## Current implementation

- Editor-only floating shortcut and configurable keyboard binding.
- Prompt field with offline style keyword parsing (English and some Spanish).
- Uses the current editor selection as a bounding area for newly generated decoration.
- LOW / MED / HIGH detail controls.
- Architectural-style blockout generator with perimeter courses, corner supports, window-like bays and finer trim at higher detail.
- Adds ordinary editable Geometry Dash objects; it does not intentionally modify the selected layout objects.
- Deduplicates overlapping generated coordinates to reduce clutter at intersecting trim and frame lines.
- Caps each generation pass at 1,200 objects to reduce accidental editor overload; the status message warns when the cap is reached.
- GitHub Actions workflow targets Android32 and Android64.

## Important limitations

This is an early procedural decorator, not a full image-generating AI. “Realistic” currently means architectural structure suggested with repeated native block objects; it does not yet create advanced textured materials, lighting, shadows, custom color channels, or a true visual preview. The selected objects are used to calculate a bounding rectangle, so irregular layouts may receive imperfect decoration. Generated objects do not yet inherit Group IDs, Editor Layer assignments, Hide behavior, or trigger relationships automatically. Review the result before saving, and keep a backup of important levels.

The newest source commits trigger Android32 and Android64 builds through GitHub Actions. Check the latest run before installing; a successful compile alone does not prove in-game behavior.

## Build

Open Actions → Build Android Geode mod. Successful jobs upload .geode artifacts for Android32 and Android64. A successful compile alone does not prove in-game behavior.
