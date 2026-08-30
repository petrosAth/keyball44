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

#include "lib/keyball/keyball.h"
#include "lib/oledkit/oledkit.h"
#include "inverse_engine.h"
#include "lighting_speed.h"
#include "rgblight.h"
#include "rgblight_drivers.h"
#include "ripple_engine.h"
#include "ripple_layout.h"
#include "ripple_packet.h"
#include "ripple_sync.h"
#include "scroll_accumulator.h"
#include "sync_timer.h"
#include "transactions.h"

#define RIPPLE_TOG QK_KB_16
#define SPLASH_TOG QK_KB_17
#define INVERSE_TOG QK_KB_18
#define RIPPLE_FRAME_INTERVAL_MS 16

static ripple_engine_t ripple_engine;
static inverse_engine_t inverse_engine;
static ripple_effect_mode_t ripple_effect_mode;
static uint32_t ripple_last_frame;
static uint8_t ripple_speed;
static ripple_sync_state_t ripple_sync;
static scroll_accumulator_t scroll_accumulators[2][2];
static uint8_t scroll_accumulator_div;

static void scroll_accumulators_reset(void) {
  for (uint8_t side = 0; side < 2; ++side) {
    for (uint8_t axis = 0; axis < 2; ++axis) {
      scroll_accumulator_reset(&scroll_accumulators[side][axis]);
    }
  }
  scroll_accumulator_div = keyball_get_scroll_div();
}

static int8_t scroll_clip(int16_t value) {
  return value < -127 ? -127 : value > 127 ? 127 : (int8_t)value;
}

void keyball_on_apply_motion_to_mouse_scroll(report_mouse_t *report,
                                             report_mouse_t *output,
                                             bool is_left) {
  uint8_t scroll_div = keyball_get_scroll_div();
  if (scroll_div != scroll_accumulator_div) {
    scroll_accumulators_reset();
  }

  uint8_t side = is_left ? 0 : 1;
  int16_t x = scroll_accumulator_take(&scroll_accumulators[side][0], report->x,
                                      scroll_div);
  int16_t y = scroll_accumulator_take(&scroll_accumulators[side][1], report->y,
                                      scroll_div);

  output->h = -scroll_clip(x);
  output->v = scroll_clip(y);
  if (is_left) {
    output->h = -output->h;
    output->v = -output->v;
  }

#if KEYBALL_SCROLLSNAP_ENABLE == 2
  switch (keyball_get_scrollsnap_mode()) {
    case KEYBALL_SCROLLSNAP_MODE_VERTICAL:
      output->h = 0;
      break;
    case KEYBALL_SCROLLSNAP_MODE_HORIZONTAL:
      output->v = 0;
      break;
    default:
      break;
  }
#endif
}

static void ripple_restore_stock(void) {
  if (rgblight_is_enabled()) {
    rgblight_mode_noeeprom(rgblight_get_mode());
  } else {
    uint8_t count =
        is_keyboard_left() ? ripple_left_led_count : ripple_right_led_count;
    for (uint8_t i = 0; i < count; ++i) {
      rgblight_driver.set_color(i, 0, 0, 0);
    }
    rgblight_driver.flush();
  }
}

static void ripple_set_effect_mode(ripple_effect_mode_t mode) {
  ripple_effect_mode = mode;
  ripple_engine_init(&ripple_engine);
  inverse_engine_init(&inverse_engine);
  if (mode != RIPPLE_EFFECT_STOCK) {
    rgblight_timer_disable();
    ripple_last_frame = 0;
  } else {
    ripple_restore_stock();
  }
}

