#include "lighting_fade_sync.h"

#include <string.h>

static void lighting_fade_sync_copy(lighting_fade_packet_t *packet,
                                    const lighting_fade_t *fade) {
  packet->started_at = fade->started_at;
  packet->starting_brightness = fade->starting_brightness;
  packet->target_brightness = fade->target_brightness;
}

void lighting_fade_sync_init(lighting_fade_sync_t *sync,
                             const lighting_fade_t *fade) {
  lighting_fade_sync_copy(&sync->packet, fade);
  sync->pending = true;
  sync->attempted = false;
  sync->last_attempt = 0;
}

void lighting_fade_sync_set_transition(lighting_fade_sync_t *sync,
                                       const lighting_fade_t *fade) {
  lighting_fade_sync_copy(&sync->packet, fade);
  sync->pending = true;
  sync->attempted = false;
}

void lighting_fade_sync_record_attempt(lighting_fade_sync_t *sync,
                                       uint32_t now, bool succeeded) {
  sync->pending = !succeeded;
  sync->attempted = true;
  sync->last_attempt = now;
}

bool lighting_fade_sync_send_due(const lighting_fade_sync_t *sync,
                                 uint32_t now) {
  if (sync->pending) {
    return !sync->attempted ||
           now - sync->last_attempt >= LIGHTING_FADE_SYNC_RETRY_INTERVAL_MS;
  }
  return sync->attempted &&
         now - sync->last_attempt >= LIGHTING_FADE_SYNC_HEARTBEAT_INTERVAL_MS;
}

bool lighting_fade_sync_receive(lighting_fade_t *fade, uint8_t input_size,
                                const void *input) {
  if (input_size != sizeof(lighting_fade_packet_t) || input == NULL) {
    return false;
  }

  lighting_fade_packet_t packet;
  memcpy(&packet, input, sizeof(packet));
  if (packet.target_brightness != LIGHTING_FADE_DARK &&
      packet.target_brightness != LIGHTING_FADE_FULL) {
    return false;
  }

  (void)lighting_fade_set_transition(
      fade, packet.started_at, packet.starting_brightness,
      packet.target_brightness);
  return true;
}
