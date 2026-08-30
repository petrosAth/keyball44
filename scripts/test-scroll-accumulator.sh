#!/bin/sh

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
binary=$(mktemp "${TMPDIR:-/tmp}/scroll-accumulator-test.XXXXXX")
trap 'rm -f "$binary"' EXIT HUP INT TERM

cc -std=c11 -Wall -Wextra -Werror -pedantic \
    "$workspace/tests/scroll_accumulator_test.c" \
    "$workspace/firmware/keymaps/vial_right_custom/scroll_accumulator.c" \
    -o "$binary"
"$binary"
