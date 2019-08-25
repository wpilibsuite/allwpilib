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
 * @defgroup hal_spi SPI Functions
 * @ingroup hal_capi
 * @{
 */

// clang-format off
HAL_ENUM(HAL_SPIPort) {
  HAL_SPI_kInvalid = -1,
  HAL_SPI_kOnboardCS0,
  HAL_SPI_kOnboardCS1,
  HAL_SPI_kOnboardCS2,
  HAL_SPI_kOnboardCS3,
  HAL_SPI_kMXP
};
// clang-format on

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
