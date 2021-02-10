// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogAccumulator.h"

#include "AnalogInternal.h"
#include "mockdata/AnalogInDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogAccumulator() {}
}  // namespace hal::init

extern "C" {
HAL_Bool HAL_IsAccumulatorChannel(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  for (int32_t i = 0; i < kNumAccumulators; i++) {
    if (port->channel == kAccumulatorChannels[i]) {
      return true;
    }
  }
  return false;
}
void HAL_InitAccumulator(HAL_AnalogInputHandle analogPortHandle,
                         int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (!HAL_IsAccumulatorChannel(analogPortHandle, status)) {
    *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
    return;
  }

  SimAnalogInData[port->channel].accumulatorInitialized = true;
}
void HAL_ResetAccumulator(HAL_AnalogInputHandle analogPortHandle,
                          int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogInData[port->channel].accumulatorCenter = 0;
  SimAnalogInData[port->channel].accumulatorCount = 0;
  SimAnalogInData[port->channel].accumulatorValue = 0;
}
void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analogPortHandle,
                              int32_t center, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogInData[port->channel].accumulatorCenter = center;
}
void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analogPortHandle,
                                int32_t deadband, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogInData[port->channel].accumulatorDeadband = deadband;
}
int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogInData[port->channel].accumulatorValue;
}
int64_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogInData[port->channel].accumulatorCount;
}
void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analogPortHandle,
                              int64_t* value, int64_t* count, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *count = SimAnalogInData[port->channel].accumulatorCount;
  *value = SimAnalogInData[port->channel].accumulatorValue;
}
}  // extern "C"
