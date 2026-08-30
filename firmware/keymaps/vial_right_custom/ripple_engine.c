#include "ripple_engine.h"

#include <string.h>

#define RIPPLE_BAND_WIDTH 180U
#define RIPPLE_MAX_RADIUS 2800U

static const uint16_t splash_durations[] = {1400, 1100, 850, 600};
static const uint16_t splash_expansion_times[] = {220, 180, 140, 100};

static uint8_t ripple_rate(uint8_t speed) {
  static const uint8_t rates[] = {2, 3, 4, 5};
  return rates[speed > 3 ? 3 : speed];
}

static uint16_t integer_sqrt(uint32_t value) {
  uint32_t bit = 1UL << 30;
  uint32_t result = 0;

  while (bit > value) {
    bit >>= 2;
  }
  while (bit != 0) {
    if (value >= result + bit) {
      value -= result + bit;
      result = (result >> 1) + bit;
    } else {
      result >>= 1;
    }
    bit >>= 2;
  }
  return (uint16_t)result;
}

static uint32_t ripple_radius(const ripple_event_t *event, uint32_t now,
                              uint8_t speed) {
  return (now - event->started_at) * ripple_rate(speed);
}

uint16_t splash_engine_duration(uint8_t speed) {
  return splash_durations[speed > 3 ? 3 : speed];
}

static uint16_t splash_radius(uint32_t age, uint8_t speed) {
  uint16_t expansion_time = splash_expansion_times[speed > 3 ? 3 : speed];
  if (age >= expansion_time) {
    return SPLASH_OUTER_RADIUS;
  }
  return SPLASH_CORE_RADIUS +
         (uint32_t)(SPLASH_OUTER_RADIUS - SPLASH_CORE_RADIUS) * age /
             expansion_time;
}

void ripple_engine_init(ripple_engine_t *engine) {
  memset(engine, 0, sizeof(*engine));
}

void ripple_engine_add(ripple_engine_t *engine, int16_t x, int16_t y,
                       uint32_t started_at) {
  ripple_event_t *event = &engine->events[engine->next_event];
  event->x = x;
  event->y = y;
  event->started_at = started_at;
  event->active = true;
  engine->next_event = (engine->next_event + 1) % RIPPLE_EVENT_CAPACITY;
}

uint8_t ripple_engine_intensity(const ripple_engine_t *engine, int16_t x,
                                int16_t y, uint32_t now, uint8_t speed,
                                uint8_t maximum) {
  uint8_t brightest = 0;

  for (uint8_t i = 0; i < RIPPLE_EVENT_CAPACITY; ++i) {
    const ripple_event_t *event = &engine->events[i];
    if (!event->active) {
      continue;
    }

    uint32_t radius = ripple_radius(event, now, speed);
    if (radius > RIPPLE_MAX_RADIUS + RIPPLE_BAND_WIDTH) {
      continue;
    }

    int32_t dx = x - event->x;
    int32_t dy = y - event->y;
    uint16_t distance = integer_sqrt((uint32_t)(dx * dx + dy * dy));
    uint32_t delta = distance > radius ? distance - radius : radius - distance;
    if (delta >= RIPPLE_BAND_WIDTH) {
      continue;
    }

    uint8_t intensity =
        (uint32_t)maximum * (RIPPLE_BAND_WIDTH - delta) / RIPPLE_BAND_WIDTH;
    if (intensity > brightest) {
      brightest = intensity;
    }
  }
  return brightest;
}

uint8_t splash_engine_intensity(const ripple_engine_t *engine, int16_t x,
                                int16_t y, uint32_t now, uint8_t speed,
                                uint8_t maximum) {
  uint8_t brightest = 0;
  uint16_t duration = splash_engine_duration(speed);

  for (uint8_t i = 0; i < RIPPLE_EVENT_CAPACITY; ++i) {
    const ripple_event_t *event = &engine->events[i];
    if (!event->active) {
      continue;
    }

    uint32_t age = now - event->started_at;
    if (age >= duration) {
      continue;
    }

    int32_t dx = x - event->x;
    int32_t dy = y - event->y;
    uint16_t distance = integer_sqrt((uint32_t)(dx * dx + dy * dy));
    uint16_t radius = splash_radius(age, speed);
    if (distance >= radius) {
      continue;
    }

    uint32_t intensity = (uint32_t)maximum * (duration - age) / duration;
    if (distance > SPLASH_CORE_RADIUS) {
      intensity = intensity * (radius - distance) /
                  (radius - SPLASH_CORE_RADIUS);
    }
    if (intensity > brightest) {
      brightest = (uint8_t)intensity;
    }
  }
  return brightest;
}

uint8_t ripple_engine_active_count(const ripple_engine_t *engine, uint32_t now,
                                   uint8_t speed) {
  uint8_t count = 0;
  for (uint8_t i = 0; i < RIPPLE_EVENT_CAPACITY; ++i) {
    if (engine->events[i].active &&
        ripple_radius(&engine->events[i], now, speed) <=
            RIPPLE_MAX_RADIUS + RIPPLE_BAND_WIDTH) {
      ++count;
    }
  }
  return count;
}

uint8_t splash_engine_active_count(const ripple_engine_t *engine, uint32_t now,
                                   uint8_t speed) {
  uint8_t count = 0;
  uint16_t duration = splash_engine_duration(speed);
  for (uint8_t i = 0; i < RIPPLE_EVENT_CAPACITY; ++i) {
    if (engine->events[i].active &&
        now - engine->events[i].started_at < duration) {
      ++count;
    }
  }
  return count;
}
