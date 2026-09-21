#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

# Shared validation for release versions. Stable releases use X.Y.Z; release
# candidates use X.Y.Z-rc.N, with no leading zeroes and N starting at 1.
release_version_is_valid() {
    case $1 in
        *"
"*) return 1 ;;
    esac
    printf '%s\n' "$1" | grep -Eq \
        '^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)(-rc\.[1-9][0-9]*)?$'
}

read_release_version() {
    version_file=$1
    release_version=$(cat "$version_file")
    if ! release_version_is_valid "$release_version"; then
        echo "Invalid VERSION: expected X.Y.Z or X.Y.Z-rc.N (N starts at 1)" >&2
        return 1
    fi
    printf '%s\n' "$release_version"
}
