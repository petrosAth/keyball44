#include <assert.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/ripple_engine.h"
#include "../firmware/keymaps/vial_right_custom/ripple_layout.h"

static void test_starts_empty_and_peaks_at_origin(void) {
  ripple_engine_t engine;
  ripple_engine_init(&engine);
  assert(ripple_engine_intensity(&engine, 0, 0, 100, 1, 150) == 0);

  ripple_engine_add(&engine, 100, 200, 100);
  assert(ripple_engine_intensity(&engine, 100, 200, 100, 1, 150) == 150);
}

static void test_wave_moves_and_overlaps_use_maximum(void) {
  ripple_engine_t engine;
  ripple_engine_init(&engine);
  ripple_engine_add(&engine, 0, 0, 0);

  assert(ripple_engine_intensity(&engine, 600, 0, 200, 1, 120) == 120);
  assert(ripple_engine_intensity(&engine, 0, 0, 200, 1, 120) == 0);

  ripple_engine_add(&engine, 600, 0, 200);
  assert(ripple_engine_intensity(&engine, 600, 0, 200, 1, 120) == 120);
}

static void test_ring_replaces_oldest_of_eight(void) {
  ripple_engine_t engine;
  ripple_engine_init(&engine);
  for (uint8_t i = 0; i < RIPPLE_EVENT_CAPACITY; ++i) {
    ripple_engine_add(&engine, i * 100, 0, i);
  }
  assert(ripple_engine_active_count(&engine, 7, 1) == 8);

  ripple_engine_add(&engine, 900, 0, 8);
  assert(engine.events[0].x == 900);
  assert(engine.events[0].started_at == 8);
  assert(ripple_engine_active_count(&engine, 8, 1) == 8);
}

static void test_expiry_and_speed(void) {
  ripple_engine_t slow;
  ripple_engine_t fast;
  ripple_engine_init(&slow);
  ripple_engine_init(&fast);
  ripple_engine_add(&slow, 0, 0, 0);
  ripple_engine_add(&fast, 0, 0, 0);

  assert(ripple_engine_intensity(&slow, 400, 0, 200, 0, 100) == 100);
  assert(ripple_engine_intensity(&fast, 1000, 0, 200, 3, 100) == 100);
  assert(ripple_engine_active_count(&fast, 1000, 3) == 0);
}

static void test_splash_origin_expansion_and_falloff(void) {
  ripple_engine_t engine;
  ripple_engine_init(&engine);
  ripple_engine_add(&engine, 0, 0, 0);

  assert(splash_engine_intensity(&engine, 0, 0, 0, 3, 150) == 150);
  assert(splash_engine_intensity(&engine, SPLASH_CORE_RADIUS - 1, 0, 0, 3,
                                 150) == 150);
  assert(splash_engine_intensity(&engine, SPLASH_CORE_RADIUS, 0, 0, 3, 150) ==
         0);

  /* At speed 3 the bloom reaches 372 units after half its expansion time. */
  assert(splash_engine_intensity(&engine, 300, 0, 50, 3, 150) > 0);
  assert(splash_engine_intensity(&engine, 372, 0, 50, 3, 150) == 0);

  uint8_t core = splash_engine_intensity(&engine, SPLASH_CORE_RADIUS, 0, 100,
                                         3, 150);
  uint8_t middle = splash_engine_intensity(&engine, 372, 0, 100, 3, 150);
  uint8_t edge = splash_engine_intensity(&engine, 640, 0, 100, 3, 150);
  assert(core > middle && middle > edge);
}

static void test_splash_fades_monotonically_and_expires(void) {
  ripple_engine_t engine;
  ripple_engine_init(&engine);
  ripple_engine_add(&engine, 0, 0, 0);

  uint8_t previous = 200;
  for (uint32_t now = 0; now < 600; now += 50) {
    uint8_t current = splash_engine_intensity(&engine, 0, 0, now, 3, 200);
    assert(current <= previous);
    previous = current;
  }
  assert(splash_engine_intensity(&engine, 0, 0, 600, 3, 200) == 0);
  assert(splash_engine_active_count(&engine, 599, 3) == 1);
  assert(splash_engine_active_count(&engine, 600, 3) == 0);
}

