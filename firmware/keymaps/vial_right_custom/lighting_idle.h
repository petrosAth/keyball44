#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LIGHTING_IDLE_TIMEOUT_MS 180000u

typedef struct {
  uint32_t activity_at;
  bool dark;
} lighting_idle_t;

void lighting_idle_init(lighting_idle_t *idle, uint32_t activity_at);
bool lighting_idle_update(lighting_idle_t *idle, uint32_t activity_at,
                          uint32_t now);
