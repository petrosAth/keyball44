#pragma once

#include <stdint.h>

void lighting_driver_set_brightness(uint8_t brightness);
uint8_t lighting_driver_get_brightness(void);
void lighting_driver_refresh(void);
