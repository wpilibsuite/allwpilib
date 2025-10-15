// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/DutyCycle.h"

#include <cstdio>
#include <memory>
#include <thread>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "SmartIo.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/cpp/fpga_clock.h"
#include "wpi/hal/handles/HandlesInternal.h"
#include "wpi/hal/handles/LimitedHandleResource.h"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeDutyCycle() {}
}  // namespace wpi::hal::init

extern "C" {
HAL_DutyCycleHandle HAL_InitializeDutyCycle(int32_t channel,
                                            const char* allocationLocation,
                                            int32_t* status) {
  wpi::hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    wpi::hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DutyCycle", 0,
                                     kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port = smartIoHandles->Allocate(channel, HAL_HandleEnum::DutyCycle,
                                       &handle, status);

  if (*status != 0) {
    if (port) {
      wpi::hal::SetLastErrorPreviouslyAllocated(status, "SmartIo", channel,
                                           port->previousAllocation);
    } else {
      wpi::hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DutyCycle", 0,
                                       kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = channel;

  *status = port->InitializeMode(SmartIoMode::PwmInput);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::DutyCycle);
    return HAL_kInvalidHandle;
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
  auto start = wpi::hal::fpga_clock::now();
  while (port.use_count() != 1) {
    auto current = wpi::hal::fpga_clock::now();
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

double HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                 int32_t* status) {
  auto port = smartIoHandles->Get(dutyCycleHandle, HAL_HandleEnum::DutyCycle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  uint16_t ret = 0;
  *status = port->GetPwmInputPeriodMicroseconds(&ret);

  if (ret == 0) {
    return 0.0;
  }

  return 1000000.0 / ret;
}

double HAL_GetDutyCycleOutput(HAL_DutyCycleHandle dutyCycleHandle,
                              int32_t* status) {
  auto port = smartIoHandles->Get(dutyCycleHandle, HAL_HandleEnum::DutyCycle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  uint16_t highTime = 0;
  *status = port->GetPwmInputMicroseconds(&highTime);

  uint16_t period = 0;
  *status = port->GetPwmInputPeriodMicroseconds(&period);

  if (period == 0) {
    return 0.0;
  }

  if (highTime >= period) {
    return 1.0;
  }

  return static_cast<double>(highTime) / static_cast<double>(period);
}

int32_t HAL_GetDutyCycleHighTime(HAL_DutyCycleHandle dutyCycleHandle,
                                 int32_t* status) {
  auto port = smartIoHandles->Get(dutyCycleHandle, HAL_HandleEnum::DutyCycle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  uint16_t ret = 0;
  *status = port->GetPwmInputMicroseconds(&ret);
  return static_cast<int32_t>(ret) * 1000;
}
}  // extern "C"
