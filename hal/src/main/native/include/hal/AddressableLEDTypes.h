// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/Types.h>
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

/**
 * Order that color data is sent over the wire.
 */
HAL_ENUM(HAL_AddressableLEDColorOrder) {
  HAL_ALED_RGB,
  HAL_ALED_RBG,
  HAL_ALED_BGR,
  HAL_ALED_BRG,
  HAL_ALED_GBR,
  HAL_ALED_GRB
};

#ifdef __cplusplus
constexpr auto format_as(HAL_AddressableLEDColorOrder order) {
  return static_cast<int32_t>(order);
}
#endif
