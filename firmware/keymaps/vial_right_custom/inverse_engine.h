#pragma once

#include <stdbool.h>
#include <stdint.h>

#define INVERSE_LED_COUNT 59
#define INVERSE_INVALID_LED 255
#define INVERSE_HOLD_MS 100U

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} inverse_rgb_t;

typedef struct {
  uint32_t started_at[INVERSE_LED_COUNT];
  bool active[INVERSE_LED_COUNT];
} inverse_engine_t;

void inverse_engine_init(inverse_engine_t *engine);
bool inverse_engine_trigger(inverse_engine_t *engine, uint8_t led_index,
                            uint32_t started_at);
bool inverse_engine_is_active(const inverse_engine_t *engine,
                              uint8_t led_index, uint32_t now, uint8_t speed);
inverse_rgb_t inverse_engine_color(const inverse_engine_t *engine,
                                   uint8_t led_index, uint32_t now,
                                   uint8_t speed, inverse_rgb_t base,
                                   inverse_rgb_t opposite);
uint8_t inverse_opposite_hue(uint8_t hue);
uint16_t inverse_engine_fade_duration(uint8_t speed);
