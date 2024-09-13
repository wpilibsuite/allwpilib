// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

/** max length of LED strip supported by FPGA. */
#define HAL_kAddressableLEDMaxLength 5460

/** structure for holding one LED's color data. */
struct HAL_AddressableLEDData {
  uint8_t b;  ///< blue value
  uint8_t g;  ///< green value
  uint8_t r;  ///< red value
  uint8_t padding;
};
