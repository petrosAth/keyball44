#include "ledmap_diag_state.h"

void ledmap_diag_init(ledmap_diag_state_t *state, uint8_t local_led_count) {
    state->local_led_count = local_led_count;
    state->local_led_index = 0;
    state->enabled         = true;
}

void ledmap_diag_advance(ledmap_diag_state_t *state) {
    if (state->local_led_count == 0) {
        state->local_led_index = 0;
        return;
    }

    state->local_led_index = (state->local_led_index + 1) % state->local_led_count;
}

void ledmap_diag_toggle(ledmap_diag_state_t *state) {
    state->enabled = !state->enabled;
}

bool ledmap_diag_is_next_key(uint8_t local_row, uint8_t col) {
    return local_row == 0 && col == 0;
}

bool ledmap_diag_is_toggle_key(uint8_t local_row, uint8_t col) {
    return local_row == 0 && col == 1;
}
