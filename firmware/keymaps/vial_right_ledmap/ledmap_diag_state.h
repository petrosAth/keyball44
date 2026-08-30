#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t local_led_count;
    uint8_t local_led_index;
    bool    enabled;
} ledmap_diag_state_t;

void ledmap_diag_init(ledmap_diag_state_t *state, uint8_t local_led_count);
void ledmap_diag_advance(ledmap_diag_state_t *state);
void ledmap_diag_toggle(ledmap_diag_state_t *state);
bool ledmap_diag_is_next_key(uint8_t local_row, uint8_t col);
bool ledmap_diag_is_toggle_key(uint8_t local_row, uint8_t col);
