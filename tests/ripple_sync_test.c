#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/ripple_sync.h"

static void test_failed_send_retries_after_interval_until_acknowledged(void) {
  ripple_sync_state_t state;
  ripple_sync_init(&state);
  ripple_sync_record_attempt(&state, 100, false);

  assert(!ripple_sync_retry_due(&state, 349));
  assert(ripple_sync_retry_due(&state, 350));

  ripple_sync_record_attempt(&state, 350, false);
  assert(!ripple_sync_retry_due(&state, 599));
  assert(ripple_sync_retry_due(&state, 600));

  ripple_sync_record_attempt(&state, 600, true);
  assert(!ripple_sync_retry_due(&state, 1000));
}

static void test_new_state_is_pending_until_first_attempt(void) {
  ripple_sync_state_t state;
  ripple_sync_init(&state);
  assert(ripple_sync_retry_due(&state, 0));
}

static void test_acknowledged_state_is_resent_as_a_heartbeat(void) {
  ripple_sync_state_t state;
  ripple_sync_init(&state);
  ripple_sync_record_attempt(&state, 100, true);

  assert(!ripple_sync_retry_due(&state, 2099));
  assert(ripple_sync_retry_due(&state, 2100));

  ripple_sync_record_attempt(&state, 2100, true);
  assert(!ripple_sync_retry_due(&state, 4099));
  assert(ripple_sync_retry_due(&state, 4100));
}

static void test_effect_mode_validation(void) {
  assert(ripple_effect_mode_normalize(RIPPLE_EFFECT_STOCK) ==
         RIPPLE_EFFECT_STOCK);
  assert(ripple_effect_mode_normalize(RIPPLE_EFFECT_RIPPLE) ==
         RIPPLE_EFFECT_RIPPLE);
  assert(ripple_effect_mode_normalize(RIPPLE_EFFECT_SPLASH) ==
         RIPPLE_EFFECT_SPLASH);
  assert(ripple_effect_mode_normalize(RIPPLE_EFFECT_INVERSE) ==
         RIPPLE_EFFECT_INVERSE);
  assert(ripple_effect_mode_normalize(RIPPLE_EFFECT_HEATMAP) ==
         RIPPLE_EFFECT_HEATMAP);
  assert(ripple_effect_mode_normalize(5) == RIPPLE_EFFECT_STOCK);
  assert(ripple_effect_mode_normalize(255) == RIPPLE_EFFECT_STOCK);
}

int main(void) {
  test_failed_send_retries_after_interval_until_acknowledged();
  test_new_state_is_pending_until_first_attempt();
  test_acknowledged_state_is_resent_as_a_heartbeat();
  test_effect_mode_validation();
  return 0;
}
