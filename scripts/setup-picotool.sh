#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=common.sh
. "$workspace/scripts/common.sh"

missing=
for tool in c++ cmake git ninja pkg-config; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        missing="$missing $tool"
    fi
done
if [ -n "$missing" ]; then
    echo "Missing picotool build prerequisites:$missing" >&2
    echo "Run this script through 'make setup-picotool'; install a C++ compiler and pkg-config if they are still missing." >&2
    exit 1
fi
if ! pkg-config --exists libusb-1.0; then
    echo "Missing libusb-1.0 development files; picotool must include USB support for backup and flashing." >&2
    exit 1
fi

clone_at_commit() {
    repository=$1
    commit=$2
    destination=$3
    label=$4

    if [ ! -d "$destination/.git" ]; then
        if [ -e "$destination" ]; then
            echo "Refusing to replace non-Git path: $destination" >&2
            exit 1
        fi
        git clone --filter=blob:none --no-checkout "$repository" "$destination"
    fi
    actual_origin=$(git -C "$destination" remote get-url origin)
    if [ "$actual_origin" != "$repository" ]; then
        echo "Refusing $label checkout with unexpected origin: $actual_origin" >&2
        exit 1
    fi
    git -C "$destination" fetch --depth=1 origin "$commit"
    git -C "$destination" checkout --detach "$commit"
    actual_commit=$(git -C "$destination" rev-parse HEAD)
    if [ "$actual_commit" != "$commit" ]; then
        echo "Refusing $label checkout at $actual_commit; expected $commit" >&2
        exit 1
    fi
}

mkdir -p "$workspace/external" "$workspace/build/tools"
clone_at_commit "$PICOTOOL_REPOSITORY" "$PICOTOOL_COMMIT" \
    "$picotool_source_home" picotool
clone_at_commit "$PICO_SDK_REPOSITORY" "$PICO_SDK_COMMIT" \
    "$pico_sdk_home" Pico-SDK

cmake -S "$picotool_source_home" -B "$picotool_build_home" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$picotool_install_home" \
    -DPICO_SDK_PATH="$pico_sdk_home" \
    -DPICOTOOL_FLAT_INSTALL=1 \
    -DPICOTOOL_NO_LIBUSB=OFF
cmake --build "$picotool_build_home"
cmake --install "$picotool_build_home"

version_output=$("$picotool_binary" version)
printf '%s\n' "$version_output"
if ! printf '%s\n' "$version_output" | grep -Fq "picotool v$PICOTOOL_VERSION"; then
    echo "Installed picotool did not report expected version $PICOTOOL_VERSION." >&2
    exit 1
fi
if printf '%s\n' "$version_output" | grep -Fq "without USB support"; then
    echo "Installed picotool lacks USB support; check the libusb-1.0 development installation and rebuild." >&2
    exit 1
fi
help_output=$("$picotool_binary" help)
for command in load save; do
    if ! printf '%s\n' "$help_output" | grep -Eq "^    $command "; then
        echo "Installed picotool does not expose the USB $command command." >&2
        exit 1
    fi
done

echo "Pinned USB-enabled picotool is ready at $picotool_binary"
