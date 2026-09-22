#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lighting_fade.h"
#include "lighting_fade_packet.h"

#define LIGHTING_FADE_SYNC_RETRY_INTERVAL_MS 16
#define LIGHTING_FADE_SYNC_HEARTBEAT_INTERVAL_MS 2000

typedef struct {
  lighting_fade_packet_t packet;
  bool pending;
  bool attempted;
  uint32_t last_attempt;
} lighting_fade_sync_t;

void lighting_fade_sync_init(lighting_fade_sync_t *sync,
                             const lighting_fade_t *fade);
void lighting_fade_sync_set_transition(lighting_fade_sync_t *sync,
                                       const lighting_fade_t *fade);
void lighting_fade_sync_record_attempt(lighting_fade_sync_t *sync,
                                       uint32_t now, bool succeeded);
bool lighting_fade_sync_send_due(const lighting_fade_sync_t *sync,
                                 uint32_t now);
bool lighting_fade_sync_receive(lighting_fade_t *fade, uint8_t input_size,
                                const void *input);
