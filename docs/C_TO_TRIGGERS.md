# C to Triggers — prototype specification

This is the first parser layer for the planned Geode mod. It accepts a small command subset inspired by C syntax and turns it into a validated trigger plan. It does **not** compile or execute arbitrary C code.

## Example input

```c
wait(0.5);
move(12, 90, 0, 1.2);
rotate(12, 360, 2);
alpha(12, 0.25, 0.4);
toggle(12, 0);
```

## Commands in the first prototype

- `wait(seconds)`: pause in the generated sequence.
- `move(groupId, x, y, seconds)`: plan a Move Trigger for a group.
- `rotate(groupId, degrees, seconds)`: plan a Rotate Trigger.
- `alpha(groupId, opacity, seconds)`: plan an Alpha Trigger; opacity is 0–1.
- `toggle(groupId, enabled)`: plan a Toggle Trigger; enabled is 0 or 1.

Each command is one line. Arguments must be numeric literals. Variables, loops, pointers, includes, function calls and general C syntax are intentionally unsupported in this phase. Invalid lines produce diagnostics instead of being executed.

## Development stages

1. Parse and validate commands into a trigger plan.
2. Add an in-editor Geode panel with source editor, Validate and Preview actions.
3. Convert the validated plan into actual editable Geometry Dash trigger objects, with a review step before insertion.
4. Add timing/order support and more trigger types.
5. Test on the targeted Geometry Dash/Geode versions.

The parser is deliberately separated from editor insertion so that source text never runs as native code. Trigger-object construction and insertion are not implemented by this parser-only milestone.
