// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/PWM.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <thread>

#include <fmt/format.h>
#include <wpi/print.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "SmartIo.h"
#include "hal/Errors.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

namespace hal::init {
void InitializePWM() {}
}  // namespace hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializePWMPort(int32_t channel,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for PWM", 0,
                                     kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port =
      smartIoHandles->Allocate(channel, HAL_HandleEnum::PWM, &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "SmartIo", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for PWM", 0,
                                       kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = channel;

  *status = port->InitializeMode(SmartIoMode::PwmOutput);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::PWM);
    return HAL_kInvalidHandle;
  }

  // Disable the PWM output.
  HAL_SetPWMPulseTimeMicroseconds(handle, 0, status);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::PWM);
    return HAL_kInvalidHandle;
  }

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

void HAL_FreePWMPort(HAL_DigitalHandle pwmPortHandle) {
  auto port = smartIoHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    return;
  }

  smartIoHandles->Free(pwmPortHandle, HAL_HandleEnum::PWM);

  // Wait for no other object to hold this handle.
  auto start = hal::fpga_clock::now();
  while (port.use_count() != 1) {
    auto current = hal::fpga_clock::now();
    if (start + std::chrono::seconds(1) < current) {
      std::puts("PWM handle free timeout");
      std::fflush(stdout);
      break;
    }
    std::this_thread::yield();
  }
}

HAL_Bool HAL_CheckPWMChannel(int32_t channel) {
  return channel < kNumSmartIo && channel >= 0;
}

void HAL_SetPWMPulseTimeMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                     int32_t microsecondPulseTime,
                                     int32_t* status) {
  auto port = smartIoHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (microsecondPulseTime < 0 ||
      (microsecondPulseTime != 0xFFFF && microsecondPulseTime >= 4096)) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format("Pulse time {} out of range. Expect [0-4096) or 0xFFFF",
                    microsecondPulseTime));
    return;
  }

  *status = port->SetPwmMicroseconds(microsecondPulseTime);
}

int32_t HAL_GetPWMPulseTimeMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                        int32_t* status) {
  auto port = smartIoHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  uint16_t microseconds = 0;
  *status = port->GetPwmMicroseconds(&microseconds);
  return microseconds;
}

void HAL_SetPWMOutputPeriod(HAL_DigitalHandle pwmPortHandle, int32_t period,
                            int32_t* status) {
  auto port = smartIoHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  switch (period) {
    case 0:
      *status = port->SetPwmOutputPeriod(hal::PwmOutputPeriod::k5ms);
      break;
    case 1:
    case 2:
      *status = port->SetPwmOutputPeriod(hal::PwmOutputPeriod::k10ms);
      break;
    case 3:
      *status = port->SetPwmOutputPeriod(hal::PwmOutputPeriod::k20ms);
      break;
    default:
      *status = PARAMETER_OUT_OF_RANGE;
      return;
  }
}

}  // extern "C"
