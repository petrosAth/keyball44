#include "scroll_accumulator.h"

int16_t scroll_accumulator_take(scroll_accumulator_t *accumulator,
                                int16_t movement, uint8_t scroll_div) {
  int16_t divisor = 1 << (scroll_div - 1);
  int32_t total = (int32_t)accumulator->remainder + movement;
  int16_t steps = total / divisor;
  accumulator->remainder = total - (int32_t)steps * divisor;
  return steps;
}

void scroll_accumulator_reset(scroll_accumulator_t *accumulator) {
  accumulator->remainder = 0;
}
