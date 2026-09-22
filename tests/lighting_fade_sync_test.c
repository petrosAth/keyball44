#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "../firmware/keymaps/vial_right_custom/lighting_fade_sync.h"

static void test_packet_size_and_byte_copy_round_trip(void) {
  lighting_fade_packet_t sent = {.started_at = 0x12345678,
                                 .starting_brightness = 137,
                                 .target_brightness = 0};
  uint8_t wire[sizeof(sent)];
  lighting_fade_packet_t received;
  memcpy(wire, &sent, sizeof(wire));
  memcpy(&received, wire, sizeof(received));

  assert(sizeof(sent) == 6);
  assert(received.started_at == 0x12345678);
  assert(received.starting_brightness == 137);
  assert(received.target_brightness == 0);
}

static void test_invalid_packets_are_rejected(void) {
  lighting_fade_t fade;
  lighting_fade_init(&fade, 10);
  lighting_fade_packet_t packet = {.started_at = 20,
                                   .starting_brightness = 255,
                                   .target_brightness = 1};

  assert(!lighting_fade_sync_receive(&fade, sizeof(packet) - 1, &packet));
  assert(!lighting_fade_sync_receive(&fade, sizeof(packet), &packet));
  assert(!lighting_fade_sync_receive(&fade, sizeof(packet), NULL));
  assert(fade.started_at == 10);
  assert(fade.target_brightness == 255);
}

static void test_failed_send_retries_every_sixteen_ms(void) {
  lighting_fade_t fade;
  lighting_fade_sync_t sync;
  lighting_fade_init(&fade, 0);
  lighting_fade_sync_init(&sync, &fade);
  assert(lighting_fade_sync_send_due(&sync, 0));

  lighting_fade_sync_record_attempt(&sync, 100, false);
  assert(!lighting_fade_sync_send_due(&sync, 115));
  assert(lighting_fade_sync_send_due(&sync, 116));
  lighting_fade_sync_record_attempt(&sync, 116, false);
  assert(!lighting_fade_sync_send_due(&sync, 131));
  assert(lighting_fade_sync_send_due(&sync, 132));
}

static void test_new_transition_replaces_pending_packet(void) {
  lighting_fade_t fade;
  lighting_fade_sync_t sync;
  lighting_fade_init(&fade, 0);
  lighting_fade_sync_init(&sync, &fade);
  lighting_fade_sync_record_attempt(&sync, 10, false);

  assert(lighting_fade_set_target(&fade, 0, 20));
  lighting_fade_sync_set_transition(&sync, &fade);
  assert(sync.packet.started_at == 20);
  assert(sync.packet.target_brightness == 0);
  assert(lighting_fade_sync_send_due(&sync, 20));
}

static void test_duplicate_delivery_does_not_restart_fade(void) {
  lighting_fade_t fade;
  lighting_fade_init(&fade, 0);
  lighting_fade_packet_t packet = {.started_at = 1000,
                                   .starting_brightness = 255,
                                   .target_brightness = 0};

  assert(lighting_fade_sync_receive(&fade, sizeof(packet), &packet));
  assert(lighting_fade_brightness(&fade, 1125) == 128);
  assert(lighting_fade_sync_receive(&fade, sizeof(packet), &packet));
  assert(fade.started_at == 1000);
  assert(lighting_fade_brightness(&fade, 1250) == 0);
}

static void test_delayed_delivery_catches_up_from_synchronized_timestamp(void) {
  lighting_fade_t fade;
  lighting_fade_init(&fade, 0);
  lighting_fade_packet_t packet = {.started_at = 1000,
                                   .starting_brightness = 255,
                                   .target_brightness = 0};

  assert(lighting_fade_sync_receive(&fade, sizeof(packet), &packet));
  assert(lighting_fade_brightness(&fade, 1200) == 51);
  assert(lighting_fade_brightness(&fade, 1300) == 0);
}

static void test_successful_send_gets_two_second_heartbeat(void) {
  lighting_fade_t fade;
  lighting_fade_sync_t sync;
  lighting_fade_init(&fade, 0);
  lighting_fade_sync_init(&sync, &fade);
  lighting_fade_sync_record_attempt(&sync, 100, true);

  assert(!lighting_fade_sync_send_due(&sync, 2099));
  assert(lighting_fade_sync_send_due(&sync, 2100));
  lighting_fade_sync_record_attempt(&sync, 2100, true);
  assert(!lighting_fade_sync_send_due(&sync, 4099));
  assert(lighting_fade_sync_send_due(&sync, 4100));
}

int main(void) {
  test_packet_size_and_byte_copy_round_trip();
  test_invalid_packets_are_rejected();
  test_failed_send_retries_every_sixteen_ms();
  test_new_transition_replaces_pending_packet();
  test_duplicate_delivery_does_not_restart_fade();
  test_delayed_delivery_catches_up_from_synchronized_timestamp();
  test_successful_send_gets_two_second_heartbeat();
  return 0;
}
