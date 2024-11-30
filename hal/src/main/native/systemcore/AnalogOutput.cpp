// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogOutput.h"

#include <string>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/IndexedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogOutput() {}
}  // namespace hal::init

extern "C" {

HAL_AnalogOutputHandle HAL_InitializeAnalogOutputPort(
    HAL_PortHandle portHandle, const char* allocationLocation,
    int32_t* status) {
  hal::init::CheckInit();

  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

HAL_Bool HAL_CheckAnalogOutputChannel(int32_t channel) {
  return channel < kNumAnalogOutputs && channel >= 0;
}

void HAL_FreeAnalogOutputPort(HAL_AnalogOutputHandle analogOutputHandle) {}

void HAL_SetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                         double voltage, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

double HAL_GetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                           int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

}  // extern "C"
