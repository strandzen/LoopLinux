# Loop Lite — Project Status

_Updated continuously as work progresses. Last update: core functionality confirmed working end-to-end live — hold F24, drag to a direction (sticky), see a transparent outline + center indicator, release to snap. (See user's own [Todo.md](Todo.md) for the full feature backlog this tracks against.)_

## Working

- **keyd** — installed, enabled, active. `/etc/keyd/default.conf` maps `capslock = f24` system-wide.
- **Build system** — verified by actually compiling and linking against the real KWin 6.7.5 dev headers on this machine.
- **Rebuild/reload — critical, do not forget**: toggling the effect off/on in kwinrc + `qdbus6 reconfigure` does **not** reload the compiled `.so`. `systemctl --user restart plasma-kwin_wayland.service` **causes a hard lockup on this machine** — never run it. The only confirmed-safe reload path is a full, normal logout/login through Plasma's own menu. Build-and-install (safe, no logout needed until actually testing):
  ```
  cmake --build /mnt/Storage/Projects/LoopLinux/build && sudo cmake --install /mnt/Storage/Projects/LoopLinux/build
  ```
- **Core interaction confirmed working end-to-end, live**: hold F24 (Caps Lock) → drag mouse to pick a direction (sticky — relaxing the hand back toward center doesn't cancel it) or press an arrow key → see the transparent bordered outline at the target position plus the small center indicator showing the direction name → release F24 → active window snaps to that position on its own screen. Esc and no-direction-release both cancel cleanly. Hyperkey+Enter maximizes, Hyperkey+Backspace minimizes.
- **Rendering architecture**: two independent `QQuickWindow`s (`Outline.qml`, `Indicator.qml`), not `QuickSceneEffect` (which was confirmed to always render opaque, a hard constraint of that class). Plain `QQuickWindow`s get real transparency through KWin's normal per-window compositing path, the same mechanism KWin uses for its own decorations/tooltips/task-switcher. This is the settled approach.
- **Effect design** (`src/loopliteeffect.{h,cpp}`), a plain `KWin::Effect`:
  - F24 global shortcut (`KGlobalAccel`) → `arm()`, guarded against OS key-repeat re-entrancy.
  - Exclusive keyboard grab + mouse interception while armed.
  - Direction detection (`src/snapcalculator.{h,cpp}`): pure function, pointer delta → 8-way direction past a configurable deadzone, sticky once set.
  - Arrow keys set direction directly (cardinal only).
  - Snap via `Window::moveResize()` within `clientArea(MaximizeArea, screen)` of the active window's own screen.
  - Config (`src/loopliteconfig.kcfg`): `EnableTopBottomHalves`, `DirectionDeadzone`, in kwinrc.
  - Debug logging (`qCWarning(LOOP_LITE)`) still present — should be trimmed now that things are stable.

## Missing / Not done yet (from Todo.md's "nice to fix / polish")

- **No real radial menu** — indicator is currently a circle with text, not a directional wedge/pointer visual.
- **No KCM settings page**: padding/gaps around placed windows, corner radii, outline thickness, outline color (system color picker), configurable keybinds (WASD/HJKL/etc. beyond arrows — needs generalizing the current hardcoded arrow-key table into a config-driven binding system).
- **No git repo, no packaging/install instructions beyond this file.**
- Debug logging should be trimmed/removed now that the core behavior is confirmed stable.

## Next steps (suggested order, not yet confirmed with user)

1. Trim debug logging now that things are stable.
2. Build the real radial-menu visual (replace the plain circle with directional wedges).
3. KCM config page for the Todo.md settings list, including the configurable-keybind generalization.
4. Consider a git repo for proper history now that the core design has stabilized.
