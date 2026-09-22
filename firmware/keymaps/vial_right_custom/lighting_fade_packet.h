#pragma once

#include <stdint.h>

typedef struct __attribute__((packed)) {
  uint32_t started_at;
  uint8_t starting_brightness;
  uint8_t target_brightness;
} lighting_fade_packet_t;

_Static_assert(sizeof(lighting_fade_packet_t) == 6,
               "lighting fade split packet wire format changed");
