#include <assert.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/heatmap_engine.h"

static void test_initializes_dark_and_rejects_invalid_indices(void) {
  heatmap_engine_t engine;
  heatmap_engine_init(&engine);
  for (uint8_t i = 0; i < HEATMAP_LED_COUNT; ++i) {
    assert(heatmap_engine_level(&engine, i, 1000, 0) == 0);
  }
  assert(!heatmap_engine_press(&engine, HEATMAP_INVALID_LED, 0, 0));
  assert(heatmap_engine_level(&engine, HEATMAP_INVALID_LED, 0, 0) == 0);
}

static void test_keys_accumulate_and_decay_independently(void) {
  heatmap_engine_t engine;
  heatmap_engine_init(&engine);
  assert(heatmap_engine_press(&engine, 2, 100, 0));
  assert(heatmap_engine_press(&engine, 2, 101, 0));
  assert(heatmap_engine_press(&engine, 58, 500, 0));
  assert(heatmap_engine_level(&engine, 2, 101, 0) == 2);
  assert(heatmap_engine_level(&engine, 58, 500, 0) == 1);
  assert(heatmap_engine_level(&engine, 2, 1301, 0) == 1);
  assert(heatmap_engine_level(&engine, 58, 1301, 0) == 1);
}

static void test_ten_rapid_presses_reach_and_saturate_at_full(void) {
  heatmap_engine_t engine;
  heatmap_engine_init(&engine);
  for (uint8_t i = 0; i < HEATMAP_MAX_LEVEL; ++i) {
    assert(heatmap_engine_press(&engine, 4, i, 3));
    assert(heatmap_engine_level(&engine, 4, i, 3) == i + 1);
  }
  assert(heatmap_engine_press(&engine, 4, 10, 3));
  assert(heatmap_engine_level(&engine, 4, 10, 3) == HEATMAP_MAX_LEVEL);
}

static void test_decay_boundaries_for_all_speeds(void) {
  static const uint16_t intervals[] = {1200, 900, 600, 300};
  for (uint8_t speed = 0; speed < 4; ++speed) {
    heatmap_engine_t engine;
    heatmap_engine_init(&engine);
    assert(heatmap_engine_interval(speed) == intervals[speed]);
    heatmap_engine_press(&engine, 0, 100, speed);
    heatmap_engine_press(&engine, 0, 101, speed);
    assert(heatmap_engine_level(&engine, 0, 100 + intervals[speed], speed) ==
           2);
    assert(heatmap_engine_level(&engine, 0, 101 + intervals[speed] - 1,
                                speed) == 2);
    assert(heatmap_engine_level(&engine, 0, 101 + intervals[speed], speed) ==
           1);
  }
  assert(heatmap_engine_interval(255) == 300);
}

static void test_full_level_is_extinguished_after_exactly_ten_intervals(void) {
  heatmap_engine_t engine;
  heatmap_engine_init(&engine);
  for (uint8_t i = 0; i < HEATMAP_MAX_LEVEL; ++i) {
    heatmap_engine_press(&engine, 1, 0, 2);
  }
  assert(heatmap_engine_level(&engine, 1, 5999, 2) == 1);
  assert(heatmap_engine_level(&engine, 1, 6000, 2) == 0);
}

static void test_repeat_press_resolves_decay_then_restarts_countdown(void) {
  heatmap_engine_t engine;
  heatmap_engine_init(&engine);
  heatmap_engine_press(&engine, 3, 0, 1);
  heatmap_engine_press(&engine, 3, 1, 1);
  heatmap_engine_press(&engine, 3, 901, 1);
  assert(heatmap_engine_level(&engine, 3, 901, 1) == 2);
  assert(heatmap_engine_level(&engine, 3, 1800, 1) == 2);
  assert(heatmap_engine_level(&engine, 3, 1801, 1) == 1);
}

static void test_speed_change_preserves_levels_and_synchronizes_countdowns(void) {
  heatmap_engine_t engine;
  heatmap_engine_init(&engine);
  heatmap_engine_press(&engine, 10, 0, 0);
  heatmap_engine_press(&engine, 10, 1, 0);
  heatmap_engine_press(&engine, 11, 400, 0);
  heatmap_engine_retime(&engine, 1201, 0);
  assert(heatmap_engine_level(&engine, 10, 1201, 3) == 1);
  assert(heatmap_engine_level(&engine, 11, 1201, 3) == 1);
  assert(heatmap_engine_level(&engine, 10, 1500, 3) == 1);
  assert(heatmap_engine_level(&engine, 11, 1500, 3) == 1);
  assert(heatmap_engine_level(&engine, 10, 1501, 3) == 0);
  assert(heatmap_engine_level(&engine, 11, 1501, 3) == 0);
}

static void test_brightness_scales_each_level_against_live_value(void) {
  heatmap_engine_t engine;
  heatmap_engine_init(&engine);
  for (uint8_t i = 0; i < 3; ++i) {
    heatmap_engine_press(&engine, 20, i, 0);
  }
  assert(heatmap_engine_brightness(&engine, 20, 2, 0, 150) == 45);
  assert(heatmap_engine_brightness(&engine, 20, 2, 0, 80) == 24);
  assert(heatmap_engine_brightness(&engine, 21, 2, 0, 150) == 0);
}

int main(void) {
  test_initializes_dark_and_rejects_invalid_indices();
  test_keys_accumulate_and_decay_independently();
  test_ten_rapid_presses_reach_and_saturate_at_full();
  test_decay_boundaries_for_all_speeds();
  test_full_level_is_extinguished_after_exactly_ten_intervals();
  test_repeat_press_resolves_decay_then_restarts_countdown();
  test_speed_change_preserves_levels_and_synchronizes_countdowns();
  test_brightness_scales_each_level_against_live_value();
  return 0;
}
