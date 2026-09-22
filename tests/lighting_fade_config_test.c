#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/lighting_fade_config.h"

static void test_encoded_values_round_trip(void) {
  assert(!lighting_fade_config_enabled(lighting_fade_config_encode(false)));
  assert(lighting_fade_config_enabled(lighting_fade_config_encode(true)));
  assert(lighting_fade_config_encode(false) !=
         lighting_fade_config_encode(true));
}

static void test_erased_legacy_and_invalid_values_default_to_disabled(void) {
  assert(!lighting_fade_config_enabled(UINT32_MAX));
  assert(!lighting_fade_config_enabled(0));
  assert(!lighting_fade_config_enabled(1));
  assert(!lighting_fade_config_enabled(0x4B460102u));
}

int main(void) {
  test_encoded_values_round_trip();
  test_erased_legacy_and_invalid_values_default_to_disabled();
  return 0;
}
