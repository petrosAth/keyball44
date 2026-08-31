#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=common.sh
. "$workspace/scripts/common.sh"

short_commit=$(printf '%.8s' "$VIAL_QMK_COMMIT")

case ${1-} in
    custom)
        keymap=vial_right_custom
        # This pinned Vial revision picks a random VIA EEPROM signature per
        # build; the shim in scripts/qmk-python-compat pins it to the published
        # ID so clean builds stay reproducible and layout-compatible.
        build_id=0xF5E3B4
        version=$(tr -d '\r\n' < "$workspace/VERSION")
        if ! printf '%s\n' "$version" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
            echo "Refusing to build: VERSION must contain a semantic version (X.Y.Z)" >&2
            exit 1
        fi
        artifact="$workspace/dist/keyball44-vial_right-custom-v${version}-${short_commit}.uf2"
        ;;
    diagnostic)
        keymap=vial_right_ledmap
        build_id=0xF9E7E5
        artifact="$workspace/dist/keyball44-vial_right-ledmap-diagnostic-${short_commit}.uf2"
        ;;
    *)
        echo "usage: $0 <custom|diagnostic>" >&2
        exit 2
        ;;
esac

overlay="$workspace/firmware/keymaps/$keymap"
target="$vial_qmk_home/keyboards/keyball/keyball44/keymaps/$keymap"

created_target=false
cleanup() {
    if [ "$created_target" = true ]; then
        rm -rf "$target"
    fi
}
trap cleanup EXIT HUP INT TERM

"$workspace/scripts/test-qmk.sh"

mkdir -p "$target" "$workspace/dist"
created_target=true
cp -R "$overlay"/. "$target"/

if [ "$1" = custom ]; then
    # Build input, not a keymap source.
    rm -f "$target/vial-custom-keycodes.json"
    jq --slurpfile custom "$overlay/vial-custom-keycodes.json" \
        '. + {customKeycodes: $custom[0]}' \
        "$vial_qmk_home/keyboards/keyball/keyball44/keymaps/vial_right/vial.json" \
        > "$target/vial.json"
else
    cp "$vial_qmk_home/keyboards/keyball/keyball44/keymaps/vial_right/vial.json" \
        "$target/vial.json"
fi

KEYBALL44_BUILD_ID="$build_id" \
    "$workspace/scripts/qmk-pinned" compile -kb keyball/keyball44 -km "$keymap"
cp "$vial_qmk_home/keyball_keyball44_${keymap}.uf2" "$artifact"
sha256 "$artifact"
