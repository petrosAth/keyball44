#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=lock.sh
. "$workspace/scripts/lock.sh"
overlay="$workspace/firmware/keymaps/vial_right_ledmap"
target="$vial_qmk_home/keyboards/keyball/keyball44/keymaps/vial_right_ledmap"
short_commit=$(printf '%.8s' "$VIAL_QMK_COMMIT")
artifact="$workspace/dist/keyball44-vial_right-ledmap-diagnostic-${short_commit}.uf2"

cleanup() {
    rm -f \
        "$target/config.h" \
        "$target/keymap.c" \
        "$target/ledmap_diag_state.c" \
        "$target/ledmap_diag_state.h" \
        "$target/rules.mk" \
        "$target/vial.json"
    rmdir "$target" 2>/dev/null || true
}
trap cleanup EXIT HUP INT TERM

if [ ! -x "$picotool_binary" ]; then
    echo "Pinned picotool is not set up; run 'make setup' first." >&2
    exit 1
fi
actual_commit=$(git -C "$vial_qmk_home" rev-parse HEAD)
if [ "$actual_commit" != "$VIAL_QMK_COMMIT" ]; then
    echo "Refusing to build: expected $VIAL_QMK_COMMIT, found $actual_commit" >&2
    exit 1
fi

mkdir -p "$target" "$workspace/dist"
cp "$overlay/config.h" "$target/config.h"
cp "$overlay/keymap.c" "$target/keymap.c"
cp "$overlay/ledmap_diag_state.c" "$target/ledmap_diag_state.c"
cp "$overlay/ledmap_diag_state.h" "$target/ledmap_diag_state.h"
cp "$overlay/rules.mk" "$target/rules.mk"
cp "$vial_qmk_home/keyboards/keyball/keyball44/keymaps/vial_right/vial.json" "$target/vial.json"

KEYBALL44_BUILD_ID=0xF9E7E5 "$workspace/scripts/qmk-pinned" compile \
    -kb keyball/keyball44 \
    -km vial_right_ledmap

cp "$vial_qmk_home/keyball_keyball44_vial_right_ledmap.uf2" "$artifact"
sha256sum "$artifact"
"$picotool_binary" info -a "$artifact"
