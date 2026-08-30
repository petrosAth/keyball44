#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=lock.sh
. "$workspace/scripts/lock.sh"
overlay="$workspace/firmware/keymaps/vial_right_custom"
qmk_keymap=vial_right_custom
target="$vial_qmk_home/keyboards/keyball/keyball44/keymaps/$qmk_keymap"
release_version=$(tr -d '\r\n' < "$workspace/VERSION")
short_commit=$(printf '%.8s' "$VIAL_QMK_COMMIT")
artifact="$workspace/dist/keyball44-vial_right-custom-v${release_version}-${short_commit}.uf2"

if ! printf '%s\n' "$release_version" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
    echo "Refusing to build: VERSION must contain a semantic version (X.Y.Z)" >&2
    exit 1
fi

cleanup() {
    rm -f "$target/config.h" "$target/keymap.c" "$target/inverse_engine.c" \
        "$target/inverse_engine.h" "$target/lighting_speed.c" \
        "$target/lighting_speed.h" "$target/ripple_engine.c" \
        "$target/ripple_engine.h" "$target/ripple_layout.c" \
        "$target/ripple_layout.h" "$target/ripple_packet.h" "$target/ripple_sync.c" \
        "$target/ripple_sync.h" "$target/scroll_accumulator.c" \
        "$target/scroll_accumulator.h" "$target/rules.mk" "$target/vial.json"
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
for file in config.h keymap.c inverse_engine.c inverse_engine.h lighting_speed.c lighting_speed.h ripple_engine.c ripple_engine.h ripple_layout.c ripple_layout.h ripple_packet.h ripple_sync.c ripple_sync.h scroll_accumulator.c scroll_accumulator.h rules.mk; do
    cp "$overlay/$file" "$target/$file"
done
jq --slurpfile custom "$overlay/vial-custom-keycodes.json" \
    '. + {customKeycodes: $custom[0]}' \
    "$vial_qmk_home/keyboards/keyball/keyball44/keymaps/vial_right/vial.json" > "$target/vial.json"

KEYBALL44_BUILD_ID=0xF5E3B4 \
    "$workspace/scripts/qmk-pinned" compile -kb keyball/keyball44 -km "$qmk_keymap"
cp "$vial_qmk_home/keyball_keyball44_${qmk_keymap}.uf2" "$artifact"
sha256sum "$artifact"
"$picotool_binary" info -a "$artifact"
