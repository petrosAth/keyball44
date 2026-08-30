#!/bin/sh

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
binary=$(mktemp "${TMPDIR:-/tmp}/ripple-engine-test.XXXXXX")
trap 'rm -f "$binary"' EXIT HUP INT TERM

cc -std=c11 -Wall -Wextra -Werror -pedantic \
    "$workspace/tests/ripple_engine_test.c" \
    "$workspace/firmware/keymaps/vial_right_custom/ripple_engine.c" \
    "$workspace/firmware/keymaps/vial_right_custom/ripple_layout.c" \
    -o "$binary"
"$binary"
