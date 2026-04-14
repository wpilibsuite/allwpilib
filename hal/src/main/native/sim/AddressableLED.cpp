// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/AddressableLED.h"

#include <fmt/format.h>

#include "DigitalInternal.hpp"
#include "HALInitializer.hpp"
#include "HALInternal.hpp"
#include "PortsInternal.hpp"
#include "mockdata/AddressableLEDDataInternal.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/HandlesInternal.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeAddressableLED() {}
}  // namespace wpi::hal::init

extern "C" {
HAL_Status HAL_InitializeAddressableLED(int32_t channel,
                                        const char* allocationLocation,
                                        HAL_AddressableLEDHandle* handle) {
  wpi::hal::init::CheckInit();

  if (channel < 0 || channel >= kNumAddressableLEDs) {
    *handle = HAL_INVALID_HANDLE;
    wpi::hal::SetLastErrorIndexOutOfRange(RESOURCE_OUT_OF_RANGE,
                                          "Invalid Index for AddressableLED", 0,
                                          kNumAddressableLEDs, channel);
    return HAL_USE_LAST_ERROR;
  }

  int32_t status = 0;
  auto port = digitalChannelHandles->Allocate(
      channel, HAL_HandleEnum::ADDRESSABLE_LED, handle, &status);

  if (status != 0) {
    if (port) {
      wpi::hal::SetLastErrorPreviouslyAllocated(status, "PWM or DIO", channel,
                                                port->previousAllocation);
    } else {
      wpi::hal::SetLastErrorIndexOutOfRange(status,
                                            "Invalid Index for AddressableLED",
                                            0, kNumAddressableLEDs, channel);
    }
    *handle = HAL_INVALID_HANDLE;
    return HAL_USE_LAST_ERROR;
  }

  port->channel = static_cast<uint8_t>(channel);

  SimAddressableLEDData[channel].start = 0;
  SimAddressableLEDData[channel].length = 0;
  SimAddressableLEDData[channel].initialized = true;
  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return HAL_SUCCESS;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {
  auto port =
      digitalChannelHandles->Get(handle, HAL_HandleEnum::ADDRESSABLE_LED);
  // no status, so no need to check for a proper free.
  digitalChannelHandles->Free(handle, HAL_HandleEnum::ADDRESSABLE_LED);
  if (port == nullptr) {
    return;
  }
  SimAddressableLEDData[port->channel].initialized = false;
}

HAL_Status HAL_SetAddressableLEDStart(HAL_AddressableLEDHandle handle,
                                      int32_t start) {
  auto port =
      digitalChannelHandles->Get(handle, HAL_HandleEnum::ADDRESSABLE_LED);
  if (!port) {
    return HAL_HANDLE_ERROR;
  }
  if (start > HAL_ADDRESSABLE_LED_MAX_LEN || start < 0) {
    wpi::hal::SetLastError(
        PARAMETER_OUT_OF_RANGE,
        fmt::format(
            "LED start must be less than or equal to {}. {} was requested",
            HAL_ADDRESSABLE_LED_MAX_LEN, start));
    return HAL_USE_LAST_ERROR;
  }
  SimAddressableLEDData[port->channel].start = start;
  return HAL_SUCCESS;
}

HAL_Status HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                       int32_t length) {
  auto port =
      digitalChannelHandles->Get(handle, HAL_HandleEnum::ADDRESSABLE_LED);
  if (!port) {
    return HAL_HANDLE_ERROR;
  }
  if (length > HAL_ADDRESSABLE_LED_MAX_LEN || length < 0) {
    wpi::hal::SetLastError(
        PARAMETER_OUT_OF_RANGE,
        fmt::format(
            "LED length must be less than or equal to {}. {} was requested",
            HAL_ADDRESSABLE_LED_MAX_LEN, length));
    return HAL_USE_LAST_ERROR;
  }
  SimAddressableLEDData[port->channel].length = length;
  return HAL_SUCCESS;
}

HAL_Status HAL_SetAddressableLEDData(
    int32_t start, int32_t length, HAL_AddressableLEDColorOrder colorOrder,
    const struct HAL_AddressableLEDData* data) {
  SimAddressableLEDDataBuffer->SetData(start, length, data);
  return HAL_SUCCESS;
}
}  // extern "C"
