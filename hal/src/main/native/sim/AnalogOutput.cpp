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
#include "mockdata/AnalogOutDataInternal.h"

using namespace hal;

namespace {
struct AnalogOutput {
  uint8_t channel;
  std::string previousAllocation;
};
}  // namespace

static IndexedHandleResource<HAL_AnalogOutputHandle, AnalogOutput,
                             kNumAnalogOutputs, HAL_HandleEnum::AnalogOutput>*
    analogOutputHandles;

namespace hal::init {
void InitializeAnalogOutput() {
  static IndexedHandleResource<HAL_AnalogOutputHandle, AnalogOutput,
                               kNumAnalogOutputs, HAL_HandleEnum::AnalogOutput>
      aoH;
  analogOutputHandles = &aoH;
}
}  // namespace hal::init

extern "C" {
HAL_AnalogOutputHandle HAL_InitializeAnalogOutputPort(
    HAL_PortHandle portHandle, const char* allocationLocation,
    int32_t* status) {
  hal::init::CheckInit();
  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex || channel >= kNumAnalogOutputs) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog Output",
                                     0, kNumAnalogOutputs, channel);
    return HAL_kInvalidHandle;
  }

  HAL_AnalogOutputHandle handle;
  auto port = analogOutputHandles->Allocate(channel, &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "Analog Output", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status,
                                       "Invalid Index for Analog Output", 0,
                                       kNumAnalogOutputs, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = static_cast<uint8_t>(channel);

  // Initialize sim analog input
  SimAnalogOutData[channel].initialized = true;

  port->previousAllocation = allocationLocation ? allocationLocation : "";
  return handle;
}

void HAL_FreeAnalogOutputPort(HAL_AnalogOutputHandle analogOutputHandle) {
  // no status, so no need to check for a proper free.
  auto port = analogOutputHandles->Get(analogOutputHandle);
  if (port == nullptr) {
    return;
  }
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
