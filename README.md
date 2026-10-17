# Art Agent Mobile

Native Geode mod for Geometry Dash Android 2.2081, targeting Geode 5.6.1. Current source version: **0.6.0**.

## Current implementation

- Editor-only floating shortcut and configurable keyboard binding.
- Prompt field with offline style and substyle keyword parsing (English and Spanish terms). Steampunk supports clockwork/gear wording; sci-fi supports futuristic/spaceship wording; ruins support abandoned structures; cyberpunk supports neon; underwater supports ocean themes; infernal supports demon/hell themes; celestial supports angel/heaven themes; industrial supports factory/warehouse themes.
- Uses the current editor selection as a bounding area for newly generated decoration.
- LOW / MED / HIGH detail controls.
- Eighteen offline decoration patterns: architecture, diamond, ring/frame, wave, star, linear trim, tech circuits, gothic/medieval arches, crystal facets, organic curves, steampunk gears, sci-fi panels, ancient ruins, neon cyberpunk, underwater reef, infernal spikes, celestial halos, and industrial machinery.
- Detail and density affect spacing in generated patterns.
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
