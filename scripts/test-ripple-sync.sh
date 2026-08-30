#!/bin/sh

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
binary=$(mktemp "${TMPDIR:-/tmp}/ripple-sync-test.XXXXXX")
trap 'rm -f "$binary"' EXIT HUP INT TERM

cc -std=c11 -Wall -Wextra -Werror -pedantic \
    "$workspace/tests/ripple_sync_test.c" \
    "$workspace/firmware/keymaps/vial_right_custom/ripple_sync.c" \
    -o "$binary"
"$binary"
