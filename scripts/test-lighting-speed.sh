#!/bin/sh

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
binary=$(mktemp "${TMPDIR:-/tmp}/lighting-speed-test.XXXXXX")
trap 'rm -f "$binary"' EXIT HUP INT TERM

cc -std=c11 -Wall -Wextra -Werror -pedantic \
    -I"$workspace/tests/stubs" \
    "$workspace/tests/lighting_speed_test.c" \
    "$workspace/firmware/keymaps/vial_right_custom/lighting_speed.c" \
    -o "$binary"
"$binary"
