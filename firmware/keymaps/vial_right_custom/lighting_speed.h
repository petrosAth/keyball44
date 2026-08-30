#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "rgblight.h"

uint8_t lighting_speed_normalize(uint8_t speed);
uint8_t lighting_speed_step(uint8_t speed, bool increase);
uint8_t lighting_speed_step_for_mode(uint8_t mode, uint8_t fallback,
                                     bool increase);
uint8_t lighting_speed_apply_to_mode(uint8_t mode, uint8_t speed);
uint8_t lighting_speed_from_mode(uint8_t mode, uint8_t fallback);
