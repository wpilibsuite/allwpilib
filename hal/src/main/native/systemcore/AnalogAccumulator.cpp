// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogAccumulator.h"

#include "hal/HAL.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogAccumulator() {}
}  // namespace hal::init

extern "C" {

HAL_Bool HAL_IsAccumulatorChannel(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

void HAL_InitAccumulator(HAL_AnalogInputHandle analogPortHandle,
                         int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ResetAccumulator(HAL_AnalogInputHandle analogPortHandle,
                          int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analogPortHandle,
                              int32_t center, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analogPortHandle,
                                int32_t deadband, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int64_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analogPortHandle,
                              int64_t* value, int64_t* count, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

}  // extern "C"
