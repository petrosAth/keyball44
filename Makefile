# SPDX-License-Identifier: GPL-2.0-or-later

.PHONY: doctor setup reference-data test firmware diagnostic release clean

doctor:
	./scripts/doctor.sh

setup:
	mise install
	./scripts/bootstrap.sh

reference-data:
	./scripts/bootstrap.sh --reference-data

test:
	./scripts/test.sh

# 'mise exec' puts the pinned jq, Python, QMK CLI, and Arm toolchain on PATH.
firmware:
	mise exec -- ./scripts/build.sh custom

diagnostic:
	mise exec -- ./scripts/build.sh diagnostic

release:
	mise exec -- ./scripts/release.sh

clean:
	rm -rf build dist