static void test_splash_all_speeds(void) {
  static const uint16_t expected_durations[] = {1400, 1100, 850, 600};
  ripple_engine_t engine;
  ripple_engine_init(&engine);
  ripple_engine_add(&engine, 0, 0, 0);

  for (uint8_t speed = 0; speed < 4; ++speed) {
    uint16_t duration = expected_durations[speed];
    assert(splash_engine_duration(speed) == duration);
    assert(splash_engine_intensity(&engine, 0, 0, duration / 2, speed, 200) ==
           100);
    assert(splash_engine_active_count(&engine, duration - 1, speed) == 1);
    assert(splash_engine_active_count(&engine, duration, speed) == 0);
  }
  assert(splash_engine_duration(255) == expected_durations[3]);
}

static void test_splash_overlaps_use_maximum_and_queue_replaces_oldest(void) {
  ripple_engine_t first;
  ripple_engine_t second;
  ripple_engine_t both;
  ripple_engine_init(&first);
  ripple_engine_init(&second);
  ripple_engine_init(&both);
  ripple_engine_add(&first, 0, 0, 0);
  ripple_engine_add(&second, 400, 0, 200);
  ripple_engine_add(&both, 0, 0, 0);
  ripple_engine_add(&both, 400, 0, 200);

  uint8_t from_first = splash_engine_intensity(&first, 300, 0, 250, 1, 150);
  uint8_t from_second = splash_engine_intensity(&second, 300, 0, 250, 1, 150);
  uint8_t combined = splash_engine_intensity(&both, 300, 0, 250, 1, 150);
  assert(combined == (from_first > from_second ? from_first : from_second));

  ripple_engine_init(&both);
  for (uint8_t i = 0; i < RIPPLE_EVENT_CAPACITY; ++i) {
    ripple_engine_add(&both, i * 100, 0, i);
  }
  ripple_engine_add(&both, 900, 0, 8);
  assert(both.events[0].x == 900);
  assert(splash_engine_active_count(&both, 8, 0) == 8);
}

static void test_derived_layout(void) {
  assert(ripple_left_led_count == 30);
  assert(ripple_right_led_count == 29);
  assert(ripple_left_leds[0].x == 953 && ripple_left_leds[0].y == -50);
  assert(ripple_right_leds[0].x == 906 && ripple_right_leds[0].y == 587);

  ripple_point_t point;
  assert(ripple_key_position(0, 0, &point));
  assert(point.x == 0 && point.y == 67);
  assert(ripple_key_position(0, 5, &point));
  assert(point.x == 953 && point.y == 0);
  assert(ripple_key_position(7, 5, &point));
  assert(point.x == 850 && point.y == 693);
  assert(!ripple_key_position(3, 0, &point));
  assert(!ripple_key_position(7, 0, &point));
}

static void test_every_matrix_position_maps_to_its_resolved_led(void) {
  static const uint8_t none = 255;
  static const uint8_t expected[8][6] = {
      {17, 14, 10, 6, 3, 0},
      {18, 15, 11, 7, 4, 1},
      {19, 16, 12, 8, 5, 2},
      {none, 13, 9, 27, 28, 29},
      {40, 43, 47, 50, 53, 56},
      {41, 44, 48, 51, 54, 57},
      {42, 45, 49, 52, 55, 58},
      {none, 46, none, none, 31, 30},
  };

  for (uint8_t row = 0; row < 8; ++row) {
    for (uint8_t col = 0; col < 6; ++col) {
      uint8_t led_index = none;
      bool valid = ripple_key_led_index(row, col, &led_index);
      assert(valid == (expected[row][col] != none));
      if (valid) {
        assert(led_index == expected[row][col]);
      }
    }
  }

  uint8_t led_index;
  assert(!ripple_key_led_index(8, 0, &led_index));
  assert(!ripple_key_led_index(0, 6, &led_index));
}

int main(void) {
  test_starts_empty_and_peaks_at_origin();
  test_wave_moves_and_overlaps_use_maximum();
  test_ring_replaces_oldest_of_eight();
  test_expiry_and_speed();
  test_splash_origin_expansion_and_falloff();
  test_splash_fades_monotonically_and_expires();
  test_splash_all_speeds();
  test_splash_overlaps_use_maximum_and_queue_replaces_oldest();
  test_derived_layout();
  test_every_matrix_position_maps_to_its_resolved_led();
  return 0;
}
