#pragma once

#include <stdint.h>

typedef enum {
  RIPPLE_PACKET_STATE,
  RIPPLE_PACKET_EVENT,
} ripple_packet_kind_t;

typedef struct __attribute__((packed)) {
  uint8_t kind;
  uint8_t effect_mode;
  int16_t x;
  int16_t y;
  uint32_t started_at;
  uint8_t speed;
  uint8_t led_index;
} ripple_packet_t;

_Static_assert(sizeof(ripple_packet_t) == 12,
               "ripple split packet wire format changed");
