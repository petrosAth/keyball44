#pragma once

#include <stdbool.h>
#include <stdint.h>

bool lighting_fade_config_enabled(uint32_t raw);
uint32_t lighting_fade_config_encode(bool enabled);
