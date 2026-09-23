# Device profile

These values were reported by Vial while the keyboard was connected to the Linux host. Together, the USB strings, VID/PID, and Vial UID provide an exact match to the KeebNERD firmware source listed below.

## Identity

| Field                     | Value                                              |
| ------------------------- | -------------------------------------------------- |
| Manufacturer              | `Yowkees`                                          |
| Product                   | `Keyball44 by KeebNERD`                            |
| USB VID                   | `0x5957`                                           |
| USB PID                   | `0x0400`                                           |
| Vial keyboard ID          | `91926CA9CB216B8F`                                 |
| Vial UID bytes in source  | `{0x8F, 0x6B, 0x21, 0xCB, 0xA9, 0x6C, 0x92, 0x91}` |
| VIA protocol              | `9`                                                |
| Vial protocol             | `6`                                                |
| Observed Linux device     | `/dev/hidraw9` (not a stable path)                 |
| Controller and bootloader | RP2040                                             |
| Trackball                 | Right half                                         |
| Half interconnect         | TRRS serial link                                   |

Linux may assign a different `hidraw` number after reconnecting or rebooting; scripts must identify the keyboard by its USB identity, not by `/dev/hidraw9`.

## Vial capabilities

- 32 macro entries with 1024 bytes of macro storage.
- Macro delays and complex two-byte macro keycodes.
- 32 Tap Dance entries.
- 32 Combo entries.
- 32 Key Override entries.
- 32 Alt Repeat Key entries.
- Caps Word and Layer Lock.
- QMK Settings.
- Eight dynamic layers in the matching source configuration.

## Exact matching firmware

- Repository: `https://github.com/tangbonze/vial-qmk`
- Branch: `vial`
- Pinned commit: see [`dependencies.lock`](../dependencies.lock)
- Keyboard: `keyball/keyball44`
- Existing right-trackball keymap: `vial_right`
- Expected QMK target: `keyball/keyball44:vial_right`

Before building, verify that `git rev-parse HEAD` prints the pinned commit. Do not silently update the checkout to a newer branch head.

## Lighting and split configuration

- RGBLight is enabled with WS2812 LEDs.
- 59 LEDs total: 30 on the left (20 key LEDs and 10 underglow LEDs) and 29 on the right (19 key LEDs and 10 underglow LEDs).
- LED data pin: `GP0`.
- Serial split pin: `GP1`.
- Firmware brightness ceiling: 150.
- RGB Matrix is explicitly disabled in the matching source (`not work yet`).
- The `vial_right` keymap enables the standard RGBLight effects: breathing, rainbow mood, rainbow swirl, snake, knight, Christmas, static gradient, RGB test, alternating, and twinkle.

QMK RGB Matrix animations such as Solid Splash are algorithm references only. They are not drop-in effects for this board and do not justify enabling RGB Matrix.

## Compatibility boundary

The exact identifiers above must remain unchanged in custom builds. Preserve the controller, bootloader, matrix and split pins, Vial UID, feature counts, and the existing trackball/OLED behavior. Do not use firmware binaries built for Holykeebs, a different Keyball44 revision, or the upstream Yowkees tree.
