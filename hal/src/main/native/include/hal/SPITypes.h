// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_spi SPI Functions
 * @ingroup hal_capi
 * @{
 */

HAL_ENUM(HAL_SPIPort) {
  HAL_SPI_kInvalid = -1,
  HAL_SPI_kOnboardCS0,
  HAL_SPI_kOnboardCS1,
  HAL_SPI_kOnboardCS2,
  HAL_SPI_kOnboardCS3,
  HAL_SPI_kMXP
};

HAL_ENUM(HAL_SPIMode) {
  HAL_SPI_kMode0 = 0,
  HAL_SPI_kMode1 = 1,
  HAL_SPI_kMode2 = 2,
  HAL_SPI_kMode3 = 3,
};

#ifdef __cplusplus
namespace hal {

/**
 * A move-only C++ wrapper around HAL_SPIPort.
 * Does not ensure destruction.
 */
using SPIPort = Handle<HAL_SPIPort, HAL_SPI_kInvalid>;

}  // namespace hal
#endif
/** @} */
