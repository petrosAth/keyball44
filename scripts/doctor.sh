#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

missing=
for tool in c++ cc cmake curl git jq make ninja pkg-config python qmk sha256sum tar; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        missing="$missing $tool"
    fi
done

if [ -n "$missing" ]; then
    echo "Missing required host tools:$missing" >&2
    echo "Install a C/C++ compiler, curl, Git, CMake, Ninja, jq, Python, QMK CLI, Make, pkg-config, tar, and coreutils." >&2
    exit 1
fi

if ! pkg-config --exists libusb-1.0; then
    echo "Missing required libusb-1.0 development files (pkg-config could not find them)." >&2
    exit 1
fi

echo "Host tools are available."
