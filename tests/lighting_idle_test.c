#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/lighting_idle.h"

static void test_dark_at_exact_timeout_boundary(void) {
  lighting_idle_t idle;
  lighting_idle_init(&idle, 1000);

  assert(!lighting_idle_update(&idle, 1000, 1000));
  assert(!lighting_idle_update(&idle, 1000, 1000 + 179999));
  assert(lighting_idle_update(&idle, 1000, 1000 + 180000));
  assert(lighting_idle_update(&idle, 1000, 1000 + 180001));
}

static void test_new_activity_clears_dark_and_restarts_countdown(void) {
  lighting_idle_t idle;
  lighting_idle_init(&idle, 0);
  assert(lighting_idle_update(&idle, 0, 180000));

  assert(!lighting_idle_update(&idle, 200000, 200000));
  assert(!lighting_idle_update(&idle, 200000, 200000 + 179999));
  assert(lighting_idle_update(&idle, 200000, 200000 + 180000));
}

static void test_activity_before_timeout_restarts_countdown(void) {
  lighting_idle_t idle;
  lighting_idle_init(&idle, 0);
  assert(!lighting_idle_update(&idle, 0, 179999));

  assert(!lighting_idle_update(&idle, 179999, 179999));
  assert(!lighting_idle_update(&idle, 179999, 180000));
  assert(!lighting_idle_update(&idle, 179999, 179999 + 179999));
  assert(lighting_idle_update(&idle, 179999, 179999 + 180000));
}

static void test_stays_dark_beyond_timer_range(void) {
  lighting_idle_t idle;
  lighting_idle_init(&idle, 5000);
  assert(lighting_idle_update(&idle, 5000, 5000 + 180000));

  // Elapsed time computed from the stale timestamp wraps to a small value,
  // but the lighting must remain dark until a new input arrives.
  assert(lighting_idle_update(&idle, 5000, UINT32_MAX));
  assert(lighting_idle_update(&idle, 5000, 5000));
  assert(lighting_idle_update(&idle, 5000, 5000 + 1));
  assert(lighting_idle_update(&idle, 5000, 5000 + 179999));
}

static void test_timer_wraparound_uses_unsigned_elapsed_time(void) {
  uint32_t activity = UINT32_MAX - 100;
  lighting_idle_t idle;
  lighting_idle_init(&idle, activity);

  assert(!lighting_idle_update(&idle, activity, activity + 179999));
  assert(lighting_idle_update(&idle, activity, activity + 180000));
  assert(activity + 180000 < activity);
}

static void test_init_seeds_first_countdown(void) {
  lighting_idle_t idle;
  lighting_idle_init(&idle, 42);

  assert(!idle.dark);
  assert(!lighting_idle_update(&idle, 42, 42 + 179999));
  assert(lighting_idle_update(&idle, 42, 42 + 180000));
}

int main(void) {
  test_dark_at_exact_timeout_boundary();
  test_new_activity_clears_dark_and_restarts_countdown();
  test_activity_before_timeout_restarts_countdown();
  test_stays_dark_beyond_timer_range();
  test_timer_wraparound_uses_unsigned_elapsed_time();
  test_init_seeds_first_countdown();
  return 0;
}
