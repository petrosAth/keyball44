#pragma once

#include <stdint.h>

#define LIGHTING_BUFFER_MAX_LEDS 30

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} lighting_buffer_rgb_t;

typedef struct {
  uint8_t count;
  lighting_buffer_rgb_t colors[LIGHTING_BUFFER_MAX_LEDS];
} lighting_buffer_t;

typedef void (*lighting_buffer_output_t)(int index, uint8_t red, uint8_t green,
                                         uint8_t blue);

void lighting_buffer_init(lighting_buffer_t *buffer, uint8_t count);
void lighting_buffer_set_color(lighting_buffer_t *buffer, int index,
                               uint8_t red, uint8_t green, uint8_t blue);
void lighting_buffer_set_color_all(lighting_buffer_t *buffer, uint8_t red,
                                   uint8_t green, uint8_t blue);
void lighting_buffer_output(const lighting_buffer_t *buffer,
                            uint8_t brightness,
                            lighting_buffer_output_t output);
