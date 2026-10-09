# Loop-lite – Loop-inspired window management for KDE Plasma

Goal: a Linux/KDE Plasma 6 (Wayland) version of the macOS app Loop (github.com/mrkai77/Loop).
This is a reimplementation, not a code fork: Loop is Swift/AppKit. We carry over the idea and UX only.

## Technical approach
- Implemented as a **KWin effect in C++** (Qt6/KF6, CMake), built out-of-tree.
- Why an effect: it can grab the keyboard (`effects->grabKeyboard()`) and receive **both press
  and release** via `grabbedKeyboardEvent()`. It can also draw overlays directly in the
  compositor and move windows with `setFrameGeometry()`. Regular KWin scripts only get key presses.
- NOTE: KWin's effect API is internal and unstable. Verify every API name against the installed
  KWin headers, and expect rebuilds or small fixes on Plasma updates (CachyOS is rolling release).

## Hyperkey (hard dependency: keyd)
- Caps Lock is mapped to F24 via keyd:

- The project only ships the snippet (`/usr/share/<project>/keyd.conf`). Do NOT write to
  `/etc/keyd/`, since it may conflict with the user's `default.conf`.
- The user enables keyd themselves: `sudo systemctl enable --now keyd`.
- PKGBUILD: `depends=('keyd' 'kwin')`.
- No Meta fallback in v1, but keep trigger logic isolated in one small class (activate/confirm)
  so a fallback can be added later.

## User flow
1. **F24 (Caps) pressed:** the effect activates, stores the cursor position as the origin,
   grabs the keyboard, intercepts mouse clicks and shows a radial menu at the cursor.
2. **Mouse moves:** compute angle and distance from the origin.
   - Inside the dead zone, nothing is selected (release = cancel).
   - Outside the dead zone, 8 sectors of 45° each: left, right, top-left, top-right,
     bottom-left, bottom-right, plus top/bottom (top/bottom half; not finalized).
   - The radial menu highlights the selected sector, and an **outline of the target geometry**
     is drawn on screen.
3. **Other keys while Caps is held** (handled directly by the effect via the grab):
   - Space maximizes.
   - Backspace minimizes.
   - Esc cancels.
   - Make it easy to add more bindings.
4. **F24 released:** apply the selected action, release keyboard and mouse, remove the overlay.

## Rules
- Always acts on **the active window**.
- Zones are computed **only on the screen containing the active window**, using
  `clientArea(MaximizeArea, …)` so panels are respected.
- Direction is relative to the origin. The cursor may end up on another screen without the
  window changing screens.
- Ignore F24 autorepeat while held.

## First task
Scaffold the project: CMake, effect class and plugin, F24 trigger with grab and release,
sector calculation, outline drawing, Space/Backspace/Esc handling, and a README with keyd setup.

## Writing style
- When writing README.md, do not use em dashes (—). Use regular dashes (-) instead.