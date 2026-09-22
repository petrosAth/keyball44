#include "lighting_driver.h"

#include "keyboard.h"
#include "lighting_buffer.h"
#include "rgblight_drivers.h"
#include "ws2812.h"

static lighting_buffer_t lighting_buffer;
static uint8_t lighting_brightness = 255;

static void lighting_driver_init(void) {
  lighting_buffer_init(&lighting_buffer, is_keyboard_left() ? 30 : 29);
  lighting_brightness = 255;
  ws2812_init();
}

static void lighting_driver_set_color(int index, uint8_t red, uint8_t green,
                                      uint8_t blue) {
  lighting_buffer_set_color(&lighting_buffer, index, red, green, blue);
}

static void lighting_driver_set_color_all(uint8_t red, uint8_t green,
                                          uint8_t blue) {
  lighting_buffer_set_color_all(&lighting_buffer, red, green, blue);
}

void lighting_driver_refresh(void) {
  lighting_buffer_output(&lighting_buffer, lighting_brightness,
                         ws2812_set_color);
  ws2812_flush();
}

static void lighting_driver_flush(void) { lighting_driver_refresh(); }

void lighting_driver_set_brightness(uint8_t brightness) {
  lighting_brightness = brightness;
}

uint8_t lighting_driver_get_brightness(void) { return lighting_brightness; }

const rgblight_driver_t rgblight_driver = {
    .init = lighting_driver_init,
    .set_color = lighting_driver_set_color,
    .set_color_all = lighting_driver_set_color_all,
    .flush = lighting_driver_flush,
};
