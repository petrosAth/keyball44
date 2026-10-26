# Tasks

## 1. Idle state module

- [x] 1.1 Add `firmware/keymaps/vial_right_custom/lighting_idle.h` and `lighting_idle.c` with `LIGHTING_IDLE_TIMEOUT_MS 180000`, `lighting_idle_t`, `lighting_idle_init`, and `lighting_idle_update`, implementing the latched behavior in design.md (new activity timestamp clears dark; unsigned `now - activity_at >= timeout` sets dark; dark persists until the timestamp changes). Use no floating point or dynamic allocation. Verify it compiles under the strict host flags through task 1.2.
- [x] 1.2 Add `tests/lighting_idle_test.c` covering: not dark at 179,999 ms and dark at exactly 180,000 ms; new activity clears dark and restarts the countdown; activity just before the timeout restarts it; staying dark when `now` advances past 2^32 ms beyond the last activity; timer wraparound where `activity_at` is near `UINT32_MAX` and `now` has wrapped; init seeding so the first timeout counts from the seed. Register it in `scripts/test.sh` and verify `make test` passes.

## 2. Firmware integration

- [x] 2.1 Add `lighting_idle.c` to `SRC` in `firmware/keymaps/vial_right_custom/rules.mk` and verify the source list includes it.
- [x] 2.2 In `keymap.c`, keep a static `lighting_idle_t`, seed it in `keyboard_post_init_user()` with `last_input_activity_time()`, and in the master branch of `housekeeping_task_user()` combine the result of `lighting_idle_update(..., last_input_activity_time(), synced_now)` with the existing Auto Mouse condition using OR, keeping the idle term outside the `lighting_fade_enabled` gate. Leave the slave path, the fade packet, and `vial_right_ledmap` unchanged. Verify with `make firmware` and confirm `git diff` touches no diagnostic keymap or packet files.
- [x] 2.3 Update `docs/custom-firmware.md`: describe the always-on 3-minute idle timeout (keys and trackball count; 250 ms fade out and in; the waking input is still sent; independent of the Auto Mouse fade setting; no EEPROM or RGBLight setting changes), and add release checklist items for idle dark on both halves, key and trackball wake, trackball movement keeping lighting on, wake from a key on the non-master half, idle with Auto Mouse fades off, overlap with Auto Mouse dark, and RGB switched off staying off. Mention the feature in the `README.md` summary. Verify the docs match the spec scenarios and `git diff --check` is clean.

## 3. Integration checks

- [x] 3.1 Run `make test` and `make firmware` from a clean state and verify both succeed. Do not flash as part of this task.
- [ ] 3.2 After the user manually flashes both halves, run the new hardware checklist items from `docs/custom-firmware.md` and record the results (pass, or pending hardware validation) for the next changelog entry.

## Workflow follow-up

- Archive the change with `/opsx:archive` after hardware validation and review.
