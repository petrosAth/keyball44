#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=lock.sh
. "$workspace/scripts/lock.sh"

reference_data=false
case ${1-} in
    "") ;;
    --reference-data) reference_data=true ;;
    *) echo "usage: $0 [--reference-data]" >&2; exit 2 ;;
esac

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

clone_at_tag() {
    repository=$1
    tag=$2
    destination=$3
    label=$4

    if [ ! -d "$destination/.git" ]; then
        if [ -e "$destination" ]; then
            echo "Refusing to replace non-Git path: $destination" >&2
            exit 1
        fi
        git clone --filter=blob:none --branch "$tag" --depth=1 \
            "$repository" "$destination"
    fi
    actual_origin=$(git -C "$destination" remote get-url origin)
    if [ "$actual_origin" != "$repository" ]; then
        echo "Refusing $label checkout with unexpected origin: $actual_origin" >&2
        exit 1
    fi
    git -C "$destination" fetch --depth=1 origin \
        "refs/tags/$tag:refs/tags/$tag"
    git -C "$destination" checkout --detach "$tag"
    if [ "$(git -C "$destination" describe --tags --exact-match)" != "$tag" ]; then
        echo "Refusing $label checkout that is not exact tag $tag" >&2
        exit 1
    fi
}

mkdir -p "$workspace/external" "$workspace/build/tools"
clone_at_commit "$VIAL_QMK_REPOSITORY" "$VIAL_QMK_COMMIT" "$vial_qmk_home" Vial-QMK
git -C "$vial_qmk_home" submodule update --init --depth=1 -- \
    lib/chibios \
    lib/chibios-contrib \
    lib/lufa \
    lib/pico-sdk \
    lib/printf

if [ ! -x "$picotool_binary" ]; then
    clone_at_tag "$PICOTOOL_REPOSITORY" "$PICOTOOL_TAG" \
        "$picotool_source_home" picotool
    clone_at_tag "$PICO_SDK_REPOSITORY" "$PICO_SDK_TAG" \
        "$pico_sdk_home" Pico-SDK
    cmake -S "$picotool_source_home" -B "$picotool_build_home" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$picotool_install_home" \
        -DPICO_SDK_PATH="$pico_sdk_home" \
        -DPICOTOOL_FLAT_INSTALL=1
    cmake --build "$picotool_build_home"
    cmake --install "$picotool_build_home"
fi

if [ "$reference_data" = true ]; then
    clone_at_commit "$KEYBALL_REPOSITORY" "$KEYBALL_COMMIT" \
        "$keyball_reference_home" Yowkees-Keyball
fi

echo "Pinned build dependencies are ready."
