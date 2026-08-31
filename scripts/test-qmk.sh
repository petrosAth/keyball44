#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

# Guards the Python compatibility shim that the pinned QMK CLI depends on.
# Run as a precondition by scripts/build.sh rather than with the host unit
# tests, because it needs the pinned Vial-QMK checkout.

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=common.sh
. "$workspace/scripts/common.sh"

PYTHONPATH="$workspace/scripts/qmk-python-compat:$vial_qmk_home/lib/python${PYTHONPATH:+:$PYTHONPATH}" \
    python - <<'PY'
import ast
from qmk.math import compute

assert ast.Num is ast.Constant
assert isinstance(ast.parse("60", mode="eval").body, ast.Num)
assert compute("60") == 60
PY

# The published build ID must override this Vial revision's random VIA EEPROM
# signature, or release builds stop being reproducible.
build_id=$(
    KEYBALL44_BUILD_ID=0xF5E3B4 \
        PYTHONPATH="$workspace/scripts/qmk-python-compat" \
        python "$vial_qmk_home/util/build_id.py"
)
test "$build_id" = '#define BUILD_ID ((uint32_t)0x00F5E3B4)'
