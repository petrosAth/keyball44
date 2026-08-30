#pragma once

#include <stdbool.h>
#include <stdint.h>

#define RIPPLE_SYNC_RETRY_INTERVAL_MS 250
#define RIPPLE_SYNC_HEARTBEAT_INTERVAL_MS 2000

typedef enum {
  RIPPLE_EFFECT_STOCK = 0,
  RIPPLE_EFFECT_RIPPLE,
  RIPPLE_EFFECT_SPLASH,
  RIPPLE_EFFECT_INVERSE,
} ripple_effect_mode_t;

typedef struct {
  bool pending;
  bool attempted;
  uint32_t last_attempt;
} ripple_sync_state_t;

void ripple_sync_init(ripple_sync_state_t *state);
void ripple_sync_mark_pending(ripple_sync_state_t *state);
void ripple_sync_record_attempt(ripple_sync_state_t *state, uint32_t now,
                                bool succeeded);
bool ripple_sync_retry_due(const ripple_sync_state_t *state, uint32_t now);
ripple_effect_mode_t ripple_effect_mode_normalize(uint8_t mode);
