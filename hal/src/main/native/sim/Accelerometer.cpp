/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Accelerometer.h"

#include "mockdata/AccelerometerDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAccelerometer() {}
}  // namespace init
}  // namespace hal

extern "C" {
void HAL_SetAccelerometerActive(HAL_Bool active) {
  SimAccelerometerData[0].active = active;
}

void HAL_SetAccelerometerRange(HAL_AccelerometerRange range) {
  SimAccelerometerData[0].range = range;
}
double HAL_GetAccelerometerX(void) { return SimAccelerometerData[0].x; }
double HAL_GetAccelerometerY(void) { return SimAccelerometerData[0].y; }
double HAL_GetAccelerometerZ(void) { return SimAccelerometerData[0].z; }
}  // extern "C"
