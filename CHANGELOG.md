# Changelog

All notable user-visible changes to the custom Keyball44 firmware are recorded
here. Versions follow [Semantic Versioning](https://semver.org/).

## [1.3.0-rc.1] - 2026-09-21

### Changed

- Enabled Auto Mouse by default for fresh or reset configuration, targeting
  layer 1 with a 500 ms timeout. Explicit enablement and timeout values saved
  in EEPROM remain authoritative across ordinary boots.
- Set the default trackball scrolling divider to `6`, a 1/32 movement
  denominator. Explicit divider values saved in EEPROM remain unchanged.
- Updated `KBC_RST` to restore Auto Mouse enabled with its 500 ms timeout and
  Div6 scrolling defaults.

### Safety and validation

- Host tests and the pinned QMK firmware build pass.
- Hardware acceptance remains pending for the fresh/reset defaults, `KBC_RST`,
  and persistence of deliberately saved overrides.

## [1.2.1] - 2026-09-02

### Changed

- Set the default trackball scrolling divider to `7`, a 1/64 movement
  denominator. Explicit divider values saved in EEPROM remain unchanged; the
  new default applies only when the stored configuration uses the `0` sentinel.

## [1.2.0] - 2026-09-01

### Added

- Vial-assignable `HEATMAP_TOG` mode. Each of the 44 switches independently
  raises its mapped LED by 10% per press, up to 100%, then loses exactly 10%
  at each decay interval until dark.
- Host coverage for initialization, invalid indices, independent keys,
  accumulation and saturation, all decay boundaries, exact extinction,
  retrigger timing, synchronized speed retiming, live brightness scaling, and
  the extended split mode and packet data.

### Changed

- Extended the existing global switch-to-LED map and 12-byte split protocol
  to carry heatmap presses and synchronized speed-change timestamps.
- The four shared speed levels select heatmap decrement intervals of 1.2, 0.9,
  0.6, and 0.3 seconds from slowest to fastest. Changing speed preserves the
  current levels and restarts all lit-key countdowns together.

### Safety and validation

- Heatmap state starts empty, resets on mode changes, remains runtime-only,
  and does not alter the published v1.1.1 artifact or checksum.

## [1.1.1] - 2026-08-30

### Changed

- Shortened the `INVERSE_TOG` opposite-color hold from 1,000 ms to 100 ms.
  The four configured crossfade durations are unchanged.

## [1.1.0] - 2026-08-30

### Added

- Vial-assignable `INVERSE_TOG` mode. Every LED displays the live RGBLight
  base color; a pressed key changes its associated LED to the opposite hue for
  one second, then crossfades it back independently.
- Direct matrix-to-LED assignments for all 39 switches with dedicated LEDs,
  plus nearest-underglow fallbacks for the five thumb switches without one.
- Unit coverage for inverse hue wrapping, timing, all fade speeds, blending,
  expiry, retriggering, invalid indices, overlapping keys, the complete
  matrix-to-LED map, and the extended split packet.

### Changed

- Extended keypress synchronization with the resolved global LED index while
  retaining coordinates for ripple and splash.
- The shared four-level effect speed now selects inverse return fades of 1.4,
  1.1, 0.85, and 0.6 seconds from slowest to fastest.

### Safety and validation

- The custom mode remains boot-disabled and non-persistent, and this release
  has passed host tests and the pinned QMK build.

## [1.0.0] - 2026-08-30

### Added

- Vial-assignable `RIPPLE_TOG` and `SPLASH_TOG` controls for hollow ripple and
  filled splash keypress animations.
- Physical 59-LED coordinate map derived from the official PCB data and
  validated on both assembled halves.
- Split synchronization for key origins, active lighting mode, and speed, with
  retries and a periodic state heartbeat.
- Four normalized animation-speed levels controlled by Vial's existing effect
  speed keycodes.

### Changed

- Corrected the RGBLight LED count to 30 LEDs on the left and 29 on the right.
- Preserved the selected stock RGBLight effect while a custom effect is active
  and restore it when the custom effect is disabled.
- Retained fractional trackball movement during scroll conversion so higher
  dividers no longer discard slow movement.
- Generalized the release artifact name from `ripple` to `custom` because the
  firmware now contains changes beyond the original ripple effect.

### Safety and validation

- Custom lighting modes start disabled after every boot and are not persisted
  to EEPROM.
- The earlier ripple-only build passed hardware validation. This release still
  requires the full hardware validation checklist in
  [docs/custom-firmware.md](docs/custom-firmware.md).
