// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/DIO.h"

#include <cmath>
#include <cstdio>
#include <thread>

#include "HALInitializer.hpp"
#include "HALInternal.hpp"
#include "PortsInternal.hpp"
#include "SmartIo.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/cpp/fpga_clock.hpp"
#include "wpi/hal/handles/HandlesInternal.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeDIO() {}
}  // namespace wpi::hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializeDIOPort(int32_t channel, HAL_Bool input,
                                        const char* allocationLocation,
                                        int32_t* status) {
  wpi::hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    wpi::hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DIO", 0,
                                          kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port =
      smartIoHandles->Allocate(channel, HAL_HandleEnum::DIO, &handle, status);

  if (*status != 0) {
    if (port) {
      wpi::hal::SetLastErrorPreviouslyAllocated(status, "SmartIo", channel,
                                                port->previousAllocation);
    } else {
      wpi::hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DIO", 0,
                                            kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = channel;

  *status = port->InitializeMode(input ? SmartIoMode::DigitalInput
                                       : SmartIoMode::DigitalOutput);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::DIO);
    return HAL_kInvalidHandle;
  }

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

HAL_Bool HAL_CheckDIOChannel(int32_t channel) {
  return channel < kNumSmartIo && channel >= 0;
}

void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    return;
  }

  smartIoHandles->Free(dioPortHandle, HAL_HandleEnum::DIO);

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

void HAL_SetDIOSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device) {
}

HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator) {}

void HAL_SetDigitalPWMRate(double rate, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDigitalPWMPPS(HAL_DigitalPWMHandle pwmGenerator, double dutyCycle,
                          int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t channel, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = port->SetDigitalOutput(value);
}

void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = port->SwitchDioDirection(input);
}

HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  bool ret = false;
  *status = port->GetDigitalInput(&ret);
  return ret;
}

HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  switch (port->currentMode) {
    case SmartIoMode::DigitalInput:
      return true;
    case SmartIoMode::DigitalOutput:
      return false;
    default:
      *status = INCOMPATIBLE_STATE;
      return false;
  }
}

void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLength,
               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_PulseMultiple(uint32_t channelMask, double pulseLength,
                       int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_IsAnyPulsing(int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

}  // extern "C"
