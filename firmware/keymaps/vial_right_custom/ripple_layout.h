#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int16_t x;
  int16_t y;
} ripple_point_t;

extern const ripple_point_t ripple_left_leds[30];
extern const ripple_point_t ripple_right_leds[29];
extern const uint8_t ripple_left_led_count;
extern const uint8_t ripple_right_led_count;

bool ripple_key_position(uint8_t row, uint8_t col, ripple_point_t *point);
bool ripple_key_led_index(uint8_t row, uint8_t col, uint8_t *led_index);
