#pragma once

#include <stdbool.h>
#include <stdint.h>

#define HEATMAP_LED_COUNT 59
#define HEATMAP_INVALID_LED 255
#define HEATMAP_MAX_LEVEL 10

typedef struct {
  uint32_t countdown_started_at[HEATMAP_LED_COUNT];
  uint8_t level[HEATMAP_LED_COUNT];
} heatmap_engine_t;

void heatmap_engine_init(heatmap_engine_t *engine);
bool heatmap_engine_press(heatmap_engine_t *engine, uint8_t led_index,
                          uint32_t now, uint8_t speed);
uint8_t heatmap_engine_level(heatmap_engine_t *engine, uint8_t led_index,
                             uint32_t now, uint8_t speed);
uint8_t heatmap_engine_brightness(heatmap_engine_t *engine,
                                  uint8_t led_index, uint32_t now,
                                  uint8_t speed, uint8_t brightness);
void heatmap_engine_retime(heatmap_engine_t *engine, uint32_t now,
                           uint8_t old_speed);
uint16_t heatmap_engine_interval(uint8_t speed);
