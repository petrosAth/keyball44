#include "lighting_speed.h"

static bool mode_is_between(uint8_t mode, uint8_t first, uint8_t last) {
  return mode >= first && mode <= last;
}

static uint8_t three_level_speed(uint8_t speed) {
  speed = lighting_speed_normalize(speed);
  return speed > 2 ? 2 : speed;
}

uint8_t lighting_speed_normalize(uint8_t speed) {
  return speed > 3 ? 3 : speed;
}

uint8_t lighting_speed_step(uint8_t speed, bool increase) {
  speed = lighting_speed_normalize(speed);
  if (increase) {
    return speed < 3 ? speed + 1 : speed;
  }
  return speed > 0 ? speed - 1 : speed;
}

uint8_t lighting_speed_step_for_mode(uint8_t mode, uint8_t fallback,
                                     bool increase) {
  uint8_t speed = lighting_speed_from_mode(mode, fallback);
  uint8_t maximum = 3;

  if (mode_is_between(mode, RGBLIGHT_MODE_RAINBOW_MOOD,
                      RGBLIGHT_MODE_RAINBOW_MOOD_end) ||
      mode_is_between(mode, RGBLIGHT_MODE_RAINBOW_SWIRL,
                      RGBLIGHT_MODE_RAINBOW_SWIRL_end) ||
      mode_is_between(mode, RGBLIGHT_MODE_SNAKE, RGBLIGHT_MODE_SNAKE_end) ||
      mode_is_between(mode, RGBLIGHT_MODE_KNIGHT,
                      RGBLIGHT_MODE_KNIGHT_end) ||
      mode_is_between(mode, RGBLIGHT_MODE_TWINKLE,
                      RGBLIGHT_MODE_TWINKLE_end)) {
    maximum = 2;
  }

  if (increase) {
    return speed < maximum ? speed + 1 : speed;
  }
  return speed > 0 ? speed - 1 : speed;
}

uint8_t lighting_speed_apply_to_mode(uint8_t mode, uint8_t speed) {
  speed = lighting_speed_normalize(speed);

  if (mode_is_between(mode, RGBLIGHT_MODE_BREATHING,
                      RGBLIGHT_MODE_BREATHING_end)) {
    return RGBLIGHT_MODE_BREATHING + speed;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_RAINBOW_MOOD,
                      RGBLIGHT_MODE_RAINBOW_MOOD_end)) {
    return RGBLIGHT_MODE_RAINBOW_MOOD + three_level_speed(speed);
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_RAINBOW_SWIRL,
                      RGBLIGHT_MODE_RAINBOW_SWIRL_end)) {
    uint8_t direction = (mode - RGBLIGHT_MODE_RAINBOW_SWIRL) % 2;
    return RGBLIGHT_MODE_RAINBOW_SWIRL + three_level_speed(speed) * 2 +
           direction;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_SNAKE,
                      RGBLIGHT_MODE_SNAKE_end)) {
    uint8_t direction = (mode - RGBLIGHT_MODE_SNAKE) % 2;
    return RGBLIGHT_MODE_SNAKE + three_level_speed(speed) * 2 + direction;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_KNIGHT,
                      RGBLIGHT_MODE_KNIGHT_end)) {
    return RGBLIGHT_MODE_KNIGHT + three_level_speed(speed);
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_TWINKLE,
                      RGBLIGHT_MODE_TWINKLE_end)) {
    uint8_t color_variant = (mode - RGBLIGHT_MODE_TWINKLE) / 3;
    return RGBLIGHT_MODE_TWINKLE + color_variant * 3 +
           three_level_speed(speed);
  }
  return mode;
}

uint8_t lighting_speed_from_mode(uint8_t mode, uint8_t fallback) {
  if (mode_is_between(mode, RGBLIGHT_MODE_BREATHING,
                      RGBLIGHT_MODE_BREATHING_end)) {
    return mode - RGBLIGHT_MODE_BREATHING;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_RAINBOW_MOOD,
                      RGBLIGHT_MODE_RAINBOW_MOOD_end)) {
    return mode - RGBLIGHT_MODE_RAINBOW_MOOD;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_RAINBOW_SWIRL,
                      RGBLIGHT_MODE_RAINBOW_SWIRL_end)) {
    return (mode - RGBLIGHT_MODE_RAINBOW_SWIRL) / 2;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_SNAKE,
                      RGBLIGHT_MODE_SNAKE_end)) {
    return (mode - RGBLIGHT_MODE_SNAKE) / 2;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_KNIGHT,
                      RGBLIGHT_MODE_KNIGHT_end)) {
    return mode - RGBLIGHT_MODE_KNIGHT;
  }
  if (mode_is_between(mode, RGBLIGHT_MODE_TWINKLE,
                      RGBLIGHT_MODE_TWINKLE_end)) {
    return (mode - RGBLIGHT_MODE_TWINKLE) % 3;
  }
  return lighting_speed_normalize(fallback);
}
