# Loop Lite

A [Loop](https://github.com/MrKai77/Loop)-inspired directional window snapping tool for KDE Plasma 6.7+ / KWin.

Hold a hyperkey (Caps Lock, remapped to F24), drag the mouse or press a direction key to pick where the active window should go, release to snap it there — left/right halves, top/bottom halves, or one of the four quarter corners, previewed live with an on-screen outline and a direction indicator.

## Features

- Hold-and-drag or hold-and-press-a-key directional snapping to 8 positions (halves + corners), with live outline and indicator previews
- Direction selection is sticky — once picked, relaxing the mouse or releasing the keys doesn't cancel it; only picking a different direction changes it
- Arrow keys, WASD, or HJKL for direction (configurable preset), including combining two keys (e.g. Up+Left) for a corner
- Hyperkey+Enter maximizes, Hyperkey+Backspace minimizes
- Esc, or releasing with no direction chosen, cancels cleanly
- Extensive settings page (System Settings → Desktop Effects → Loop Lite): mouse deadzone, window padding, indicator size/position/corner radius/ring thickness/color, outline border thickness/corner radius/color, keybind scheme

## Requirements

- KDE Plasma 6.7+ (KWin with the Wayland session)
- [keyd](https://github.com/rvaiya/keyd) — hard dependency, used to remap Caps Lock to F24 at the input level
- Build dependencies: CMake, Extra CMake Modules (ECM), Qt6 (Core, Gui, Qml, Quick, DBus), KDE Frameworks 6 (Config, CoreAddons, GlobalAccel, KCMUtils), and KWin's own development headers

## Installation

### 1. Configure keyd

Create `/etc/keyd/default.conf`:

```ini
[ids]

*

[main]

capslock = f24
```

Then enable and start the service:

```sh
sudo systemctl enable --now keyd
```

This dedicates Caps Lock entirely to acting as the hyperkey.

### 2. Build and install the plugin

```sh
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

This installs both the KWin effect itself and its System Settings configuration page.

### 3. Enable the effect

Open **System Settings → Desktop Effects**, find **Loop Lite**, and enable it.

## Usage

1. Hold Caps Lock (F24).
2. Move the mouse toward an edge or corner, or press an arrow key (or WASD/HJKL, depending on the configured scheme) — the direction indicator and a preview outline show the current pick.
3. Release Caps Lock to snap the active window there, on its own screen (windows never move across monitors).

While held:
- **Enter** — maximize
- **Backspace** — minimize
- **Esc**, or releasing with no direction picked — cancel without moving the window

## Configuration

All settings live in **System Settings → Desktop Effects → Loop Lite** (the configure/gear icon next to the effect): mouse deadzone (horizontal/vertical), window padding, indicator appearance (visibility, spawn-at-cursor vs. centered, size, corner radius, ring thickness, text, color), outline appearance (visibility, border thickness, corner radius, color), and the keybind scheme (arrows/WASD/HJKL).

Settings changes take effect automatically the next time you hold the hyperkey — no need to log out or restart the effect.

## Known limitations

- Only one window is placed at a time — no multi-window "thirds" layouts yet
- Keybind schemes are fixed presets (arrows / WASD / HJKL), not arbitrary rebinding
- Snapping doesn't move a window to a different monitor

## License

GPL-2.0-or-later.
