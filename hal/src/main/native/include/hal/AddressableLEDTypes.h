/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#define HAL_kAddressableLEDMaxLength 5460

struct HAL_AddressableLEDData {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t padding;
};
