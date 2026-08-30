# SPDX-License-Identifier: GPL-2.0-or-later

.PHONY: doctor setup reference-data test firmware diagnostic release clean

doctor:
	./scripts/doctor.sh

setup:
	./scripts/bootstrap.sh

reference-data:
	./scripts/bootstrap.sh --reference-data

test:
	./scripts/test-all.sh

firmware:
	./scripts/build-custom-firmware.sh

diagnostic:
	./scripts/build-ledmap-diagnostic.sh

release:
	./scripts/release.sh

clean:
	rm -rf build dist
