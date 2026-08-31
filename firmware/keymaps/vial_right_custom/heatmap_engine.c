#include "heatmap_engine.h"

#include <string.h>

static const uint16_t decay_intervals[] = {1200, 900, 600, 300};

uint16_t heatmap_engine_interval(uint8_t speed) {
  return decay_intervals[speed > 3 ? 3 : speed];
}

void heatmap_engine_init(heatmap_engine_t *engine) {
  memset(engine, 0, sizeof(*engine));
}

uint8_t heatmap_engine_level(heatmap_engine_t *engine, uint8_t led_index,
                             uint32_t now, uint8_t speed) {
  if (led_index >= HEATMAP_LED_COUNT || engine->level[led_index] == 0) {
    return 0;
  }

  uint16_t interval = heatmap_engine_interval(speed);
  uint32_t elapsed = now - engine->countdown_started_at[led_index];
  uint32_t decrements = elapsed / interval;
  if (decrements == 0) {
    return engine->level[led_index];
  }
  if (decrements >= engine->level[led_index]) {
    engine->level[led_index] = 0;
    engine->countdown_started_at[led_index] = 0;
    return 0;
  }

  engine->level[led_index] -= (uint8_t)decrements;
  engine->countdown_started_at[led_index] += decrements * interval;
  return engine->level[led_index];
}

bool heatmap_engine_press(heatmap_engine_t *engine, uint8_t led_index,
                          uint32_t now, uint8_t speed) {
  if (led_index >= HEATMAP_LED_COUNT) {
    return false;
  }
  uint8_t level = heatmap_engine_level(engine, led_index, now, speed);
  engine->level[led_index] =
      level < HEATMAP_MAX_LEVEL ? level + 1 : HEATMAP_MAX_LEVEL;
  engine->countdown_started_at[led_index] = now;
  return true;
}

uint8_t heatmap_engine_brightness(heatmap_engine_t *engine,
                                  uint8_t led_index, uint32_t now,
                                  uint8_t speed, uint8_t brightness) {
  return (uint8_t)(((uint16_t)brightness *
                    heatmap_engine_level(engine, led_index, now, speed)) /
                   HEATMAP_MAX_LEVEL);
}

void heatmap_engine_retime(heatmap_engine_t *engine, uint32_t now,
                           uint8_t old_speed) {
  for (uint8_t i = 0; i < HEATMAP_LED_COUNT; ++i) {
    if (heatmap_engine_level(engine, i, now, old_speed) > 0) {
      engine->countdown_started_at[i] = now;
    }
  }
}