static void ripple_receive(uint8_t input_size, const void *input,
                           uint8_t output_size, void *output) {
  (void)output_size;
  (void)output;
  if (input_size != sizeof(ripple_packet_t)) {
    return;
  }

  const ripple_packet_t *packet = input;
  if (packet->kind == RIPPLE_PACKET_STATE) {
    ripple_effect_mode_t mode =
        ripple_effect_mode_normalize(packet->effect_mode);
    ripple_speed = lighting_speed_normalize(packet->speed);
    if (ripple_effect_mode != mode) {
      ripple_set_effect_mode(mode);
    }
  } else if (packet->kind == RIPPLE_PACKET_EVENT &&
             ripple_effect_mode != RIPPLE_EFFECT_STOCK &&
             ripple_effect_mode_normalize(packet->effect_mode) ==
                 ripple_effect_mode) {
    if (ripple_effect_mode == RIPPLE_EFFECT_INVERSE) {
      (void)inverse_engine_trigger(&inverse_engine, packet->led_index,
                                   packet->started_at);
    } else {
      ripple_engine_add(&ripple_engine, packet->x, packet->y,
                        packet->started_at);
    }
  }
}

static bool ripple_send(const ripple_packet_t *packet) {
  return is_keyboard_master() &&
         transaction_rpc_send(RIPPLE_EVENT_TRANSACTION, sizeof(*packet),
                              packet);
}

static void ripple_send_state(uint32_t now) {
  ripple_packet_t packet = {.kind = RIPPLE_PACKET_STATE,
                            .effect_mode = ripple_effect_mode,
                            .speed = ripple_speed};
  ripple_sync_record_attempt(&ripple_sync, now, ripple_send(&packet));
}

static void ripple_render(void) {
  const ripple_point_t *leds =
      is_keyboard_left() ? ripple_left_leds : ripple_right_leds;
  uint8_t count =
      is_keyboard_left() ? ripple_left_led_count : ripple_right_led_count;
  uint32_t now = sync_timer_read32();
  hsv_t hsv = rgblight_get_hsv();
  if (!rgblight_is_enabled()) {
    hsv.v = 0;
  }
  if (hsv.v > RGBLIGHT_LIMIT_VAL) {
    hsv.v = RGBLIGHT_LIMIT_VAL;
  }

  rgblight_timer_disable();
  rgb_t base_rgb = hsv_to_rgb(hsv);
  hsv_t opposite_hsv = hsv;
  opposite_hsv.h = inverse_opposite_hue(hsv.h);
  rgb_t opposite_rgb = hsv_to_rgb(opposite_hsv);

  for (uint8_t i = 0; i < count; ++i) {
    if (ripple_effect_mode == RIPPLE_EFFECT_INVERSE) {
      uint8_t global_index = is_keyboard_left() ? i : i + 30;
      inverse_rgb_t color = inverse_engine_color(
          &inverse_engine, global_index, now, ripple_speed,
          (inverse_rgb_t){base_rgb.r, base_rgb.g, base_rgb.b},
          (inverse_rgb_t){opposite_rgb.r, opposite_rgb.g, opposite_rgb.b});
      rgblight_driver.set_color(i, color.r, color.g, color.b);
      continue;
    }
    hsv_t pixel = hsv;
    pixel.v = ripple_effect_mode == RIPPLE_EFFECT_SPLASH
                  ? splash_engine_intensity(&ripple_engine, leds[i].x,
                                            leds[i].y, now, ripple_speed, hsv.v)
                  : ripple_engine_intensity(&ripple_engine, leds[i].x,
                                            leds[i].y, now, ripple_speed, hsv.v);
    rgb_t rgb = hsv_to_rgb(pixel);
    rgblight_driver.set_color(i, rgb.r, rgb.g, rgb.b);
  }
  rgblight_driver.flush();
}

void keyboard_post_init_user(void) {
  ripple_engine_init(&ripple_engine);
  inverse_engine_init(&inverse_engine);
  ripple_effect_mode = RIPPLE_EFFECT_STOCK;
  ripple_speed =
      lighting_speed_from_mode(rgblight_get_mode(), rgblight_get_speed());
  ripple_sync_init(&ripple_sync);
  if (!is_keyboard_master()) {
    transaction_register_rpc(RIPPLE_EVENT_TRANSACTION, ripple_receive);
  }
}

