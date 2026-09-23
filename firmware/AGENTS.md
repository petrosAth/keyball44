# Firmware guidance

## Overlay model

- Files under `keymaps/` are repository-owned overlays. `scripts/build.sh` copies one overlay into the exact Vial-QMK revision pinned in `dependencies.lock`; do not patch `external/vial-qmk` instead.
- `vial_right_custom` is the production keymap. `vial_right_ledmap` is a temporary one-LED-at-a-time diagnostic and must not acquire production-only behavior.
- Both keymaps deliberately include the pinned upstream `vial_right` config and keymap. Preserve the macro-renaming hooks around the upstream include so the local layer and OLED callbacks can delegate correctly.

## Firmware contracts

- Preserve the physical lighting model: 59 global LEDs, split as 30 on the left and 29 on the right. Local driver indices and global effect indices are different on the right half.
- Treat `ripple_packet_t` as a wire protocol. It remains packed and exactly 12 bytes unless both halves, validation, and compatibility expectations are deliberately migrated together.
- Keep split state normalized before use, register user transaction IDs through `SPLIT_TRANSACTION_IDS_USER`, and account for failed sends, retries, and heartbeat resynchronization.
- Preserve unsigned timer wraparound behavior by using QMK timer helpers or subtraction-based elapsed-time logic. Avoid dynamic allocation and floating point in runtime paths.
- Custom effects must respect live RGB enable, hue, value, and speed settings, clamp brightness to `RGBLIGHT_LIMIT_VAL`, and restore the stock effect when disabled.
- Changes to LED coordinates or matrix-to-LED mappings must stay consistent with `docs/led-map.csv` and `docs/led-mapping.md`.

## Structure and validation

- Keep platform-independent state machines and calculations in small `.c`/`.h` modules. Keep QMK callbacks, hardware access, and split transport orchestration in `keymap.c`.
- Add or update host tests for behavior changes, including invalid inputs, exact timing boundaries, speed extremes, simultaneous events, and index limits where relevant.
- Add new production sources to `vial_right_custom/rules.mk` and register their host tests in `scripts/test.sh` when applicable.
- Run `make test`. Also run `make firmware` after changes to QMK-facing code, configuration, source lists, Vial custom keycodes, or split interfaces. Run `make diagnostic` when the diagnostic keymap changes.
