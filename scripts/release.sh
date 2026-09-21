#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=common.sh
. "$workspace/scripts/common.sh"
# shellcheck source=release-version.sh
. "$workspace/scripts/release-version.sh"
version=$(read_release_version "$workspace/VERSION")

# Require the validated version to be documented before building it.
if ! grep -F "## [$version]" "$workspace/CHANGELOG.md" >/dev/null; then
    echo "Refusing release: CHANGELOG.md has no entry for $version" >&2
    exit 1
fi

"$workspace/scripts/build.sh" custom
(
    cd "$workspace/dist"
    sha256 "keyball44-vial_right-custom-v${version}-"*.uf2 > SHA256SUMS
)
cat "$workspace/dist/SHA256SUMS"
