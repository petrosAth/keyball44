#include "ripple_sync.h"

ripple_effect_mode_t ripple_effect_mode_normalize(uint8_t mode) {
  return mode <= RIPPLE_EFFECT_INVERSE ? (ripple_effect_mode_t)mode
                                     : RIPPLE_EFFECT_STOCK;
}

void ripple_sync_init(ripple_sync_state_t *state) {
  state->pending = true;
  state->attempted = false;
  state->last_attempt = 0;
}

void ripple_sync_mark_pending(ripple_sync_state_t *state) {
  state->pending = true;
  state->attempted = false;
}

void ripple_sync_record_attempt(ripple_sync_state_t *state, uint32_t now,
                                bool succeeded) {
  state->pending = !succeeded;
  state->attempted = true;
  state->last_attempt = now;
}

bool ripple_sync_retry_due(const ripple_sync_state_t *state, uint32_t now) {
  if (state->pending) {
    return !state->attempted ||
           now - state->last_attempt >= RIPPLE_SYNC_RETRY_INTERVAL_MS;
  }
  return state->attempted &&
         now - state->last_attempt >= RIPPLE_SYNC_HEARTBEAT_INTERVAL_MS;
}
