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

/** SPI port. */
HAL_ENUM(HAL_SPIPort) {
  /** Invalid port number. */
  HAL_SPI_kInvalid = -1,
  /** Onboard SPI bus port CS0. */
  HAL_SPI_kOnboardCS0,
  /** Onboard SPI bus port CS1. */
  HAL_SPI_kOnboardCS1,
  /** Onboard SPI bus port CS2. */
  HAL_SPI_kOnboardCS2,
  /** Onboard SPI bus port CS3. */
  HAL_SPI_kOnboardCS3,
  /** MXP (roboRIO MXP) SPI bus port. */
  HAL_SPI_kMXP
};

/** SPI mode. */
HAL_ENUM(HAL_SPIMode) {
  /** Clock idle low, data sampled on rising edge. */
  HAL_SPI_kMode0 = 0,
  /** Clock idle low, data sampled on falling edge. */
  HAL_SPI_kMode1 = 1,
  /** Clock idle high, data sampled on falling edge. */
  HAL_SPI_kMode2 = 2,
  /** Clock idle high, data sampled on rising edge. */
  HAL_SPI_kMode3 = 3,
};
/** @} */
