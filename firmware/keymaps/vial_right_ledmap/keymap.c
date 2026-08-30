/*
 * Copyright 2022 @Yowkees
 * Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#define layer_state_set_user vial_right_layer_state_set_user
#define oledkit_render_info_user vial_right_oledkit_render_info_user
#include "../vial_right/keymap.c"
#undef layer_state_set_user
#undef oledkit_render_info_user

#include "ledmap_diag_state.h"
#include "lib/keyball/keyball.h"
#include "lib/oledkit/oledkit.h"
#include "rgblight.h"
#include "rgblight_drivers.h"

static ledmap_diag_state_t ledmap_diag;
static bool                ledmap_output_dirty;
static uint64_t            observed_rgblight_config;

static uint64_t ledmap_rgblight_snapshot(void) {
    return ((uint64_t)rgblight_is_enabled()) | ((uint64_t)rgblight_get_mode() << 8) | ((uint64_t)rgblight_get_hue() << 16) | ((uint64_t)rgblight_get_sat() << 24) |
           ((uint64_t)rgblight_get_val() << 32) | ((uint64_t)rgblight_get_speed() << 40);
}

static void ledmap_render_local_led(void) {
    hsv_t hsv = rgblight_get_hsv();
    if (hsv.v > RGBLIGHT_LIMIT_VAL) {
        hsv.v = RGBLIGHT_LIMIT_VAL;
    }
    rgb_t rgb = hsv_to_rgb(hsv);

    for (uint8_t i = 0; i < ledmap_diag.local_led_count; ++i) {
        rgblight_driver.set_color(i, 0, 0, 0);
    }
    if (ledmap_diag.local_led_count > 0) {
        rgblight_driver.set_color(ledmap_diag.local_led_index, rgb.r, rgb.g, rgb.b);
    }
    rgblight_driver.flush();

    observed_rgblight_config = ledmap_rgblight_snapshot();
    ledmap_output_dirty      = false;
}

static void ledmap_restore_stock_effect(void) {
    if (rgblight_is_enabled()) {
        rgblight_mode_noeeprom(rgblight_get_mode());
    } else {
        for (uint8_t i = 0; i < ledmap_diag.local_led_count; ++i) {
            rgblight_driver.set_color(i, 0, 0, 0);
        }
        rgblight_driver.flush();
    }
}

void keyboard_post_init_user(void) {
    ledmap_diag_init(&ledmap_diag, rgblight_ranges.clipping_num_leds);
    rgblight_timer_disable();
    observed_rgblight_config = ledmap_rgblight_snapshot();
    ledmap_output_dirty      = true;
}

void housekeeping_task_user(void) {
    if (!ledmap_diag.enabled) {
        return;
    }

    if (observed_rgblight_config != ledmap_rgblight_snapshot()) {
        rgblight_timer_disable();
        ledmap_output_dirty = true;
    }
    if (ledmap_output_dirty) {
        ledmap_render_local_led();
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    (void)keycode;
    if (!record->event.pressed) {
        return true;
    }

    uint8_t local_row = record->event.key.row % (MATRIX_ROWS / 2);
    if (ledmap_diag_is_next_key(local_row, record->event.key.col)) {
        if (ledmap_diag.enabled) {
            ledmap_diag_advance(&ledmap_diag);
            ledmap_output_dirty = true;
        }
    } else if (ledmap_diag_is_toggle_key(local_row, record->event.key.col)) {
        ledmap_diag_toggle(&ledmap_diag);
        if (ledmap_diag.enabled) {
            rgblight_timer_disable();
            ledmap_output_dirty = true;
        } else {
            ledmap_restore_stock_effect();
        }
    }

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return vial_right_layer_state_set_user(state);
}

#ifdef OLED_ENABLE
void oledkit_render_info_user(void) {
    if (!ledmap_diag.enabled) {
        vial_right_oledkit_render_info_user();
        return;
    }

    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    oled_write_P(PSTR("LED \xB1"), false);
    oled_write_char(is_keyboard_left() ? 'L' : 'R', false);
    oled_write_char(' ', false);
    oled_write_char('0' + (ledmap_diag.local_led_index / 10), false);
    oled_write_char('0' + (ledmap_diag.local_led_index % 10), false);
    oled_write_P(PSTR(" 00> 01 mode"), false);
}
#endif
