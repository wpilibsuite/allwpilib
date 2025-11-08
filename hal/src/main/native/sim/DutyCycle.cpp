// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/DutyCycle.h"

#include <string>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/HandlesInternal.h"
#include "wpi/hal/handles/IndexedHandleResource.h"
#include "mockdata/DutyCycleDataInternal.h"

using namespace hal;

namespace {
struct DutyCycle {
  uint8_t index;
  std::string previousAllocation;
};
struct Empty {};
}  // namespace

static IndexedHandleResource<HAL_DutyCycleHandle, DutyCycle, kNumDutyCycles,
                             HAL_HandleEnum::DutyCycle>* dutyCycleHandles;

namespace hal::init {
void InitializeDutyCycle() {
  static IndexedHandleResource<HAL_DutyCycleHandle, DutyCycle, kNumDutyCycles,
                               HAL_HandleEnum::DutyCycle>
      dcH;
  dutyCycleHandles = &dcH;
}
}  // namespace hal::init

extern "C" {
HAL_DutyCycleHandle HAL_InitializeDutyCycle(int32_t channel,
                                            const char* allocationLocation,
                                            int32_t* status) {
  hal::init::CheckInit();

  HAL_DutyCycleHandle handle = HAL_kInvalidHandle;
  auto dutyCycle = dutyCycleHandles->Allocate(channel, &handle, status);

  if (*status != 0) {
    if (dutyCycle) {
      hal::SetLastErrorPreviouslyAllocated(status, "SmartIo", channel,
                                           dutyCycle->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Duty Cycle",
                                       0, kNumDutyCycles, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  int16_t index = getHandleIndex(handle);
  SimDutyCycleData[index].initialized = true;
  SimDutyCycleData[index].simDevice = 0;
  dutyCycle->index = index;
  dutyCycle->previousAllocation = allocationLocation ? allocationLocation : "";
  return handle;
}
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  dutyCycleHandles->Free(dutyCycleHandle);
  if (dutyCycle == nullptr) {
    return;
  }
  SimDutyCycleData[dutyCycle->index].initialized = false;
}

void HAL_SetDutyCycleSimDevice(HAL_EncoderHandle handle,
                               HAL_SimDeviceHandle device) {
  auto dutyCycle = dutyCycleHandles->Get(handle);
  if (dutyCycle == nullptr) {
    return;
  }
  SimDutyCycleData[dutyCycle->index].simDevice = device;
}

double HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                 int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (dutyCycle == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return SimDutyCycleData[dutyCycle->index].frequency;
}

double HAL_GetDutyCycleOutput(HAL_DutyCycleHandle dutyCycleHandle,
                              int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (dutyCycle == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return SimDutyCycleData[dutyCycle->index].output;
}

int32_t HAL_GetDutyCycleHighTime(HAL_DutyCycleHandle dutyCycleHandle,
                                 int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (dutyCycle == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (SimDutyCycleData[dutyCycle->index].frequency == 0) {
    return 0;
  }

  double period = 1e9 / SimDutyCycleData[dutyCycle->index].frequency;  // ns
  return period * SimDutyCycleData[dutyCycle->index].output;
}
}  // extern "C"
