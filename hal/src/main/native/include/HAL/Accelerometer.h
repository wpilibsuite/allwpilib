/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL/Types.h"

enum HAL_AccelerometerRange : int32_t {
  HAL_AccelerometerRange_k2G = 0,
  HAL_AccelerometerRange_k4G = 1,
  HAL_AccelerometerRange_k8G = 2,
};

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Set the accelerometer to active or standby mode.  It must be in standby
 * mode to change any configuration.
 */
void HAL_SetAccelerometerActive(HAL_Bool active);

/**
 * Set the range of values that can be measured (either 2, 4, or 8 g-forces).
 * The accelerometer should be in standby mode when this is called.
 */
void HAL_SetAccelerometerRange(HAL_AccelerometerRange range);

/**
 * Get the x-axis acceleration
 *
 * This is a floating point value in units of 1 g-force
 */
double HAL_GetAccelerometerX(void);

/**
 * Get the y-axis acceleration
 *
 * This is a floating point value in units of 1 g-force
 */
double HAL_GetAccelerometerY(void);

/**
 * Get the z-axis acceleration
 *
 * This is a floating point value in units of 1 g-force
 */
double HAL_GetAccelerometerZ(void);
#ifdef __cplusplus
}  // extern "C"
#endif
