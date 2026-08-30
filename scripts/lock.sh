#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later

# This file is sourced by project scripts after they set $workspace.
# shellcheck disable=SC1091
. "$workspace/dependencies.lock"

vial_qmk_home="$workspace/external/vial-qmk"
keyball_reference_home="$workspace/external/yowkees-keyball"
picotool_source_home="$workspace/external/picotool"
pico_sdk_home="$workspace/external/pico-sdk"
picotool_build_home="$workspace/build/picotool-$PICOTOOL_TAG"
picotool_install_home="$workspace/build/tools/picotool-$PICOTOOL_TAG"
picotool_binary="$picotool_install_home/picotool/picotool"
arm_toolchain_home="$workspace/build/tools/arm-gnu-toolchain-$ARM_TOOLCHAIN_VERSION"
arm_toolchain_bin="$arm_toolchain_home/bin"
arm_toolchain_gcc="$arm_toolchain_bin/arm-none-eabi-gcc"
