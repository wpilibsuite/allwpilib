// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DIO.h"

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "mockdata/DIODataInternal.h"
#include "SmartIo.h"
#include "hal/cpp/fpga_clock.h"
#include "mockdata/DigitalPWMDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeDIO() {}
}  // namespace hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializeDIOPort(int32_t channel, HAL_Bool input,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DIO", 0,
                                     kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port =
      smartIoHandles->Allocate(channel, HAL_HandleEnum::DIO, &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "PWM or DIO", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DIO", 0,
                                       kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = static_cast<uint8_t>(channel);

  SimDIOData[channel].initialized = true;
  SimDIOData[channel].isInput = input;
  SimDIOData[channel].simDevice = 0;
  SimDIOData[channel].value = true;
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

  SimDIOData[port->channel].initialized = false;
}

void HAL_SetDIOSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device) {
  auto port = smartIoHandles->Get(handle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    return;
  }
  SimDIOData[port->channel].simDevice = device;
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
  if (value != 0 && value != 1) {
    if (value != 0) {
      value = 1;
    }
  }
  if (SimDIOData[port->channel].isInput) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(status, "Cannot set output of an input channel");
    return;
  }
  SimDIOData[port->channel].value = value;
}

void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimDIOData[port->channel].isInput = input;
}

HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  HAL_Bool value = SimDIOData[port->channel].value;
  if (value > 1) {
    value = 1;
  }
  if (value < 0) {
    value = 0;
  }
  return value;
}

HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = smartIoHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  HAL_Bool value = SimDIOData[port->channel].isInput;
  if (value > 1) {
    value = 1;
  }
  if (value < 0) {
    value = 0;
  }
  return value;
}

void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLengthSeconds,
               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_PulseMultiple(uint32_t channelMask, double pulseLengthSeconds,
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
