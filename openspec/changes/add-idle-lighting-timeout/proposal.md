# Proposal

## Why

The keyboard lighting stays on for as long as the board has USB power, even
while nobody is touching it. Vial cannot fix this. Its QMK Settings tab has no
lighting timeout, and QMK's built-in idle timeout only exists for RGB Matrix,
not for the RGBLight subsystem this board uses. Firmware support is the only
way to get it.

## What Changes

- All 59 LEDs fade to black after 3 minutes with no key press and no trackball
  movement.
- The next key press or trackball movement fades the lighting back in.
- The fade-out and fade-in reuse the existing 250 ms lighting fade and its
  split synchronization, so both halves stay in step.
- The timeout is always on and fixed at 3 minutes in firmware. It has no Vial
  setting or keycode, and it works the same whether the Auto Mouse fade
  setting is on or off.
- Going dark lowers only the runtime output brightness. RGBLight's enable
  state, hue, saturation, brightness, effect, and speed are left alone and
  nothing is written to EEPROM.
- The key press or trackball movement that wakes the lighting is still sent
  to the host as normal.

## Capabilities

### New Capabilities
- `lighting-idle-timeout`: Turns keyboard lighting off after a fixed period
  with no input and restores it on the next input, without changing saved
  lighting settings.

### Modified Capabilities
<!-- None: there are no existing specs. The Auto Mouse fade behavior is
     unchanged; this change only adds a second reason to go dark. -->

## Impact

- `firmware/keymaps/vial_right_custom/keymap.c`: the master half's fade
  target decision gains an idle condition that is independent of the
  Auto Mouse fade setting.
- New platform-independent idle module in
  `firmware/keymaps/vial_right_custom/`, added to `rules.mk`.
- New host test registered in `scripts/test.sh`.
- `docs/custom-firmware.md`: describes the behavior and adds hardware
  verification steps.
- No change to the split wire protocol, Vial keycodes, EEPROM layout, or the
  diagnostic keymap.
