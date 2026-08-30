#include <assert.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/inverse_engine.h"

static const inverse_rgb_t base = {20, 100, 220};
static const inverse_rgb_t opposite = {220, 100, 20};

static void assert_rgb(inverse_rgb_t actual, inverse_rgb_t expected) {
  assert(actual.r == expected.r);
  assert(actual.g == expected.g);
  assert(actual.b == expected.b);
}

static void test_hue_wraps_halfway_around_wheel(void) {
  assert(inverse_opposite_hue(0) == 128);
  assert(inverse_opposite_hue(127) == 255);
  assert(inverse_opposite_hue(128) == 0);
  assert(inverse_opposite_hue(255) == 127);
}

static void test_all_fade_durations(void) {
  static const uint16_t expected[] = {1400, 1100, 850, 600};
  for (uint8_t speed = 0; speed < 4; ++speed) {
    assert(inverse_engine_fade_duration(speed) == expected[speed]);
  }
  assert(inverse_engine_fade_duration(255) == 600);
}

static void test_holds_for_100ms_then_fades_and_expires(void) {
  inverse_engine_t engine;
  inverse_engine_init(&engine);
  assert_rgb(inverse_engine_color(&engine, 5, 100, 0, base, opposite), base);
  assert(inverse_engine_trigger(&engine, 5, 100));

  assert_rgb(inverse_engine_color(&engine, 5, 199, 0, base, opposite),
             opposite);
  assert_rgb(inverse_engine_color(&engine, 5, 200, 0, base, opposite),
             opposite);
  assert(inverse_engine_is_active(&engine, 5, 1599, 0));
  assert(!inverse_engine_is_active(&engine, 5, 1600, 0));
  assert_rgb(inverse_engine_color(&engine, 5, 1600, 0, base, opposite), base);
}

static void test_rgb_blend_is_monotonic(void) {
  inverse_engine_t engine;
  inverse_engine_init(&engine);
  inverse_engine_trigger(&engine, 0, 0);

  inverse_rgb_t previous = opposite;
  for (uint32_t now = INVERSE_HOLD_MS; now <= 1500; now += 100) {
    inverse_rgb_t current =
        inverse_engine_color(&engine, 0, now, 0, base, opposite);
    assert(current.r <= previous.r);
    assert(current.g == previous.g);
    assert(current.b >= previous.b);
    previous = current;
  }
  assert_rgb(previous, base);
}

static void test_retrigger_restarts_only_that_led(void) {
  inverse_engine_t engine;
  inverse_engine_init(&engine);
  inverse_engine_trigger(&engine, 2, 0);
  inverse_rgb_t fading =
      inverse_engine_color(&engine, 2, 500, 0, base, opposite);
  assert(fading.r < opposite.r);

  inverse_engine_trigger(&engine, 2, 500);
  assert_rgb(inverse_engine_color(&engine, 2, 600, 0, base, opposite),
             opposite);
}

static void test_invalid_indices_are_ignored(void) {
  inverse_engine_t engine;
  inverse_engine_init(&engine);
  assert(!inverse_engine_trigger(&engine, INVERSE_INVALID_LED, 0));
  assert(!inverse_engine_is_active(&engine, INVERSE_INVALID_LED, 0, 0));
  assert_rgb(inverse_engine_color(&engine, INVERSE_INVALID_LED, 0, 0, base,
                                  opposite),
             base);
}

static void test_simultaneous_leds_remain_independent(void) {
  inverse_engine_t engine;
  inverse_engine_init(&engine);
  inverse_engine_trigger(&engine, 10, 0);
  inverse_engine_trigger(&engine, 58, 250);

  inverse_rgb_t first =
      inverse_engine_color(&engine, 10, 300, 3, base, opposite);
  inverse_rgb_t second =
      inverse_engine_color(&engine, 58, 300, 3, base, opposite);
  assert(first.r < opposite.r && first.r > base.r);
  assert_rgb(second, opposite);
  assert(inverse_engine_is_active(&engine, 10, 300, 3));
  assert(inverse_engine_is_active(&engine, 58, 300, 3));
}

int main(void) {
  test_hue_wraps_halfway_around_wheel();
  test_all_fade_durations();
  test_holds_for_100ms_then_fades_and_expires();
  test_rgb_blend_is_monotonic();
  test_retrigger_restarts_only_that_led();
  test_invalid_indices_are_ignored();
  test_simultaneous_leds_remain_independent();
  return 0;
}
