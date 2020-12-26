// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"

/**
 * @defgroup hal_accelerometer Accelerometer Functions
 * @ingroup hal_capi
 * @{
 */

// clang-format off
/**
 * The acceptable accelerometer ranges.
 */
HAL_ENUM(HAL_AccelerometerRange) {
  HAL_AccelerometerRange_k2G = 0,
  HAL_AccelerometerRange_k4G = 1,
  HAL_AccelerometerRange_k8G = 2,
};
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Sets the accelerometer to active or standby mode.
 *
 * It must be in standby mode to change any configuration.
 *
 * @param active true to set to active, false for standby
 */
void HAL_SetAccelerometerActive(HAL_Bool active);

/**
 * Sets the range of values that can be measured (either 2, 4, or 8 g-forces).
 *
 * The accelerometer should be in standby mode when this is called.
 *
 * @param range the accelerometer range
 */
void HAL_SetAccelerometerRange(HAL_AccelerometerRange range);

/**
 * Gets the x-axis acceleration.
 *
 * This is a floating point value in units of 1 g-force.
 *
 * @return the X acceleration
 */
double HAL_GetAccelerometerX(void);

/**
 * Gets the y-axis acceleration.
 *
 * This is a floating point value in units of 1 g-force.
 *
 * @return the Y acceleration
 */
double HAL_GetAccelerometerY(void);

/**
 * Gets the z-axis acceleration.
 *
 * This is a floating point value in units of 1 g-force.
 *
 * @return the Z acceleration
 */
double HAL_GetAccelerometerZ(void);
#ifdef __cplusplus
}  // extern "C"
/** @} */
#endif
