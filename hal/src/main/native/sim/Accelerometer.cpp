// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Accelerometer.h"

#include "mockdata/AccelerometerDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAccelerometer() {}
}  // namespace hal::init

extern "C" {
void HAL_SetAccelerometerActive(HAL_Bool active) {
  SimAccelerometerData[0].active = active;
}

void HAL_SetAccelerometerRange(HAL_AccelerometerRange range) {
  SimAccelerometerData[0].range = range;
}
double HAL_GetAccelerometerX(void) {
  return SimAccelerometerData[0].x;
}
double HAL_GetAccelerometerY(void) {
  return SimAccelerometerData[0].y;
}
double HAL_GetAccelerometerZ(void) {
  return SimAccelerometerData[0].z;
}
}  // extern "C"
