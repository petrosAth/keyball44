#include "lighting_buffer.h"

static uint8_t lighting_buffer_scale(uint8_t value, uint8_t brightness) {
  return (uint8_t)(((uint16_t)value * brightness) / 255);
}

void lighting_buffer_init(lighting_buffer_t *buffer, uint8_t count) {
  buffer->count = count > LIGHTING_BUFFER_MAX_LEDS
                      ? LIGHTING_BUFFER_MAX_LEDS
                      : count;
  for (uint8_t i = 0; i < LIGHTING_BUFFER_MAX_LEDS; ++i) {
    buffer->colors[i] = (lighting_buffer_rgb_t){0, 0, 0};
  }
}

void lighting_buffer_set_color(lighting_buffer_t *buffer, int index,
                               uint8_t red, uint8_t green, uint8_t blue) {
  if (index < 0 || index >= buffer->count) {
    return;
  }
  buffer->colors[index] = (lighting_buffer_rgb_t){red, green, blue};
}

void lighting_buffer_set_color_all(lighting_buffer_t *buffer, uint8_t red,
                                   uint8_t green, uint8_t blue) {
  for (uint8_t i = 0; i < buffer->count; ++i) {
    lighting_buffer_set_color(buffer, i, red, green, blue);
  }
}

void lighting_buffer_output(const lighting_buffer_t *buffer,
                            uint8_t brightness,
                            lighting_buffer_output_t output) {
  for (uint8_t i = 0; i < buffer->count; ++i) {
    output(i, lighting_buffer_scale(buffer->colors[i].r, brightness),
           lighting_buffer_scale(buffer->colors[i].g, brightness),
           lighting_buffer_scale(buffer->colors[i].b, brightness));
  }
}
