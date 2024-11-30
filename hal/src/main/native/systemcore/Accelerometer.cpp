// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Accelerometer.h"

#include <stdint.h>

#include <cassert>
#include <cstdio>
#include <memory>

#include "HALInitializer.h"
#include "hal/HAL.h"

using namespace hal;

namespace hal::init {
void InitializeAccelerometer() {}
}  // namespace hal::init

namespace hal {

/**
 * Initialize the accelerometer.
 */
static void initializeAccelerometer() {
  hal::init::CheckInit();
}

}  // namespace hal

extern "C" {

void HAL_SetAccelerometerActive(HAL_Bool active) {
  initializeAccelerometer();
}

void HAL_SetAccelerometerRange(HAL_AccelerometerRange range) {
  initializeAccelerometer();
}

double HAL_GetAccelerometerX(void) {
  initializeAccelerometer();
  return 0.0;
}

double HAL_GetAccelerometerY(void) {
  initializeAccelerometer();
  return 0.0;
}

double HAL_GetAccelerometerZ(void) {
  initializeAccelerometer();
  return 0.0;
}

}  // extern "C"
