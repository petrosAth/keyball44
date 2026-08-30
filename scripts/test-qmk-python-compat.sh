#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# shellcheck source=lock.sh
. "$workspace/scripts/lock.sh"

PYTHONPATH="$workspace/scripts/qmk-python-compat:$vial_qmk_home/lib/python${PYTHONPATH:+:$PYTHONPATH}" \
    python - <<'PY'
import ast
from qmk.math import compute

assert ast.Num is ast.Constant
assert isinstance(ast.parse("60", mode="eval").body, ast.Num)
assert compute("60") == 60
PY

"$workspace/scripts/qmk-pinned" --version >/dev/null
"$workspace/scripts/qmk-pinned" info -kb keyball/keyball44 >/dev/null

build_id=$(
    KEYBALL44_BUILD_ID=0xF5E3B4 \
        PYTHONPATH="$workspace/scripts/qmk-python-compat" \
        python "$vial_qmk_home/util/build_id.py"
)
test "$build_id" = '#define BUILD_ID ((uint32_t)0x00F5E3B4)'
