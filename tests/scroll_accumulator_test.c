#include <assert.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/scroll_accumulator.h"

static void test_accumulates_small_positive_movements(void) {
  scroll_accumulator_t accumulator = {0};

  assert(scroll_accumulator_take(&accumulator, 1, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, 1, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, 1, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, 1, 3) == 1);
}

static void test_accumulates_small_negative_movements(void) {
  scroll_accumulator_t accumulator = {0};

  assert(scroll_accumulator_take(&accumulator, -1, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, -1, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, -1, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, -1, 3) == -1);
}

static void test_opposite_movements_cancel(void) {
  scroll_accumulator_t accumulator = {0};

  assert(scroll_accumulator_take(&accumulator, 3, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, -3, 3) == 0);
  assert(scroll_accumulator_take(&accumulator, 4, 3) == 1);
}

static void test_divider_one_remains_immediate(void) {
  scroll_accumulator_t accumulator = {0};

  assert(scroll_accumulator_take(&accumulator, 1, 1) == 1);
  assert(scroll_accumulator_take(&accumulator, -2, 1) == -2);
}

static void test_reset_discards_partial_movement(void) {
  scroll_accumulator_t accumulator = {0};

  assert(scroll_accumulator_take(&accumulator, 3, 3) == 0);
  scroll_accumulator_reset(&accumulator);
  assert(scroll_accumulator_take(&accumulator, 1, 3) == 0);
}

int main(void) {
  test_accumulates_small_positive_movements();
  test_accumulates_small_negative_movements();
  test_opposite_movements_cancel();
  test_divider_one_remains_immediate();
  test_reset_discards_partial_movement();
  return 0;
}
