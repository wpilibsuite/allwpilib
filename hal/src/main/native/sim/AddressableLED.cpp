// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/AddressableLED.h"

#include <fmt/format.h>

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/HandlesInternal.h"
#include "wpi/hal/handles/IndexedHandleResource.h"
#include "mockdata/AddressableLEDDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAddressableLED() {}
}  // namespace hal::init

extern "C" {
HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    int32_t channel, const char* allocationLocation, int32_t* status) {
  hal::init::CheckInit();

  if (channel < 0 || channel >= kNumAddressableLEDs) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for AddressableLED",
                                     0, kNumAddressableLEDs, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port = digitalChannelHandles->Allocate(
      channel, HAL_HandleEnum::AddressableLED, &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "PWM or DIO", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status,
                                       "Invalid Index for AddressableLED", 0,
                                       kNumAddressableLEDs, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = static_cast<uint8_t>(channel);

  SimAddressableLEDData[channel].start = 0;
  SimAddressableLEDData[channel].length = 0;
  SimAddressableLEDData[channel].initialized = true;
  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {
  auto port =
      digitalChannelHandles->Get(handle, HAL_HandleEnum::AddressableLED);
  // no status, so no need to check for a proper free.
  digitalChannelHandles->Free(handle, HAL_HandleEnum::AddressableLED);
  if (port == nullptr) {
    return;
  }
  SimAddressableLEDData[port->channel].initialized = false;
}

void HAL_SetAddressableLEDStart(HAL_AddressableLEDHandle handle, int32_t start,
                                int32_t* status) {
  auto port =
      digitalChannelHandles->Get(handle, HAL_HandleEnum::AddressableLED);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (start > HAL_kAddressableLEDMaxLength || start < 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format(
            "LED start must be less than or equal to {}. {} was requested",
            HAL_kAddressableLEDMaxLength, start));
    return;
  }
  SimAddressableLEDData[port->channel].start = start;
}

void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status) {
  auto port =
      digitalChannelHandles->Get(handle, HAL_HandleEnum::AddressableLED);
  if (!port) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (length > HAL_kAddressableLEDMaxLength || length < 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format(
            "LED length must be less than or equal to {}. {} was requested",
            HAL_kAddressableLEDMaxLength, length));
    return;
  }
  SimAddressableLEDData[port->channel].length = length;
}

void HAL_SetAddressableLEDData(int32_t start, int32_t length,
                               HAL_AddressableLEDColorOrder colorOrder,
                               const struct HAL_AddressableLEDData* data,
                               int32_t* status) {
  SimAddressableLEDDataBuffer->SetData(start, length, data);
}
}  // extern "C"
