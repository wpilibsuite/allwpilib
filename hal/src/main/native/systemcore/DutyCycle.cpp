// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DutyCycle.h"

#include <cstdio>
#include <memory>
#include <thread>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "SmartIo.h"
#include "hal/Errors.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeDutyCycle() {}
}  // namespace hal::init

extern "C" {
HAL_DutyCycleHandle HAL_InitializeDutyCycle(int32_t channel,
                                            const char* allocationLocation,
                                            int32_t* status) {
  hal::init::CheckInit();

  if (channel < 0 || channel >= NUM_SMART_IO) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DutyCycle", 0,
                                     NUM_SMART_IO, channel);
    return HAL_InvalidHandle;
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
                                       NUM_SMART_IO, channel);
    }
    return HAL_InvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = channel;

  *status = port->InitializeMode(SmartIoMode::PwmInput);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::DutyCycle);
    return HAL_InvalidHandle;
  }

  port->previousAllocation = allocationLocation ? allocationLocation : "";

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
}

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
  auto port = smartIoHandles->Get(dutyCycleHandle, HAL_HandleEnum::DutyCycle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  uint16_t ret = false;
  *status = port->GetPwmInputMicroseconds(&ret);
  return ret;
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
