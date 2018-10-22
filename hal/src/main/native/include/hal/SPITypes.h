#pragma once

#include <stdint.h>

#include "hal/AnalogTrigger.h"
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
/** @} */
