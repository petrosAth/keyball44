# Keyball44 custom firmware

This public, source-only repository builds custom Vial firmware for the
KeebNERD Keyball44. It adds ripple, splash, inverse lighting, and reliable
trackball scrolling while preserving the board's existing Vial, OLED, split,
and pointing-device behavior.

Generated firmware, tool builds, upstream checkouts, and private device
backups are deliberately excluded from Git. No command in this repository
flashes a keyboard.

## Safety

1. Never connect or disconnect TRRS while either half is powered. Disconnect
   USB first.
2. Do not flash Holykeebs, generic Keyball44, or unrelated UF2 files. This
   board uses a confirmed KeebNERD-specific configuration.
3. Before flashing, export the Vial layout and make a verified full-flash
   backup of each half separately with TRRS disconnected.
4. Keep those backups in encrypted storage outside the clone. Set
   `KEYBALL44_PRIVATE_BACKUP_DIR` to that external directory when following
   the commands in [the recovery guide](docs/recovery.md).

## Prerequisites

The build requires Git, Make, a C compiler, CMake, Ninja, jq, Python, QMK CLI,
the ARM GNU toolchain, and the development files for libusb. Package names
vary by operating system; `make doctor` reports any missing command-line
tools.

## Fresh-clone workflow

```sh
make doctor
make setup
make test
make firmware
```

`make setup` fetches the exact dependencies in
[`dependencies.lock`](dependencies.lock), initializes the required Vial-QMK
submodules, and builds the pinned picotool under ignored directories. The
firmware appears in `dist/`.

Other tasks are:

- `make diagnostic` — build the one-LED-at-a-time mapping diagnostic.
- `make reference-data` — optionally fetch the pinned Yowkees hardware design
  repository used to derive the LED map.
- `make release` — validate `VERSION`, build the firmware, and create
  `dist/SHA256SUMS`.
- `make clean` — remove generated builds and release staging.

## Releases

Download firmware and `SHA256SUMS` together from the matching tagged GitHub
release. For the initial `v1.1.1` release, the firmware checksum must be:

```text
efcd6a4fdae5541885db6388038c2a51f98f901ebd07c611041242a8119ff7fd
```

Binaries are release attachments, not repository files. Older local builds
and the obsolete stale-orientation diagnostic are not distributable because
their exact source states are unavailable or unsafe.

## Documentation

- [Device profile](docs/device-profile.md)
- [Custom firmware behavior and validation](docs/custom-firmware.md)
- [LED map provenance](docs/led-mapping.md) and [mapping data](docs/led-map.csv)
- [Backup and recovery](docs/recovery.md)
- [Changelog](CHANGELOG.md)
- [Research sources](docs/sources.md)

The repository is licensed under GPL-2.0-or-later. See [LICENSE](LICENSE).
