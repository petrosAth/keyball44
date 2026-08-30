#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "ledmap_diag_state.h"

static void test_initializes_at_first_led(void) {
    ledmap_diag_state_t state;

    ledmap_diag_init(&state, 30);

    assert(state.local_led_count == 30);
    assert(state.local_led_index == 0);
    assert(state.enabled);
}

static void test_advance_wraps_at_local_led_count(void) {
    ledmap_diag_state_t state;
    ledmap_diag_init(&state, 29);

    for (uint8_t i = 0; i < 28; ++i) {
        ledmap_diag_advance(&state);
    }
    assert(state.local_led_index == 28);

    ledmap_diag_advance(&state);
    assert(state.local_led_index == 0);
}

static void test_zero_led_count_remains_safe(void) {
    ledmap_diag_state_t state;
    ledmap_diag_init(&state, 0);

    ledmap_diag_advance(&state);

    assert(state.local_led_index == 0);
}

static void test_toggle_is_non_persistent_runtime_state(void) {
    ledmap_diag_state_t state;
    ledmap_diag_init(&state, 30);

    ledmap_diag_toggle(&state);
    assert(!state.enabled);

    ledmap_diag_toggle(&state);
    assert(state.enabled);
}

static void test_control_positions_are_available_on_either_half(void) {
    assert(ledmap_diag_is_next_key(0, 0));
    assert(ledmap_diag_is_toggle_key(0, 1));
    assert(!ledmap_diag_is_next_key(1, 0));
    assert(!ledmap_diag_is_toggle_key(0, 2));
}

int main(void) {
    test_initializes_at_first_led();
    test_advance_wraps_at_local_led_count();
    test_zero_led_count_remains_safe();
    test_toggle_is_non_persistent_runtime_state();
    test_control_positions_are_available_on_either_half();
    return 0;
}
