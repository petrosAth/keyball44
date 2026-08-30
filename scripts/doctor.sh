#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

set -eu

missing=
for tool in arm-none-eabi-gcc c++ cc cmake git jq make ninja pkg-config python qmk sha256sum; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        missing="$missing $tool"
    fi
done

if [ -n "$missing" ]; then
    echo "Missing required host tools:$missing" >&2
    echo "Install C/C++ and ARM toolchains, Git, CMake, Ninja, jq, Python, QMK CLI, Make, pkg-config, and coreutils." >&2
    exit 1
fi

if ! pkg-config --exists libusb-1.0; then
    echo "Missing required libusb-1.0 development files (pkg-config could not find them)." >&2
    exit 1
fi

echo "Host tools are available."
