/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL/Types.h"

enum HAL_AccelerometerRange {
  HAL_AccelerometerRange_k2G = 0,
  HAL_AccelerometerRange_k4G = 1,
  HAL_AccelerometerRange_k8G = 2,
};

extern "C" {
void HAL_SetAccelerometerActive(HAL_Bool);
void HAL_SetAccelerometerRange(HAL_AccelerometerRange);
double HAL_GetAccelerometerX();
double HAL_GetAccelerometerY();
double HAL_GetAccelerometerZ();
}
