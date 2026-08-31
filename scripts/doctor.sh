#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

# jq, Python, the QMK CLI, and the Arm toolchain are pinned and installed by
# mise; see mise.toml. Only system-level prerequisites are checked here,
# because mise cannot provide a host compiler.
missing=
for tool in cc git make mise; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        missing="$missing $tool"
    fi
done

if [ -n "$missing" ]; then
    echo "Missing required host tools:$missing" >&2
    echo "Install a C compiler, Git, Make, and mise." >&2
    exit 1
fi

echo "Host tools are available."
