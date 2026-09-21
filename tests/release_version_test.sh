#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=../scripts/release-version.sh
. "$workspace/scripts/release-version.sh"

for version in \
    0.0.0 \
    1.3.0 \
    1.3.0-rc.1 \
    10.20.30-rc.42
do
    if ! release_version_is_valid "$version"; then
        echo "expected valid release version: $version" >&2
        exit 1
    fi
done

for version in \
    1.3.0-rc \
    1.3.0-rc.0 \
    1.3.0-rc.01 \
    1.3.0-beta.1 \
    1.3.0+build.1 \
    1.3 \
    v1.3.0 \
    01.3.0 \
    1.03.0 \
    1.3.00
do
    if release_version_is_valid "$version"; then
        echo "expected invalid release version: $version" >&2
        exit 1
    fi
done

if release_version_is_valid "1.3.0
unexpected"; then
    echo "expected invalid multi-line release version" >&2
    exit 1
fi

echo "ok: release-version"
