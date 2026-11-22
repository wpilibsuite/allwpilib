// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/DIO.h"

#include "DigitalInternal.hpp"
#include "HALInitializer.hpp"
#include "PortsInternal.hpp"
#include "mockdata/DIODataInternal.hpp"
#include "wpi/hal/ErrorHandling.hpp"
#include "wpi/hal/Errors.h"
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

  if (channel < 0 || channel >= NUM_DIGITAL_CHANNELS) {
    *status = MakeErrorIndexOutOfRange(HAL_RESOURCE_OUT_OF_RANGE,
                                       "Invalid Index for DIO", 0,
                                       NUM_DIGITAL_CHANNELS - 1, channel);
    return HAL_INVALID_HANDLE;
  }

  auto resource =
      digitalChannelHandles->Allocate(channel, HAL_HandleEnum::DIO, "DIO");

  if (!resource) {
    *status = resource.error();
    return HAL_INVALID_HANDLE;  // failed to allocate. Pass error back.
  }

  auto [handle, port] = *resource;
  port->channel = static_cast<uint8_t>(channel);

  SimDIOData[channel].initialized = true;
  SimDIOData[channel].isInput = input;
  SimDIOData[channel].simDevice = 0;
  SimDIOData[channel].value = true;
  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

HAL_Bool HAL_CheckDIOChannel(int32_t channel) {
  return channel < NUM_DIGITAL_CHANNELS && channel >= 0;
}

void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  // no status, so no need to check for a proper free.
  digitalChannelHandles->Free(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    return;
  }
  SimDIOData[port->channel].initialized = false;
}

void HAL_SetDIOSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device) {
  auto port = digitalChannelHandles->Get(handle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    return;
  }
  SimDIOData[port->channel].simDevice = device;
}

void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
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
    *status = MakeError(HAL_PARAMETER_OUT_OF_RANGE,
                        "Cannot set output of an input channel");
    return;
  }
  SimDIOData[port->channel].value = value;
}

void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimDIOData[port->channel].isInput = input;
}

HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
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
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
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
}  // extern "C"
