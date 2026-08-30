#include "ripple_layout.h"

/* Coordinates are tenths of a millimetre, derived from the Yowkees/keyball
 * revision pinned in dependencies.lock. The nominal gap between the inner key
 * centers is one 19.05 mm key pitch. */
const ripple_point_t ripple_left_leds[30] = {
    {953, -50},  {953, 141},  {953, 331},  {762, -75}, {762, 116},  {762, 306},
    {572, -100}, {572, 91},   {572, 281},  {572, 472}, {381, -53},  {381, 138},
    {381, 328},  {381, 519},  {191, 17},   {191, 208}, {191, 398},  {0, 17},
    {0, 208},    {0, 398},    {857, -50},  {470, -58}, {95, 17},    {0, 164},
    {0, 354},    {381, 475},  {571, 427},  {792, 524}, {1012, 548}, {1181, 606},
};

const ripple_point_t ripple_right_leds[29] = {
    {906, 587},   {1017, 534}, {1334, 245}, {1524, 401}, {1808, 595},
    {2096, 330},  {2095, 141}, {2000, -1},  {1626, -79}, {1419, -105},
    {2096, -2},   {2096, 189}, {2096, 379}, {1905, -2},  {1905, 189},
    {1905, 379},  {1905, 570}, {1715, -72}, {1715, 119}, {1715, 309},
    {1524, -119}, {1524, 72},  {1524, 262}, {1334, -94}, {1334, 97},
    {1334, 287},  {1143, -69}, {1143, 122}, {1143, 312},
};

const uint8_t ripple_left_led_count = 30;
const uint8_t ripple_right_led_count = 29;

typedef struct {
  ripple_point_t point;
  bool valid;
} ripple_key_point_t;

static const ripple_key_point_t key_positions[8][6] = {
    {{{0, 67}, true},
     {{191, 67}, true},
     {{381, -3}, true},
     {{572, -50}, true},
     {{762, -25}, true},
     {{953, 0}, true}},
    {{{0, 258}, true},
     {{191, 258}, true},
     {{381, 188}, true},
     {{572, 141}, true},
     {{762, 166}, true},
     {{953, 191}, true}},
    {{{0, 448}, true},
     {{191, 448}, true},
     {{381, 378}, true},
     {{572, 331}, true},
     {{762, 356}, true},
     {{953, 381}, true}},
    {{{0, 0}, false},
     {{381, 569}, true},
     {{572, 522}, true},
     {{820, 639}, true},
     {{1036, 656}, true},
     {{1245, 712}, true}},
    {{{2096, 48}, true},
     {{1905, 48}, true},
     {{1715, -22}, true},
     {{1524, -69}, true},
     {{1334, -44}, true},
     {{1143, -19}, true}},
    {{{2096, 238}, true},
     {{1905, 238}, true},
     {{1715, 168}, true},
     {{1524, 121}, true},
     {{1334, 146}, true},
     {{1143, 171}, true}},
    {{{2096, 429}, true},
     {{1905, 429}, true},
     {{1715, 359}, true},
     {{1524, 312}, true},
     {{1334, 337}, true},
     {{1143, 362}, true}},
    {{{0, 0}, false},
     {{1905, 619}, true},
     {{0, 0}, false},
     {{0, 0}, false},
     {{1060, 637}, true},
     {{850, 693}, true}},
};

#define NO_LED 255

/* Global LED indices. Thumb switches without a dedicated LED use their
 * nearest underglow LED: left local 27-29 and right local 1 and 0. */
static const uint8_t key_led_indices[8][6] = {
    {17, 14, 10, 6, 3, 0},
    {18, 15, 11, 7, 4, 1},
    {19, 16, 12, 8, 5, 2},
    {NO_LED, 13, 9, 27, 28, 29},
    {40, 43, 47, 50, 53, 56},
    {41, 44, 48, 51, 54, 57},
    {42, 45, 49, 52, 55, 58},
    {NO_LED, 46, NO_LED, NO_LED, 31, 30},
};

bool ripple_key_position(uint8_t row, uint8_t col, ripple_point_t *point) {
  if (row >= 8 || col >= 6 || !key_positions[row][col].valid) {
    return false;
  }
  *point = key_positions[row][col].point;
  return true;
}

bool ripple_key_led_index(uint8_t row, uint8_t col, uint8_t *led_index) {
  if (row >= 8 || col >= 6 || key_led_indices[row][col] == NO_LED) {
    return false;
  }
  *led_index = key_led_indices[row][col];
  return true;
}
