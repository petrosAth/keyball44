# Script guidance

## Shell conventions

- Scripts target POSIX `/bin/sh`, not Bash. Use portable syntax and retain `set -eu` for operational scripts.
- Resolve the repository root from the script location into `workspace`, then source `scripts/common.sh` when shared pins, paths, or `sha256` are needed.
- Quote path and argument expansions. Prefer explicit case-based argument validation, actionable errors on stderr, and nonzero exits for refusals.
- Use traps to clean partially created or temporary state. Never replace an unexpected existing path, checkout from an unverified origin, or silently accept a revision other than the requested immutable commit.

## Reproducibility and safety

- Keep downloadable host/toolchain pins and checksums in `mise.toml` and `mise.lock`. Keep source repository URLs and commits in `dependencies.lock`.
- Route QMK compilation through `scripts/qmk-pinned`, retain the Python compatibility guard, and preserve the published deterministic Vial build ID.
- Keep generated checkouts, tool builds, firmware images, and checksum staging in the existing ignored `external/`, `build/`, and `dist/` locations.
- Setup, test, build, and release scripts must never flash a keyboard or infer a keyboard half or firmware filename. The picotool wrapper only passes through explicit user arguments.
- Preserve the macOS/Linux checksum fallback and USB-support checks for the repository-managed picotool.

## Validation

- Run `make test` after script changes that affect host testing.
- Run `make doctor` when prerequisite detection changes.
- Exercise the narrow script entry point and its invalid-argument path where feasible. Run firmware, diagnostic, picotool, or release builds only when the changed path requires their pinned dependencies.
- Run `git diff --check`; use `shellcheck` when available, but do not require an unpinned optional tool for otherwise valid changes.
