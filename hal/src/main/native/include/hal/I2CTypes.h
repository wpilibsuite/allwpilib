// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_i2c I2C Functions
 * @ingroup hal_capi
 * @{
 */

HAL_ENUM(HAL_I2CPort) {
  HAL_I2C_kInvalid = -1,
  HAL_I2C_kOnboard,
  HAL_I2C_kMXP
};
/** @} */
