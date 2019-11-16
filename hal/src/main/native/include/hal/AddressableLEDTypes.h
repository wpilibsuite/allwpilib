#pragma once

#include <stdint.h>

struct HAL_AddressableLEDData {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t padding;
};
