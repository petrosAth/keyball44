#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/lighting_speed.h"

static void test_normalizes_vial_values_and_stops_at_boundaries(void) {
  assert(lighting_speed_normalize(0) == 0);
  assert(lighting_speed_normalize(3) == 3);
  assert(lighting_speed_normalize(255) == 3);

  assert(lighting_speed_step(0, false) == 0);
  assert(lighting_speed_step(0, true) == 1);
  assert(lighting_speed_step(3, true) == 3);
  assert(lighting_speed_step(3, false) == 2);
}

static void test_applies_native_speed_variants_without_changing_effect(void) {
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_BREATHING, 3) ==
         RGBLIGHT_MODE_BREATHING + 3);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_RAINBOW_MOOD, 3) ==
         RGBLIGHT_MODE_RAINBOW_MOOD + 2);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_KNIGHT + 1, 0) ==
         RGBLIGHT_MODE_KNIGHT);
}

static void test_preserves_direction_and_color_variants(void) {
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_RAINBOW_SWIRL, 2) ==
         RGBLIGHT_MODE_RAINBOW_SWIRL + 4);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_RAINBOW_SWIRL + 1, 2) ==
         RGBLIGHT_MODE_RAINBOW_SWIRL + 5);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_SNAKE + 1, 1) ==
         RGBLIGHT_MODE_SNAKE + 3);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_TWINKLE + 3, 1) ==
         RGBLIGHT_MODE_TWINKLE + 4);
}

static void test_leaves_effects_without_native_speed_variants_unchanged(void) {
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_STATIC_LIGHT, 3) ==
         RGBLIGHT_MODE_STATIC_LIGHT);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_CHRISTMAS, 3) ==
         RGBLIGHT_MODE_CHRISTMAS);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_STATIC_GRADIENT + 4, 3) ==
         RGBLIGHT_MODE_STATIC_GRADIENT + 4);
  assert(lighting_speed_apply_to_mode(RGBLIGHT_MODE_RGB_TEST, 3) ==
         RGBLIGHT_MODE_RGB_TEST);
}

static void test_recovers_speed_from_mode_or_normalized_fallback(void) {
  assert(lighting_speed_from_mode(RGBLIGHT_MODE_BREATHING + 3, 0) == 3);
  assert(lighting_speed_from_mode(RGBLIGHT_MODE_RAINBOW_SWIRL + 5, 0) == 2);
  assert(lighting_speed_from_mode(RGBLIGHT_MODE_SNAKE + 2, 0) == 1);
  assert(lighting_speed_from_mode(RGBLIGHT_MODE_TWINKLE + 4, 0) == 1);
  assert(lighting_speed_from_mode(RGBLIGHT_MODE_STATIC_LIGHT, 255) == 3);
}

static void test_stock_step_uses_current_variant_and_native_boundaries(void) {
  assert(lighting_speed_step_for_mode(RGBLIGHT_MODE_BREATHING, 3, false) == 0);
  assert(lighting_speed_step_for_mode(RGBLIGHT_MODE_BREATHING, 3, true) == 1);
  assert(lighting_speed_step_for_mode(RGBLIGHT_MODE_RAINBOW_MOOD + 2, 0,
                                      true) == 2);
  assert(lighting_speed_step_for_mode(RGBLIGHT_MODE_RAINBOW_MOOD + 2, 0,
                                      false) == 1);
  assert(lighting_speed_step_for_mode(RGBLIGHT_MODE_STATIC_LIGHT, 1, true) ==
         2);
}

int main(void) {
  test_normalizes_vial_values_and_stops_at_boundaries();
  test_applies_native_speed_variants_without_changing_effect();
  test_preserves_direction_and_color_variants();
  test_leaves_effects_without_native_speed_variants_unchanged();
  test_recovers_speed_from_mode_or_normalized_fallback();
  test_stock_step_uses_current_variant_and_native_boundaries();
  return 0;
}
