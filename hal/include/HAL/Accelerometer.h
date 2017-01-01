/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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
void HAL_SetAccelerometerActive(HAL_Bool active);
void HAL_SetAccelerometerRange(HAL_AccelerometerRange range);
double HAL_GetAccelerometerX(void);
double HAL_GetAccelerometerY(void);
double HAL_GetAccelerometerZ(void);
#ifdef __cplusplus
}
#endif
