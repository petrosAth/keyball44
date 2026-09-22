#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LIGHTING_FADE_DURATION_MS 250
#define LIGHTING_FADE_FULL 255
#define LIGHTING_FADE_DARK 0

typedef struct {
  uint32_t started_at;
  uint8_t starting_brightness;
  uint8_t target_brightness;
} lighting_fade_t;

void lighting_fade_init(lighting_fade_t *fade, uint32_t now);
uint8_t lighting_fade_brightness(const lighting_fade_t *fade, uint32_t now);
bool lighting_fade_set_target(lighting_fade_t *fade, uint8_t target,
                              uint32_t now);
bool lighting_fade_set_transition(lighting_fade_t *fade, uint32_t started_at,
                                  uint8_t starting_brightness,
                                  uint8_t target_brightness);
bool lighting_fade_is_transitioning(const lighting_fade_t *fade, uint32_t now);
