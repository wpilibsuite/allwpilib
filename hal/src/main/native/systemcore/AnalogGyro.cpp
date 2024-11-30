// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogGyro.h"

#include <cmath>
#include <string>
#include <thread>

#include <wpi/print.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "hal/AnalogAccumulator.h"
#include "hal/AnalogInput.h"
#include "hal/handles/IndexedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogGyro() {}
}  // namespace hal::init

extern "C" {

HAL_GyroHandle HAL_InitializeAnalogGyro(HAL_AnalogInputHandle analogHandle,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_SetupAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_FreeAnalogGyro(HAL_GyroHandle handle) {}

void HAL_SetAnalogGyroParameters(HAL_GyroHandle handle,
                                 double voltsPerDegreePerSecond, double offset,
                                 int32_t center, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetAnalogGyroVoltsPerDegreePerSecond(HAL_GyroHandle handle,
                                              double voltsPerDegreePerSecond,
                                              int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ResetAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_CalibrateAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetAnalogGyroDeadband(HAL_GyroHandle handle, double volts,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

double HAL_GetAnalogGyroAngle(HAL_GyroHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetAnalogGyroRate(HAL_GyroHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetAnalogGyroOffset(HAL_GyroHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetAnalogGyroCenter(HAL_GyroHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

}  // extern "C"
