#include "lighting_fade_config.h"

// "KF", format version 1, followed by the enabled flag.
#define LIGHTING_FADE_CONFIG_DISABLED 0x4B460100u
#define LIGHTING_FADE_CONFIG_ENABLED 0x4B460101u

bool lighting_fade_config_enabled(uint32_t raw) {
  return raw == LIGHTING_FADE_CONFIG_ENABLED;
}

uint32_t lighting_fade_config_encode(bool enabled) {
  return enabled ? LIGHTING_FADE_CONFIG_ENABLED
                 : LIGHTING_FADE_CONFIG_DISABLED;
}
