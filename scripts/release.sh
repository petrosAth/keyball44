#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
version=$(tr -d '\r\n' < "$workspace/VERSION")

if ! printf '%s\n' "$version" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
    echo "Refusing release: VERSION must contain a semantic version (X.Y.Z)" >&2
    exit 1
fi
if ! grep -F "## [$version]" "$workspace/CHANGELOG.md" >/dev/null; then
    echo "Refusing release: CHANGELOG.md has no entry for $version" >&2
    exit 1
fi

if git -C "$workspace" rev-parse --git-dir >/dev/null 2>&1; then
    for tag in $(git -C "$workspace" tag --points-at HEAD); do
        if [ "$tag" != "v$version" ]; then
            echo "Refusing release: tag $tag does not match VERSION v$version" >&2
            exit 1
        fi
    done
fi

"$workspace/scripts/build-custom-firmware.sh"
(
    cd "$workspace/dist"
    sha256sum keyball44-vial_right-custom-v*.uf2 > SHA256SUMS
)
if [ "$version" = 1.1.1 ]; then
    expected='87c6b571836612a5c1233737ec4261e9461699c09926b291ea13208f77357a39  keyball44-vial_right-custom-v1.1.1-41babbb8.uf2'
    if ! grep -Fx "$expected" "$workspace/dist/SHA256SUMS" >/dev/null; then
        echo "Refusing release: v1.1.1 checksum does not match the documented release" >&2
        exit 1
    fi
fi
cat "$workspace/dist/SHA256SUMS"
