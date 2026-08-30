# Research sources

Access dates and branch heads change. Firmware work must use the revisions in
[`dependencies.lock`](../dependencies.lock), not whichever commits branch
names point to later.

## Exact device source

- [KeebNERD/tangbonze GitHub profile](https://github.com/tangbonze)
- [KeebNERD Vial-QMK fork](https://github.com/tangbonze/vial-qmk)
- [Exact Keyball44 subtree](https://github.com/tangbonze/vial-qmk/tree/vial/keyboards/keyball/keyball44)
- [`keyboard.json`](https://github.com/tangbonze/vial-qmk/blob/vial/keyboards/keyball/keyball44/keyboard.json)
- [`vial_right/config.h`](https://github.com/tangbonze/vial-qmk/blob/vial/keyboards/keyball/keyball44/keymaps/vial_right/config.h)
- [`vial_right/rules.mk`](https://github.com/tangbonze/vial-qmk/blob/vial/keyboards/keyball/keyball44/keymaps/vial_right/rules.mk)

The source strings, VID/PID, and Vial UID match the connected keyboard.

## Lighting references

- [QMK RGBLight documentation](https://docs.qmk.fm/features/rgblight)
- [Vial lighting documentation](https://get.vial.today/docs/lighting.html)
- [QMK RGB Matrix animation library](https://github.com/qmk/qmk_firmware/tree/master/quantum/rgb_matrix/animations)
- [QMK Solid Splash implementation](https://github.com/qmk/qmk_firmware/blob/master/quantum/rgb_matrix/animations/solid_splash_anim.h)

The RGB Matrix sources are references for animation mathematics only. The
matching KeebNERD firmware says RGB Matrix does not work for this board; the
custom animation must remain on RGBLight.

## Hardware and recovery references

- [Official Yowkees Keyball repository](https://github.com/Yowkees/keyball)
- [Official Keyball44 design data](https://github.com/Yowkees/keyball/tree/main/keyball44/design_data)
- [QMK flashing documentation](https://docs.qmk.fm/flashing)
- [Official Raspberry Pi picotool](https://github.com/raspberrypi/picotool)
- [Raspberry Pi microcontroller documentation](https://www.raspberrypi.com/documentation/microcontrollers/)
- [Arch Linux QMK package](https://archlinux.org/packages/extra/any/qmk/)

The LED map uses the pinned official KiCad PCB and netlist archives. The design
data is authoritative for physical geometry and serial routing; it is not a
substitute for the exact KeebNERD firmware source used to build the
application.

## Product and comparison references

- [Purchased AliExpress listing](https://www.aliexpress.us/item/3256810408543873.html)
- [Holykeebs Keyball44 product page](https://holykeebs.com/products/keyball44)
- [Holykeebs firmware documentation](https://docs.holykeebs.com/firmware/)

Holykeebs firmware is a separate implementation and must not be flashed to this
KeebNERD board. Similar appearance and Vial support do not establish electrical
or pin compatibility.
