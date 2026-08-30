"""Compatibility and reproducibility fixes for pinned Vial-QMK scripts."""

# SPDX-License-Identifier: GPL-2.0-or-later

import ast
import os
import random
import sys


if not hasattr(ast, "Num"):
    ast.Num = ast.Constant

if not hasattr(ast.Constant(value=0), "n"):
    ast.Constant.n = property(lambda node: node.value)

# This Vial revision chooses a random VIA EEPROM signature for every build.
# Override only that generator when a release build supplies the published ID.
if os.path.basename(sys.argv[0]) == "build_id.py" and os.environ.get(
    "KEYBALL44_BUILD_ID"
):
    build_id = int(os.environ["KEYBALL44_BUILD_ID"], 0)
    random.randrange = lambda *_args, **_kwargs: build_id
