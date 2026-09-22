#include "lighting_fade.h"

void lighting_fade_init(lighting_fade_t *fade, uint32_t now) {
  fade->started_at = now;
  fade->starting_brightness = LIGHTING_FADE_FULL;
  fade->target_brightness = LIGHTING_FADE_FULL;
}

uint8_t lighting_fade_brightness(const lighting_fade_t *fade, uint32_t now) {
  uint32_t elapsed = now - fade->started_at;
  if (elapsed >= LIGHTING_FADE_DURATION_MS ||
      fade->starting_brightness == fade->target_brightness) {
    return fade->target_brightness;
  }

  int16_t distance = (int16_t)fade->target_brightness -
                     (int16_t)fade->starting_brightness;
  int16_t offset =
      (int16_t)((int32_t)distance * (int32_t)elapsed /
                LIGHTING_FADE_DURATION_MS);
  return (uint8_t)((int16_t)fade->starting_brightness + offset);
}

bool lighting_fade_set_target(lighting_fade_t *fade, uint8_t target,
                              uint32_t now) {
  if (fade->target_brightness == target) {
    return false;
  }

  fade->starting_brightness = lighting_fade_brightness(fade, now);
  fade->target_brightness = target;
  fade->started_at = now;
  return true;
}

bool lighting_fade_set_transition(lighting_fade_t *fade, uint32_t started_at,
                                  uint8_t starting_brightness,
                                  uint8_t target_brightness) {
  if (fade->started_at == started_at &&
      fade->starting_brightness == starting_brightness &&
      fade->target_brightness == target_brightness) {
    return false;
  }

  fade->started_at = started_at;
  fade->starting_brightness = starting_brightness;
  fade->target_brightness = target_brightness;
  return true;
}

bool lighting_fade_is_transitioning(const lighting_fade_t *fade,
                                    uint32_t now) {
  return fade->starting_brightness != fade->target_brightness &&
         now - fade->started_at < LIGHTING_FADE_DURATION_MS;
}
