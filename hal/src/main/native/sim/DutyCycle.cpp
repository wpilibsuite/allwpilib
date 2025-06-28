// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DutyCycle.h"

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "SmartIo.h"
#include "hal/Errors.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/LimitedHandleResource.h"
#include "mockdata/DutyCycleDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeDutyCycle() {}
}  // namespace hal::init

extern "C" {
HAL_DutyCycleHandle HAL_InitializeDutyCycle(int32_t channel,
                                            const char* allocationLocation,
                                            int32_t* status) {
  hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DutyCycle", 0,
                                     kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port = smartIoHandles->Allocate(channel, HAL_HandleEnum::DutyCycle,
                                       &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "SmartIo", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DutyCycle", 0,
                                       kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = channel;
  port->previousAllocation = allocationLocation ? allocationLocation : "";

  SimDutyCycleData[channel].digitalChannel = channel;
  SimDutyCycleData[channel].initialized = true;
  SimDutyCycleData[channel].simDevice = 0;

  return handle;
}
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle) {
  auto port = smartIoHandles->Get(dutyCycleHandle, HAL_HandleEnum::DutyCycle);
  if (port == nullptr) {
    return;
  }

  smartIoHandles->Free(dutyCycleHandle, HAL_HandleEnum::DutyCycle);

  // Wait for no other object to hold this handle.
  auto start = hal::fpga_clock::now();
  while (port.use_count() != 1) {
    auto current = hal::fpga_clock::now();
    if (start + std::chrono::seconds(1) < current) {
      std::puts("DIO handle free timeout");
      std::fflush(stdout);
      break;
    }
    std::this_thread::yield();
  }
  SimDutyCycleData[port->channel].initialized = false;
}

void HAL_SetDutyCycleSimDevice(HAL_EncoderHandle handle,
                               HAL_SimDeviceHandle device) {
  auto port = smartIoHandles->Get(handle, HAL_HandleEnum::DutyCycle);
  if (port == nullptr) {
    return;
  }
  SimDutyCycleData[port->channel].simDevice = device;
}

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
  auto port = smartIoHandles->Get(dutyCycleHandle, HAL_HandleEnum::DutyCycle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (SimDutyCycleData[port->channel].frequency == 0) {
    return 0;
  }

  double period = 1e9 / SimDutyCycleData[port->channel].frequency;  // ns
  return period * SimDutyCycleData[port->channel].output;
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
