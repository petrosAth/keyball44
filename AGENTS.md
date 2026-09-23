# Repository guidance

## Project shape

- This repository contains source overlays and tooling for reproducible custom Vial firmware for the KeebNERD Keyball44.
- `firmware/` contains the production and LED-diagnostic keymap overlays.
- `tests/` contains host-side C tests for platform-independent firmware logic.
- `scripts/`, `Makefile`, `mise.toml`, and `dependencies.lock` define setup, builds, tests, and releases.
- `docs/` and `README.md` are operational documentation. Treat recovery and hardware-safety instructions as part of the product contract.

Read the nearest nested `AGENTS.md` before changing a subsystem. Its guidance adds to this file.

## Source and generated boundaries

- Edit tracked source in this repository, not fetched copies under `external/`. Firmware overlays are copied into the pinned Vial-QMK checkout during builds.
- Do not commit or hand-edit generated content under `build/` or `dist/`, or firmware/toolchain outputs such as UF2, ELF, BIN, HEX, MAP, object, or archive files.
- Keep private device backups outside the repository. Never inspect, modify, or commit material under ignored `backups/` or `private/` paths.
- Preserve reproducibility: downloadable tool versions and checksums belong in `mise.toml`/`mise.lock`; source repository URLs and immutable revisions belong in `dependencies.lock`.

## Safety

- Never automate flashing as part of setup, build, test, or release commands. Flashing must remain an explicit action with an explicitly named image.
- Do not claim that these images work on Holykeebs, generic Keyball44, or other hardware. This repository targets the documented KeebNERD configuration.
- Preserve the recovery rule that USB power is disconnected before connecting or disconnecting TRRS, and that each half is backed up separately.

## Common workflow

- `make test` runs the host unit suite and needs only a C compiler.
- `make doctor` checks host prerequisites.
- `make setup` installs pinned tools and fetches pinned build dependencies; it performs network and ignored-tree writes, so run it only when required.
- `make firmware` and `make diagnostic` require setup and write release staging under `dist/`.
- `make release` validates `VERSION`, builds production firmware, and creates `dist/SHA256SUMS`. Use it only for release work.
- For ordinary source changes, run the narrowest relevant checks followed by `make test`. Run a firmware build when QMK integration, keymap configuration, or release behavior changes.

Keep changes focused, preserve unrelated work in a dirty worktree, and use `git diff --check` before handing off edits.
