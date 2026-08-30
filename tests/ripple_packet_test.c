#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "../firmware/keymaps/vial_right_custom/ripple_packet.h"

static void test_event_packet_round_trip_retains_all_event_data(void) {
  ripple_packet_t sent = {.kind = RIPPLE_PACKET_EVENT,
                          .effect_mode = 3,
                          .x = -123,
                          .y = 456,
                          .started_at = 0x12345678,
                          .speed = 2,
                          .led_index = 58};
  uint8_t wire[sizeof(sent)];
  ripple_packet_t received;
  memcpy(wire, &sent, sizeof(wire));
  memcpy(&received, wire, sizeof(received));

  assert(sizeof(sent) == 12);
  assert(received.kind == RIPPLE_PACKET_EVENT);
  assert(received.effect_mode == 3);
  assert(received.x == -123);
  assert(received.y == 456);
  assert(received.started_at == 0x12345678);
  assert(received.speed == 2);
  assert(received.led_index == 58);
}

static void test_state_packet_retains_mode_and_speed(void) {
  ripple_packet_t packet = {
      .kind = RIPPLE_PACKET_STATE, .effect_mode = 3, .speed = 3};
  assert(packet.effect_mode == 3);
  assert(packet.speed == 3);
}

int main(void) {
  test_event_packet_round_trip_retains_all_event_data();
  test_state_packet_retains_mode_and_speed();
  return 0;
}
