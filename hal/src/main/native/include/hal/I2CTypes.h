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

/** @} */
