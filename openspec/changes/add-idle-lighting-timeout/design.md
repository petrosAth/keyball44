# Design

## Context

See proposal.md for motivation and specs/lighting-idle-timeout/spec.md for
required behavior.

The production keymap already has a master-driven lighting fade:

- `housekeeping_task_user()` on the master half calls
  `lighting_fade_should_suppress()`. When the result changes, it sets the fade
  target to `LIGHTING_FADE_DARK` (0) or `LIGHTING_FADE_FULL` (255) and sends
  the transition over `LIGHTING_FADE_TRANSACTION`.
- Both halves apply the fade through `lighting_fade_refresh()` ->
  `lighting_driver_set_brightness()`. This scales the driver output only.
  RGBLight state and EEPROM are untouched and effects keep rendering.
- The sync module retries a failed update every 16 ms and sends a heartbeat
  every 2 s.
- `lighting_fade_should_suppress()` currently returns
  `lighting_fade_enabled && get_auto_mouse_enable() && layer_active`, so
  turning off the fade setting switches off the whole dark path.

The pinned Vial-QMK records input activity in `quantum/keyboard.c`.
`last_input_activity_time()` is a `sync_timer` timestamp updated on key matrix
changes, encoder activity, and pointing device report changes. The master scans
both halves' matrices, and the user has confirmed that trackball movement
counts as activity on this board.

## Goals / Non-Goals

**Goals:**
- Add idle as a second, independent reason for the existing fade to go dark.
- Keep the idle decision in a small, host-testable module.
- Stay dark through arbitrarily long idle periods.

**Non-Goals:**
- Changing the OLED timeout or behavior when the host USB is suspended.
- Adding a Vial setting, a keycode, or EEPROM storage for the timeout.
- Swallowing the key press that wakes the lighting.
- Any change to the split packet format or the diagnostic keymap.

## Decisions

### Reuse the existing fade path, combined with OR

The master computes the dark target as
`auto_mouse_dark || idle_dark`, where `auto_mouse_dark` is the current
expression unchanged and `idle_dark` comes from the new module. The idle term
sits outside the `lighting_fade_enabled` check.

This gets split synchronization, retries, heartbeat recovery, smooth
reversal, and preservation of RGBLight settings with no new transport code.

Alternatives considered:
- `rgblight_disable_noeeprom()` / `rgblight_suspend()`: these interfere with
  the custom effect renderers (which treat a disabled RGBLight as black and
  restore stock mode). They also need their own split sync and would cut the
  lights abruptly instead of fading.
- Enabling RGB Matrix to use `RGB_MATRIX_TIMEOUT`: this is a different
  lighting subsystem and would replace the whole custom lighting stack.

### A latched idle state machine, not a plain elapsed-time comparison

New module `lighting_idle.c/.h`:

```c
#define LIGHTING_IDLE_TIMEOUT_MS 180000

typedef struct {
  uint32_t activity_at;
  bool dark;
} lighting_idle_t;

void lighting_idle_init(lighting_idle_t *idle, uint32_t activity_at);
bool lighting_idle_update(lighting_idle_t *idle, uint32_t activity_at,
                          uint32_t now);
```

`lighting_idle_update` behaves as follows:
- If `activity_at` differs from the stored value, there has been new input.
  Store it, clear `dark`, and return false.
- Otherwise, if not dark and `now - activity_at >= LIGHTING_IDLE_TIMEOUT_MS`
  (unsigned subtraction), set `dark`.
- Return `dark`.

The master calls it each housekeeping pass with `last_input_activity_time()`
and `sync_timer_read32()`.

Why latch: `last_input_activity_elapsed()` is a 32-bit difference. After
about 49.7 days of idle it wraps to a small value and would bring the lights
back. Latching on "dark until the activity timestamp changes" meets the spec
requirement for any idle length. Detecting input by a change in the
timestamp, rather than by elapsed time, also means wraparound of the timer
itself is handled by the unsigned subtraction alone.

Alternative considered: `last_input_activity_elapsed() >= timeout` directly
in `keymap.c`. It is simpler, but it fails the long-idle requirement and
puts timing logic where host tests can't reach it.

### Fixed timeout as a header constant

`LIGHTING_IDLE_TIMEOUT_MS` lives in `lighting_idle.h`, so the host test and
the firmware use the same value. There is no `#ifndef` override and no
`config.h` entry, because the user asked for a fixed 3-minute timeout with no
configuration.

### Master-only decision

The idle decision runs only inside the existing `is_keyboard_master()` branch.
The slave never evaluates idle state. It follows the synchronized fade target,
so the halves can't disagree. `SPLIT_ACTIVITY_ENABLE` is not needed.

## Risks / Trade-offs

- [Pointing activity only counts when the mouse report changes] → The user
  confirmed the trackball counts. Hardware verification in
  `docs/custom-firmware.md` includes trackball-only wake and keep-alive
  checks.
- [A held key with no other input lets the lights go dark after 3 minutes]
  → Accepted and specified. Only matrix changes count as activity.
- [Boot: the activity timestamp starts at 0] → `lighting_idle_init` is seeded
  with the current `last_input_activity_time()`. The first timeout is counted
  from boot, which is the expected behavior.
- [Idle dark and Auto Mouse dark interact] → OR combination. Each condition
  clears independently, and the fade target only changes when the combined
  result changes. Host tests cover overlap in the idle module. The combination
  itself is one boolean expression in `keymap.c`, covered by the hardware
  checklist.
- [Timestamp collision: new input recorded with exactly the stored
  timestamp] → This needs input exactly 2^32 ms after the last input with no
  housekeeping pass in between. Not reachable in practice. Accepted.

## Migration Plan

This is a firmware-only change with no stored state. Build with
`make firmware` and flash each half manually, following the existing install
and recovery docs. To roll back, flash the previous release image. There is
no EEPROM migration.
