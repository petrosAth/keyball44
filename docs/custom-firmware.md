# Custom firmware

## Releases

Tagged firmware is available from
[GitHub Releases](https://github.com/petrosAth/keyball44/releases). Download the
UF2 and `SHA256SUMS` from the same tagged release, then verify the UF2 against
that checksum file. The attached `SHA256SUMS` is the authoritative digest for
its release. Release artifacts are built on Linux x86_64, so a local build on
another platform will differ.

Release artifacts use the name
`keyball44-vial_right-custom-v<version>-<upstream-commit>.uf2`. The semantic
version comes from [`VERSION`](../VERSION), and the trailing commit identifies
the pinned upstream Vial-QMK source. `VERSION` describes the checked-out source;
it does not indicate whether a matching release has been published. The
[changelog](../CHANGELOG.md) records version-specific behavior and validation
status.

To prepare and publish a release:

1. Update `VERSION` to a stable `X.Y.Z` version or a numbered release candidate
   in the form `X.Y.Z-rc.N`, where `N` starts at 1.
2. Add a dated entry to `CHANGELOG.md` describing the firmware changes and its
   validation status.
3. Run `make test` and `make release`, then inspect the generated UF2 and
   `dist/SHA256SUMS`.
4. Commit the release preparation, create a `v<VERSION>` tag using the
   [release tag message guide](tag-messages.md), and push the tag.
5. Confirm the tagged GitHub Actions run succeeds and the resulting release
   contains both the UF2 and `SHA256SUMS`.

The tagged-release workflow requires the tag to equal `v` followed by the
contents of `VERSION`. It fetches the pinned dependencies, runs the host tests,
builds the release on Linux x86_64, and uploads both release assets. Never
replace an existing release artifact without incrementing `VERSION`.

Use numbered release candidates for hardware acceptance. For example, prepare
`X.Y.Z-rc.N` and tag it as `vX.Y.Z-rc.N`; the tagged workflow publishes it as
a GitHub prerelease. If acceptance finds a problem, increment the suffix for
the next candidate. After acceptance, remove the suffix, prepare `X.Y.Z`, and
tag `vX.Y.Z`; stable versions are published as normal GitHub releases.

Do not distribute or flash any artifact whose name contains
`stale-left-orientation`. That obsolete build used an incorrect, unmirrored
left-half coordinate transform and is intentionally absent from the
repository and releases.

It is built from the Vial-QMK revision in
[`dependencies.lock`](../dependencies.lock) with the overlay in
`firmware/keymaps/vial_right_custom/`. From a fresh clone, rebuild and inspect
it with:

```sh
make doctor
make setup
make test
make firmware
```

This pinned Vial-QMK revision normally generates a random 24-bit build ID for
each compile and uses it as the VIA EEPROM validity signature. The build shim
uses the fixed ID `0xF5E3B4` for custom builds, so clean builds are reproducible
and preserve stored-layout compatibility across releases.

The build corrects the physical LED count to 59 (`30` left and `29` right).
Its positions come from the official PCB data recorded in
[led-mapping.md](led-mapping.md). Key origins include every populated matrix
position, including thumb keys without a key LED. The coordinate model assumes
a nominal 19.05 mm gap between the inner key centers; edit the documented
translation in `ripple_layout.c` if a different fixed placement is preferred.

`RIPPLE_TOG`, `SPLASH_TOG`, `INVERSE_TOG`, `HEATMAP_TOG`, and the Auto Mouse
fade toggle appear under Vial's User keycodes. Assign them to any keys on any
layer. All custom modes start disabled on every boot, are never written to
EEPROM, and return to the current stock RGBLight mode when the active effect is
toggled off. Pressing another custom effect's toggle switches directly to it
and clears all custom effect state. The OLED shows `RIP`, `SPL`, `INV`, or
`HMP` for the active custom mode.

Ripple remains the existing hollow travelling ring. Splash produces a filled,
rapidly expanding bloom with a full-brightness core about one key pitch wide,
a soft linear falloff to a 65 mm radius, and a linear fade. Its four speed
levels last 1.4, 1.1, 0.85, and 0.6 seconds. Overlapping animations use their
brightest contribution instead of adding brightness. Both effects use the live
Vial hue, saturation, brightness (capped at 150), and normalized four-level
speed. Selecting blue in Vial produces the blue splash appearance. Vial's
`Effect +` and `Effect -` keycodes adjust that speed,
preserve the selected stock effect, and synchronize the ripple speed across
both halves. Stock breathing exposes four native speeds; rainbow mood, swirl,
snake, knight, and twinkle expose three, so the top two normalized levels are
equivalent for those effects. Static and fixed-interval effects do not change
speed. Eight keypress events are retained; the oldest is replaced by a ninth.
Both halves render locally, and a dedicated 12-byte split RPC carries
key-origin coordinates, the resolved global LED index, and validated
stock/ripple/splash/inverse/heatmap mode plus speed updates. Failed state updates
are retried every 250 ms
until the other half acknowledges them, and an authoritative state heartbeat
is sent every two seconds so either half converges after a reset or reconnect.

Inverse mode renders all 59 LEDs from the live RGBLight hue, saturation, and
brightness, retaining the firmware brightness cap of 150. A keypress uses hue
`(base + 128) mod 256` with the same saturation and brightness. It holds that
opposite color for exactly 100 ms, then RGB-crossfades to the live base over
1.4, 1.1, 0.85, or 0.6 seconds. `Effect +` selects a faster return and
`Effect -` a slower return; `Hue +` and `Hue -` continue to change the live
base normally. Every global LED has independent state, and pressing the same
key again immediately restarts its hold timer.

Heatmap mode starts with every LED dark. Each keypress first applies all whole
decay intervals elapsed for that key, adds one of ten brightness levels, caps
at 100%, and restarts only that key's countdown. Each interval removes exactly
one level, so a fully lit key reaches zero after ten intervals. The four speed
levels use 1,200, 900, 600, and 300 ms intervals from slowest to fastest;
`Effect +` selects a faster interval and `Effect -` a slower one. A speed
change resolves elapsed decay using the old speed, preserves the resulting
levels, and restarts every lit countdown from one synchronized timestamp on
both halves. The mode uses the live Vial hue and saturation, and scales each
level against the live brightness after the firmware cap of 150. Unmapped
underglow LEDs remain off.

The 39 switches with dedicated key LEDs map directly to those LEDs. The five
remaining thumb switches use the nearest underglow positions: left matrix
`3,3`, `3,4`, and `3,5` use left local LEDs 27, 28, and 29; right matrix `7,4`
and `7,5` use right local LEDs 1 and 0. Global indices are left local `0`–`29`
and right local `0`–`28` offset by 30. The complete mapping remains recorded in
[led-map.csv](led-map.csv).

Trackball scroll conversion retains sub-step movement between pointing-device
reports. Higher scroll dividers therefore reduce sensitivity without dropping
slow movements. Partial movement is cleared when scroll mode or its divider
changes, preventing an old remainder from causing a later scroll step. The
keymap default divider is `6`, which uses a 1/32 movement denominator. An
explicit divider saved in EEPROM is preserved; this default is used only when
the stored configuration contains the `0` sentinel.

Auto Mouse is enabled by default for fresh or reset configuration. Pointing
movement activates layer 1, and the layer is released after the default 500 ms
timeout. Auto Mouse enablement, timeout, and the scroll divider are saved to
EEPROM by `KBC_SAVE`; ordinary boots preserve those explicit settings,
including Auto Mouse being saved off. `KBC_RST` restores Auto Mouse enabled,
the 500 ms timeout, and the Div6 scroll default without saving them.

Auto Mouse lighting fades default to disabled for fresh, reset, upgraded, or
otherwise unrecognized user configuration. Assign `Auto Mouse Fade Toggle`
from Vial's User tab to a key to enable or disable them. `KBC_SAVE` persists
the current setting, while `KBC_RST` restores the disabled default without
saving it. Vial provides the assignable keycode but does not display its
current state.

When the fade setting and Auto Mouse are enabled and the configured mouse layer
is active, all 59 keyboard LEDs fade linearly to black over 250 ms. They remain
suppressed until that layer is released, including through the existing 500 ms
inactivity timeout, then restore over 250 ms. The layer's bit is followed even
when a higher layer is also active. Disabling either the fade setting or Auto
Mouse releases the suppression. Rapid reactivation or toggling reverses from
the current brightness without a jump.
Stock RGBLight and every custom effect continue running while suppressed, so
the restore uses their current output and live hue, saturation, brightness,
speed, effect, and enable state. Lighting that the user switches off remains
off. A separate six-byte split update synchronizes each transition; failed
updates retry every 16 ms and a two-second heartbeat recovers a reset or
reconnected half without changing RGBLight EEPROM settings.

Do not flash until the Vial export and both verified factory backups required by
[recovery.md](recovery.md) exist. Validate the diagnostic sequence before the
final image: left local indices must follow PCB `LED1`–`LED30`, and right local
indices must follow `LED1`–`LED29`. The hardware checklist covers the keys,
trackball modes, OLEDs, Vial, stock-effect restoration, rapid keypresses, and
split-link stability listed below.

Apply the following checklist to every release candidate. Record completed and
pending hardware validation in that version's changelog entry:

- All 44 switches, including the five thumb-to-underglow fallbacks.
- With fresh, upgraded, or reset configuration, confirm that pointing movement
  activates mouse layer 1 without fading, releases it after 500 ms, and that
  scrolling uses divider setting `6` (a 1/32 movement denominator).
- Assign `Auto Mouse Fade Toggle` from Vial's User tab, enable it, and confirm
  stock and custom lighting fade to black in 250 ms on mouse-layer activation,
  restore in 250 ms on release, and stay synchronized across both halves.
- Save the enabled setting with `KBC_SAVE`, reboot, and confirm that it remains
  enabled. Toggle without saving and confirm that reboot restores the last
  saved setting. Press `KBC_RST` and confirm fading becomes disabled without
  overwriting the saved setting.
- Reactivate Auto Mouse during either fade and confirm the direction reverses
  smoothly from the current brightness. Confirm that a higher active layer
  does not restore lighting while the configured mouse-layer bit remains set.
- While lighting is dark, change the RGB enable state, brightness, and effect;
  confirm the latest live state is restored, and that lighting switched off
  manually remains off. Disable Auto Mouse while suppressed and confirm the
  lights restore.
- Change the Auto Mouse enablement, timeout, and scroll divider, press
  `KBC_RST`, and confirm that Auto Mouse is enabled again with a 500 ms timeout
  and Div6 scrolling.
- Save Auto Mouse off with `KBC_SAVE`, reboot, and confirm that it remains off.
  Also confirm that explicitly saved timeout and scroll-divider values remain
  unchanged after reboot.
- In heatmap mode, all 44 switches light only their mapped LED, including the
  five thumb-to-underglow fallbacks; unmapped LEDs remain dark.
- Ten rapid presses reach 100% without an intervening decay, an eleventh press
  remains capped, and independently pressed keys retain independent levels and
  countdowns.
- A fully lit key reaches zero after approximately 12, 9, 6, and 3 seconds at
  the four speed levels, with one exact 10% decrement per interval.
- `Effect +` selects shorter intervals and `Effect -` longer ones without an
  immediate decrement; all currently lit countdowns restart together.
- Live hue, saturation, and brightness changes recolor/rescale lit heatmap
  keys immediately while preserving their levels.
- Rapid overlapping presses and immediate same-key retriggering in every
  custom mode.
- Live `Hue +`/`Hue -` changes and all four `Effect +`/`Effect -` levels.
- Keypress events initiated on either half and synchronized across the split.
- Direct switching among ripple, splash, inverse, and heatmap clears transient
  state for both the mode being left and the mode being entered.
- RGB enable/disable behavior, stock restoration, boot-disabled behavior, and
  the `HMP` OLED indicator.
- State recovery after either half resets or the split link reconnects.

No build, verification, or setup command flashes the keyboard automatically.
Flashing requires an explicit `./scripts/picotool load` command and firmware
filename as described in the [firmware installation guide](install-firmware.md).
