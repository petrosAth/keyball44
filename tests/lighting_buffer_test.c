#include <assert.h>
#include <stdint.h>

#include "../firmware/keymaps/vial_right_custom/lighting_buffer.h"

static lighting_buffer_rgb_t rendered[LIGHTING_BUFFER_MAX_LEDS];
static uint8_t rendered_count;

static void capture(int index, uint8_t red, uint8_t green, uint8_t blue) {
  assert(index >= 0 && index < LIGHTING_BUFFER_MAX_LEDS);
  rendered[index] = (lighting_buffer_rgb_t){red, green, blue};
  ++rendered_count;
}

static void reset_capture(void) {
  rendered_count = 0;
  for (uint8_t i = 0; i < LIGHTING_BUFFER_MAX_LEDS; ++i) {
    rendered[i] = (lighting_buffer_rgb_t){0, 0, 0};
  }
}

static void test_scaling_and_exact_endpoints(void) {
  lighting_buffer_t buffer;
  lighting_buffer_init(&buffer, 30);
  lighting_buffer_set_color(&buffer, 0, 255, 128, 1);

  reset_capture();
  lighting_buffer_output(&buffer, 128, capture);
  assert(rendered[0].r == 128);
  assert(rendered[0].g == 64);
  assert(rendered[0].b == 0);

  reset_capture();
  lighting_buffer_output(&buffer, 0, capture);
  assert(rendered[0].r == 0 && rendered[0].g == 0 && rendered[0].b == 0);

  reset_capture();
  lighting_buffer_output(&buffer, 255, capture);
  assert(rendered[0].r == 255);
  assert(rendered[0].g == 128);
  assert(rendered[0].b == 1);
}

static void test_repeated_static_frame_output_is_not_cumulative(void) {
  lighting_buffer_t buffer;
  lighting_buffer_init(&buffer, 29);
  lighting_buffer_set_color(&buffer, 5, 200, 100, 50);

  for (uint8_t refresh = 0; refresh < 3; ++refresh) {
    reset_capture();
    lighting_buffer_output(&buffer, 128, capture);
    assert(rendered_count == 29);
    assert(rendered[5].r == 100);
    assert(rendered[5].g == 50);
    assert(rendered[5].b == 25);
  }

  reset_capture();
  lighting_buffer_output(&buffer, 255, capture);
  assert(rendered[5].r == 200);
  assert(rendered[5].g == 100);
  assert(rendered[5].b == 50);
}

static void test_left_and_right_bounds_ignore_invalid_indices(void) {
  lighting_buffer_t left;
  lighting_buffer_init(&left, 30);
  lighting_buffer_set_color_all(&left, 1, 2, 3);
  lighting_buffer_set_color(&left, -1, 9, 9, 9);
  lighting_buffer_set_color(&left, 30, 9, 9, 9);
  reset_capture();
  lighting_buffer_output(&left, 255, capture);
  assert(rendered_count == 30);
  assert(rendered[0].r == 1 && rendered[29].b == 3);

  lighting_buffer_t right;
  lighting_buffer_init(&right, 29);
  lighting_buffer_set_color_all(&right, 4, 5, 6);
  lighting_buffer_set_color(&right, 29, 9, 9, 9);
  reset_capture();
  lighting_buffer_output(&right, 255, capture);
  assert(rendered_count == 29);
  assert(rendered[28].r == 4 && rendered[28].b == 6);
  assert(rendered[29].r == 0);
}

static void test_count_is_clamped_to_storage_capacity(void) {
  lighting_buffer_t buffer;
  lighting_buffer_init(&buffer, 255);
  reset_capture();
  lighting_buffer_output(&buffer, 255, capture);
  assert(rendered_count == LIGHTING_BUFFER_MAX_LEDS);
}

int main(void) {
  test_scaling_and_exact_endpoints();
  test_repeated_static_frame_output_is_not_cumulative();
  test_left_and_right_bounds_ignore_invalid_indices();
  test_count_is_clamped_to_storage_capacity();
  return 0;
}
