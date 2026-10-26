#include "lighting_idle.h"

void lighting_idle_init(lighting_idle_t *idle, uint32_t activity_at) {
  idle->activity_at = activity_at;
  idle->dark = false;
}

bool lighting_idle_update(lighting_idle_t *idle, uint32_t activity_at,
                          uint32_t now) {
  // Input is detected by a changed timestamp rather than by elapsed time, so
  // the latched dark state survives idle periods longer than the timer range.
  if (activity_at != idle->activity_at) {
    idle->activity_at = activity_at;
    idle->dark = false;
    return false;
  }

  if (!idle->dark && now - activity_at >= LIGHTING_IDLE_TIMEOUT_MS) {
    idle->dark = true;
  }
  return idle->dark;
}
