#pragma once

#include <stdint.h>

typedef struct {
  int16_t remainder;
} scroll_accumulator_t;

int16_t scroll_accumulator_take(scroll_accumulator_t *accumulator,
                                int16_t movement, uint8_t scroll_div);
void scroll_accumulator_reset(scroll_accumulator_t *accumulator);
