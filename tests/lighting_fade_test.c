#include <assert.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/lighting_fade.h"

static void test_fade_to_black_at_exact_boundaries(void) {
  lighting_fade_t fade;
  lighting_fade_init(&fade, 0);
  assert(lighting_fade_set_target(&fade, LIGHTING_FADE_DARK, 1000));

  assert(lighting_fade_brightness(&fade, 1000) == 255);
  assert(lighting_fade_brightness(&fade, 1125) == 128);
  assert(lighting_fade_brightness(&fade, 1249) == 2);
  assert(lighting_fade_brightness(&fade, 1250) == 0);
  assert(!lighting_fade_is_transitioning(&fade, 1250));
}

static void test_fade_to_full_at_exact_boundaries(void) {
  lighting_fade_t fade = {.started_at = 1000,
                          .starting_brightness = 0,
                          .target_brightness = 255};

  assert(lighting_fade_brightness(&fade, 1000) == 0);
  assert(lighting_fade_brightness(&fade, 1125) == 127);
  assert(lighting_fade_brightness(&fade, 1249) == 253);
  assert(lighting_fade_brightness(&fade, 1250) == 255);
}

static void test_interrupted_transition_starts_from_current_brightness(void) {
  lighting_fade_t fade;
  lighting_fade_init(&fade, 0);
  assert(lighting_fade_set_target(&fade, LIGHTING_FADE_DARK, 1000));
  assert(lighting_fade_brightness(&fade, 1125) == 128);

  assert(lighting_fade_set_target(&fade, LIGHTING_FADE_FULL, 1125));
  assert(lighting_fade_brightness(&fade, 1125) == 128);
  assert(lighting_fade_brightness(&fade, 1250) == 191);
  assert(lighting_fade_brightness(&fade, 1375) == 255);
}

static void test_repeated_target_does_not_restart_transition(void) {
  lighting_fade_t fade;
  lighting_fade_init(&fade, 0);
  assert(lighting_fade_set_target(&fade, LIGHTING_FADE_DARK, 1000));
  assert(!lighting_fade_set_target(&fade, LIGHTING_FADE_DARK, 1125));
  assert(fade.started_at == 1000);
  assert(lighting_fade_brightness(&fade, 1250) == 0);
}

static void test_timer_wraparound_uses_unsigned_elapsed_time(void) {
  lighting_fade_t fade;
  lighting_fade_init(&fade, 0);
  uint32_t start = UINT32_MAX - 100;
  assert(lighting_fade_set_target(&fade, LIGHTING_FADE_DARK, start));

  assert(lighting_fade_brightness(&fade, start) == 255);
  assert(lighting_fade_brightness(&fade, start + 125) == 128);
  assert(lighting_fade_brightness(&fade, start + 249) == 2);
  assert(lighting_fade_brightness(&fade, start + 250) == 0);
}

int main(void) {
  test_fade_to_black_at_exact_boundaries();
  test_fade_to_full_at_exact_boundaries();
  test_interrupted_transition_starts_from_current_brightness();
  test_repeated_target_does_not_restart_transition();
  test_timer_wraparound_uses_unsigned_elapsed_time();
  return 0;
}
