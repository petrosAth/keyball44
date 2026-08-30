#include "inverse_engine.h"

#include <string.h>

static const uint16_t fade_durations[] = {1400, 1100, 850, 600};

static uint8_t blend_channel(uint8_t from, uint8_t to, uint32_t elapsed,
                             uint16_t duration) {
  return (uint8_t)(((uint32_t)from * (duration - elapsed) +
                    (uint32_t)to * elapsed) /
                   duration);
}

void inverse_engine_init(inverse_engine_t *engine) {
  memset(engine, 0, sizeof(*engine));
}

bool inverse_engine_trigger(inverse_engine_t *engine, uint8_t led_index,
                            uint32_t started_at) {
  if (led_index >= INVERSE_LED_COUNT) {
    return false;
  }
  engine->started_at[led_index] = started_at;
  engine->active[led_index] = true;
  return true;
}

uint8_t inverse_opposite_hue(uint8_t hue) {
  return (uint8_t)(hue + 128U);
}

uint16_t inverse_engine_fade_duration(uint8_t speed) {
  return fade_durations[speed > 3 ? 3 : speed];
}

bool inverse_engine_is_active(const inverse_engine_t *engine,
                              uint8_t led_index, uint32_t now, uint8_t speed) {
  if (led_index >= INVERSE_LED_COUNT || !engine->active[led_index]) {
    return false;
  }
  return now - engine->started_at[led_index] <
         INVERSE_HOLD_MS + inverse_engine_fade_duration(speed);
}

inverse_rgb_t inverse_engine_color(const inverse_engine_t *engine,
                                   uint8_t led_index, uint32_t now,
                                   uint8_t speed, inverse_rgb_t base,
                                   inverse_rgb_t opposite) {
  if (led_index >= INVERSE_LED_COUNT || !engine->active[led_index]) {
    return base;
  }

  uint32_t age = now - engine->started_at[led_index];
  if (age <= INVERSE_HOLD_MS) {
    return opposite;
  }

  uint16_t duration = inverse_engine_fade_duration(speed);
  uint32_t elapsed = age - INVERSE_HOLD_MS;
  if (elapsed >= duration) {
    return base;
  }

  inverse_rgb_t blended = {
      .r = blend_channel(opposite.r, base.r, elapsed, duration),
      .g = blend_channel(opposite.g, base.g, elapsed, duration),
      .b = blend_channel(opposite.b, base.b, elapsed, duration),
  };
  return blended;
}
