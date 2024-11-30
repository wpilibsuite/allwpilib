// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DutyCycle.h"

#include <memory>

#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeDutyCycle() {}
}  // namespace hal::init

extern "C" {
HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle,
                                            HAL_AnalogTriggerType triggerType,
                                            int32_t* status) {
  hal::init::CheckInit();

  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle) {}

void HAL_SetDutyCycleSimDevice(HAL_EncoderHandle handle,
                               HAL_SimDeviceHandle device) {}

int32_t HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetDutyCycleOutput(HAL_DutyCycleHandle dutyCycleHandle,
                              int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetDutyCycleHighTime(HAL_DutyCycleHandle dutyCycleHandle,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetDutyCycleOutputScaleFactor(HAL_DutyCycleHandle dutyCycleHandle,
                                          int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetDutyCycleFPGAIndex(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}
}  // extern "C"
