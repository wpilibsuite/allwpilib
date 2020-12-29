// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#define HAL_kAddressableLEDMaxLength 5460

struct HAL_AddressableLEDData {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t padding;
};
