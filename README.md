# Keyball44 custom firmware

This public, source-only repository builds custom Vial firmware for the KeebNERD Keyball44. It adds ripple, splash, inverse, and per-key heatmap lighting, optional synchronized Auto Mouse lighting fades, and reliable trackball scrolling while preserving the board's existing Vial, OLED, split, and pointing-device behavior.

Generated firmware, tool builds, upstream checkouts, and private device backups are deliberately excluded from Git. No build or setup target flashes a keyboard; flashing requires an explicit command and firmware filename.

## Safety

1. Never connect or disconnect TRRS while either half is powered. Disconnect USB first.
2. Do not flash Holykeebs, generic Keyball44, or unrelated UF2 files. This board uses a confirmed KeebNERD-specific configuration.
3. Before flashing, export the Vial layout and make a verified full-flash backup of each half separately with TRRS disconnected.
4. Keep those backups in encrypted storage outside the clone. Set `KEYBALL44_PRIVATE_BACKUP_DIR` to that external directory when following the commands in [the recovery guide](docs/recovery.md).

## Prerequisites

The build requires [mise](https://mise.jdx.dev), Git, Make, and a C compiler. Everything else — jq, Python, the QMK CLI, and the Arm GNU Toolchain used to compile firmware — is pinned in [`mise.toml`](mise.toml), checksum-verified against [`mise.lock`](mise.lock), and installed by `mise install`; none of it needs to be preinstalled. Package names vary by operating system; `make doctor` reports any missing command-line tools.

`make test` runs the host unit tests against stubs and needs only a C compiler — no mise, no toolchain, and no upstream checkout.

The optional repository-managed `picotool` requires a C++ compiler, `pkg-config`, and the libusb-1.0 development files. Its pinned CMake and Ninja versions are installed only when its separate setup target runs.

## Fresh-clone workflow

```sh
make doctor
make setup
make test
make firmware
```

`make setup` installs the pinned tools from [`mise.toml`](mise.toml), fetches the exact source revisions in [`dependencies.lock`](dependencies.lock), and initializes the required Vial-QMK submodules under ignored directories. The firmware appears in `dist/`.

The two pin files divide the work: `mise.toml` pins downloadable tools by version and SHA256, while `dependencies.lock` pins source repositories by immutable Git revision. Released firmware is built on Linux x86_64; see [Releases](#releases).

Other tasks are:

- `make setup-picotool` — build the pinned, USB-enabled Raspberry Pi `picotool` used for backup, inspection, and flashing.
- `make diagnostic` — build the one-LED-at-a-time mapping diagnostic.
- `make reference-data` — optionally fetch the pinned Yowkees hardware design repository used to derive the LED map.
- `make release` — validate `VERSION`, build the firmware, and create `dist/SHA256SUMS`.
- `make clean` — remove generated builds and release staging.

## Picotool and flashing

Install the optional flashing tool separately from the firmware dependencies:

```sh
make setup-picotool
./scripts/picotool version
```

The wrapper always invokes the repository's pinned build. It deliberately passes arguments through without guessing a keyboard half or firmware file:

```sh
./scripts/picotool info -a
./scripts/picotool load -v dist/explicitly-named-image.uf2
```

On Linux, install the upstream rule at `external/picotool/udev/60-picotool.rules` according to the operating system's udev policy if USB access would otherwise require root. Before loading any image, follow the per-half backup and power-safety procedure in [the recovery guide](docs/recovery.md), then follow the [firmware installation guide](docs/install-firmware.md).

## Releases

Download firmware and `SHA256SUMS` together from the matching tagged [GitHub release](https://github.com/petrosAth/keyball44/releases), and verify the firmware against that checksum file. The attached `SHA256SUMS` is the authoritative digest for its release.

Binaries are release attachments, not repository files. Older local builds and the obsolete stale-orientation diagnostic are not distributable because their exact source states are unavailable or unsafe.

## Documentation

- [Device profile](docs/device-profile.md)
- [Custom firmware behavior and validation](docs/custom-firmware.md)
- [LED map provenance](docs/led-mapping.md) and [mapping data](docs/led-map.csv)
- [Backup and recovery](docs/recovery.md)
- [Firmware installation](docs/install-firmware.md)
- [Changelog](CHANGELOG.md)
- [Research sources](docs/sources.md)

The repository is licensed under GPL-2.0-or-later. See [LICENSE](LICENSE).
