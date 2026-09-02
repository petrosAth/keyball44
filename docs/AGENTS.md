# Documentation guidance

## Document roles

- `README.md` is the entry point for safety, prerequisites, common workflows,
  releases, and links to detailed guides.
- `device-profile.md` records exact hardware identity and compatibility limits.
- `custom-firmware.md` describes current firmware behavior and validation.
- `recovery.md` owns backup, flashing, and restoration procedures.
- `led-mapping.md` explains provenance and validation for `led-map.csv`.
- `sources.md` records external evidence; prefer primary upstream sources.

## Consistency rules

- Keep hardware safety language unambiguous and consistent with the root
  README: disconnect USB before changing TRRS, handle halves separately, and
  require verified backups before flashing.
- Do not broaden compatibility beyond the documented KeebNERD device or carry
  assumptions from generic Keyball44 variants into commands or claims.
- Keep general build and release instructions release-neutral. Put historical
  release facts in `CHANGELOG.md`, not long-lived procedural documentation.
- When firmware behavior, defaults, custom keycodes, LED mappings, build
  commands, or prerequisites change, update all documents that describe that
  contract in the same change.
- Update `docs/led-map.csv`, its provenance text, firmware layout tables, and
  mapping tests together when the physical mapping changes.
- A release version change requires a matching `CHANGELOG.md` entry; release
  tags must match `v$(cat VERSION)`.

Verify commands and internal links against the current repository. Run
`git diff --check` and `make test` when documentation accompanies source
changes; documentation-only edits do not require downloading build tools.
