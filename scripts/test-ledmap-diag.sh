#!/bin/sh

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
test_binary="${TMPDIR:-/tmp}/keyball44-ledmap-diag-state-test"

cc -std=c11 -Wall -Wextra -Werror \
    -I"$workspace/firmware/keymaps/vial_right_ledmap" \
    "$workspace/tests/ledmap_diag_state_test.c" \
    "$workspace/firmware/keymaps/vial_right_ledmap/ledmap_diag_state.c" \
    -o "$test_binary"

"$test_binary"
