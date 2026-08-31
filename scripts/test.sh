#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

# Host unit tests. These compile the firmware's platform-independent engines
# against stubs, so they need nothing but a C compiler -- no mise, no toolchain,
# and no Vial-QMK checkout.

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
custom="$workspace/firmware/keymaps/vial_right_custom"
ledmap="$workspace/firmware/keymaps/vial_right_ledmap"

binary=
cleanup() { rm -f "$binary"; }
trap cleanup EXIT HUP INT TERM

run_test() {
    name=$1
    shift
    binary=$(mktemp "${TMPDIR:-/tmp}/keyball44-$name.XXXXXX")
    cc -std=c11 -Wall -Wextra -Werror -pedantic \
        -I"$workspace/tests/stubs" -I"$ledmap" "$@" -o "$binary"
    "$binary"
    rm -f "$binary"
    binary=
    echo "ok: $name"
}

run_test inverse-engine \
    "$workspace/tests/inverse_engine_test.c" "$custom/inverse_engine.c"
run_test ledmap-diag \
    "$workspace/tests/ledmap_diag_state_test.c" "$ledmap/ledmap_diag_state.c"
run_test lighting-speed \
    "$workspace/tests/lighting_speed_test.c" "$custom/lighting_speed.c"
run_test ripple-engine \
    "$workspace/tests/ripple_engine_test.c" "$custom/ripple_engine.c" \
    "$custom/ripple_layout.c"
run_test ripple-packet \
    "$workspace/tests/ripple_packet_test.c"
run_test ripple-sync \
    "$workspace/tests/ripple_sync_test.c" "$custom/ripple_sync.c"
run_test scroll-accumulator \
    "$workspace/tests/scroll_accumulator_test.c" "$custom/scroll_accumulator.c"
