#!/bin/sh

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
binary=$(mktemp "${TMPDIR:-/tmp}/inverse-engine-test.XXXXXX")
trap 'rm -f "$binary"' EXIT HUP INT TERM

cc -std=c11 -Wall -Wextra -Werror -pedantic \
    "$workspace/tests/inverse_engine_test.c" \
    "$workspace/firmware/keymaps/vial_right_custom/inverse_engine.c" \
    -o "$binary"
"$binary"
