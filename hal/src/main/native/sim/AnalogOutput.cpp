/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/AnalogOutput.h"

#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/IndexedHandleResource.h"
#include "mockdata/AnalogOutDataInternal.h"

using namespace hal;

namespace {
struct AnalogOutput {
  uint8_t channel;
};
}  // namespace

static IndexedHandleResource<HAL_AnalogOutputHandle, AnalogOutput,
                             kNumAnalogOutputs, HAL_HandleEnum::AnalogOutput>*
    analogOutputHandles;

namespace hal {
namespace init {
void InitializeAnalogOutput() {
  static IndexedHandleResource<HAL_AnalogOutputHandle, AnalogOutput,
                               kNumAnalogOutputs, HAL_HandleEnum::AnalogOutput>
      aoH;
  analogOutputHandles = &aoH;
}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_AnalogOutputHandle HAL_InitializeAnalogOutputPort(HAL_PortHandle portHandle,
                                                      int32_t* status) {
  hal::init::CheckInit();
  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  HAL_AnalogOutputHandle handle =
      analogOutputHandles->Allocate(channel, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  auto port = analogOutputHandles->Get(handle);
  if (port == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  port->channel = static_cast<uint8_t>(channel);

  // Initialize sim analog input
  SimAnalogOutData[channel].initialized = true;
  return handle;
}

void HAL_FreeAnalogOutputPort(HAL_AnalogOutputHandle analogOutputHandle) {
  // no status, so no need to check for a proper free.
  auto port = analogOutputHandles->Get(analogOutputHandle);
  if (port == nullptr) return;
  analogOutputHandles->Free(analogOutputHandle);
  SimAnalogOutData[port->channel].initialized = false;
}

HAL_Bool HAL_CheckAnalogOutputChannel(int32_t channel) {
  return channel < kNumAnalogOutputs && channel >= 0;
}

void HAL_SetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                         double voltage, int32_t* status) {
  auto port = analogOutputHandles->Get(analogOutputHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogOutData[port->channel].voltage = voltage;
}

double HAL_GetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                           int32_t* status) {
  auto port = analogOutputHandles->Get(analogOutputHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  return SimAnalogOutData[port->channel].voltage;
}
}  // extern "C"