void housekeeping_task_user(void) {
  uint32_t now = timer_read32();
  if (is_keyboard_master() && ripple_sync_retry_due(&ripple_sync, now)) {
    ripple_send_state(now);
  }
  if (ripple_effect_mode == RIPPLE_EFFECT_STOCK ||
      timer_elapsed32(ripple_last_frame) < RIPPLE_FRAME_INTERVAL_MS) {
    return;
  }
  ripple_last_frame = timer_read32();
  ripple_render();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (keycode == SCRL_MO || (keycode == SCRL_TO && record->event.pressed)) {
    scroll_accumulators_reset();
  }

  if (!record->event.pressed) {
    return true;
  }

  if (keycode == RIPPLE_TOG || keycode == SPLASH_TOG ||
      keycode == INVERSE_TOG) {
    ripple_effect_mode_t selected =
        keycode == RIPPLE_TOG   ? RIPPLE_EFFECT_RIPPLE
        : keycode == SPLASH_TOG ? RIPPLE_EFFECT_SPLASH
                                : RIPPLE_EFFECT_INVERSE;
    if (ripple_effect_mode == RIPPLE_EFFECT_STOCK) {
      ripple_speed =
          lighting_speed_from_mode(rgblight_get_mode(), ripple_speed);
      rgblight_set_speed_noeeprom(ripple_speed);
    }
    ripple_set_effect_mode(ripple_effect_mode == selected
                               ? RIPPLE_EFFECT_STOCK
                               : selected);
    ripple_sync_mark_pending(&ripple_sync);
    ripple_send_state(timer_read32());
    return false;
  }

  if (keycode == RGB_SPI || keycode == RGB_SPD) {
    if (ripple_effect_mode != RIPPLE_EFFECT_STOCK) {
      ripple_speed = lighting_speed_step(ripple_speed, keycode == RGB_SPI);
    } else {
      ripple_speed = lighting_speed_step_for_mode(
          rgblight_get_mode(), ripple_speed, keycode == RGB_SPI);
    }
    rgblight_set_speed(ripple_speed);
    uint8_t mode =
        lighting_speed_apply_to_mode(rgblight_get_mode(), ripple_speed);
    if (mode != rgblight_get_mode()) {
      rgblight_mode(mode);
    }
    if (ripple_effect_mode != RIPPLE_EFFECT_STOCK) {
      rgblight_timer_disable();
    }
    ripple_sync_mark_pending(&ripple_sync);
    ripple_send_state(timer_read32());
    return false;
  }

  if (ripple_effect_mode != RIPPLE_EFFECT_STOCK) {
    ripple_point_t point;
    uint8_t led_index;
    if (ripple_key_position(record->event.key.row, record->event.key.col,
                            &point) &&
        ripple_key_led_index(record->event.key.row, record->event.key.col,
                             &led_index)) {
      uint32_t started_at = sync_timer_read32();
      if (ripple_effect_mode == RIPPLE_EFFECT_INVERSE) {
        (void)inverse_engine_trigger(&inverse_engine, led_index, started_at);
      } else {
        ripple_engine_add(&ripple_engine, point.x, point.y, started_at);
      }
      ripple_packet_t packet = {.kind = RIPPLE_PACKET_EVENT,
                                .effect_mode = ripple_effect_mode,
                                .x = point.x,
                                .y = point.y,
                                .started_at = started_at,
                                .led_index = led_index};
      (void)ripple_send(&packet);
    }
  }
  return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
  bool was_scroll_mode = keyball_get_scroll_mode();
  state = vial_right_layer_state_set_user(state);
  if (keyball_get_scroll_mode() != was_scroll_mode) {
    scroll_accumulators_reset();
  }
  return state;
}

#ifdef OLED_ENABLE
void oledkit_render_info_user(void) {
  vial_right_oledkit_render_info_user();
  if (ripple_effect_mode == RIPPLE_EFFECT_RIPPLE) {
    oled_write_P(PSTR(" RIP"), false);
  } else if (ripple_effect_mode == RIPPLE_EFFECT_SPLASH) {
    oled_write_P(PSTR(" SPL"), false);
  } else if (ripple_effect_mode == RIPPLE_EFFECT_INVERSE) {
    oled_write_P(PSTR(" INV"), false);
  }
}
#endif
