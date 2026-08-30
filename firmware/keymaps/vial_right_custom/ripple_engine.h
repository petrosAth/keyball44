#pragma once

#include <stdbool.h>
#include <stdint.h>

#define RIPPLE_EVENT_CAPACITY 8
#define SPLASH_CORE_RADIUS 95U
#define SPLASH_OUTER_RADIUS 650U

typedef struct {
  int16_t x;
  int16_t y;
  uint32_t started_at;
  bool active;
} ripple_event_t;

typedef struct {
  ripple_event_t events[RIPPLE_EVENT_CAPACITY];
  uint8_t next_event;
} ripple_engine_t;

void ripple_engine_init(ripple_engine_t *engine);
void ripple_engine_add(ripple_engine_t *engine, int16_t x, int16_t y,
                       uint32_t started_at);
uint8_t ripple_engine_intensity(const ripple_engine_t *engine, int16_t x,
                                int16_t y, uint32_t now, uint8_t speed,
                                uint8_t maximum);
uint8_t splash_engine_intensity(const ripple_engine_t *engine, int16_t x,
                                int16_t y, uint32_t now, uint8_t speed,
                                uint8_t maximum);
uint8_t ripple_engine_active_count(const ripple_engine_t *engine, uint32_t now,
                                   uint8_t speed);
uint8_t splash_engine_active_count(const ripple_engine_t *engine, uint32_t now,
                                   uint8_t speed);
uint16_t splash_engine_duration(uint8_t speed);
