RGBLIGHT_ENABLE = yes
OLED_ENABLE = yes

VIA_ENABLE = yes
VIAL_ENABLE = yes

LTO_ENABLE = yes
QMK_SETTINGS = yes
TAP_DANCE_ENABLE = yes
COMBO_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes

SRC += inverse_engine.c lighting_speed.c ripple_engine.c ripple_layout.c ripple_sync.c \
       scroll_accumulator.c
