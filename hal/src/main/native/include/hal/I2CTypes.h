/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_i2c I2C Functions
 * @ingroup hal_capi
 * @{
 */

// clang-format off
HAL_ENUM(HAL_I2CPort) { HAL_I2C_kInvalid = -1, HAL_I2C_kOnboard, HAL_I2C_kMXP };
// clang-format on

#ifdef __cplusplus
namespace hal {

/**
 * A move-only C++ wrapper around HAL_I2CPort.
 * Does not ensure destruction.
 */
using I2CPort = Handle<HAL_I2CPort, HAL_I2C_kInvalid>;

}  // namespace hal
#endif
/** @} */
