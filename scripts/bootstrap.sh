#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=common.sh
. "$workspace/scripts/common.sh"

reference_data=false
case ${1-} in
    "") ;;
    --reference-data) reference_data=true ;;
    *) echo "usage: $0 [--reference-data]" >&2; exit 2 ;;
esac

# The pinned commit is asserted again by scripts/qmk-pinned, which every
# compile goes through; this only has to produce the checkout.
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
}

mkdir -p "$workspace/external"
clone_at_commit "$VIAL_QMK_REPOSITORY" "$VIAL_QMK_COMMIT" "$vial_qmk_home" Vial-QMK
git -C "$vial_qmk_home" submodule update --init --depth=1 -- \
    lib/chibios \
    lib/chibios-contrib \
    lib/lufa \
    lib/pico-sdk \
    lib/printf

if [ "$reference_data" = true ]; then
    clone_at_commit "$KEYBALL_REPOSITORY" "$KEYBALL_COMMIT" \
        "$keyball_reference_home" Yowkees-Keyball
fi

echo "Pinned build dependencies are ready."
