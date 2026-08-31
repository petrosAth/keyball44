#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

# This file is sourced by project scripts after they set $workspace.
# shellcheck disable=SC1091
. "$workspace/dependencies.lock"

vial_qmk_home="$workspace/external/vial-qmk"
keyball_reference_home="$workspace/external/yowkees-keyball"
picotool_source_home="$workspace/external/picotool"
pico_sdk_home="$workspace/external/pico-sdk"
picotool_build_home="$workspace/build/picotool-$PICOTOOL_VERSION"
picotool_install_home="$workspace/build/tools/picotool-$PICOTOOL_VERSION"
picotool_binary="$picotool_install_home/picotool/picotool"

# GNU coreutils names it sha256sum; macOS ships shasum instead.
sha256() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$@"
    else
        shasum -a 256 "$@"
    fi
}
