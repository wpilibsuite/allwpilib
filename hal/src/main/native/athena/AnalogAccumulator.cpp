/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/AnalogAccumulator.h"

#include "AnalogInternal.h"
#include "hal/HAL.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAnalogAccumulator() {}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_Bool HAL_IsAccumulatorChannel(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  for (int32_t i = 0; i < kNumAccumulators; i++) {
    if (port->channel == kAccumulatorChannels[i]) return true;
  }
  return false;
}

void HAL_InitAccumulator(HAL_AnalogInputHandle analogPortHandle,
                         int32_t* status) {
  if (!HAL_IsAccumulatorChannel(analogPortHandle, status)) {
    *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
    return;
  }
  HAL_SetAccumulatorCenter(analogPortHandle, 0, status);
  HAL_ResetAccumulator(analogPortHandle, status);
}

void HAL_ResetAccumulator(HAL_AnalogInputHandle analogPortHandle,
                          int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (port->accumulator == nullptr) {
    *status = NULL_PARAMETER;
    return;
  }
  port->accumulator->strobeReset(status);
}

void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analogPortHandle,
                              int32_t center, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (port->accumulator == nullptr) {
    *status = NULL_PARAMETER;
    return;
  }
  port->accumulator->writeCenter(center, status);
}

void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analogPortHandle,
                                int32_t deadband, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (port->accumulator == nullptr) {
    *status = NULL_PARAMETER;
    return;
  }
  port->accumulator->writeDeadband(deadband, status);
}

int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  if (port->accumulator == nullptr) {
    *status = NULL_PARAMETER;
    return 0;
  }
  int64_t value = port->accumulator->readOutput_Value(status);
  return value;
}

int64_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  if (port->accumulator == nullptr) {
    *status = NULL_PARAMETER;
    return 0;
  }
  return port->accumulator->readOutput_Count(status);
}

void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analogPortHandle,
                              int64_t* value, int64_t* count, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (port->accumulator == nullptr) {
    *status = NULL_PARAMETER;
    return;
  }
  if (value == nullptr || count == nullptr) {
    *status = NULL_PARAMETER;
    return;
  }

  tAccumulator::tOutput output = port->accumulator->readOutput(status);

  *value = output.Value;
  *count = output.Count;
}

}  // extern "C"
