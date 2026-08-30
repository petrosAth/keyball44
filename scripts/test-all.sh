#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

workspace=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
for test_script in \
    test-inverse-engine.sh \
    test-ledmap-diag.sh \
    test-lighting-speed.sh \
    test-ripple-engine.sh \
    test-ripple-packet.sh \
    test-ripple-sync.sh \
    test-scroll-accumulator.sh \
    test-qmk-python-compat.sh; do
    "$workspace/scripts/$test_script"
done
