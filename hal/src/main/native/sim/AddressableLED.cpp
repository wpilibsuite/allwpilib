// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AddressableLED.h"

#include <fmt/format.h>

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "mockdata/AddressableLEDDataInternal.h"

using namespace hal;

namespace {
struct AddressableLED {
  uint8_t index;
};
}  // namespace

static LimitedHandleResource<HAL_AddressableLEDHandle, AddressableLED,
                             kNumAddressableLEDs,
                             HAL_HandleEnum::AddressableLED>* ledHandles;

namespace hal::init {
void InitializeAddressableLED() {
  static LimitedHandleResource<HAL_AddressableLEDHandle, AddressableLED,
                               kNumAddressableLEDs,
                               HAL_HandleEnum::AddressableLED>
      dcH;
  ledHandles = &dcH;
}
}  // namespace hal::init

extern "C" {
HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    HAL_DigitalHandle outputPort, int32_t* status) {
  hal::init::CheckInit();

  auto digitalPort =
      hal::digitalChannelHandles->Get(outputPort, hal::HAL_HandleEnum::PWM);

  if (!digitalPort) {
    // If DIO was passed, channel error, else generic error
    if (getHandleType(outputPort) == hal::HAL_HandleEnum::DIO) {
      *status = HAL_LED_CHANNEL_ERROR;
    } else {
      *status = HAL_HANDLE_ERROR;
    }
    return HAL_kInvalidHandle;
  }

  if (digitalPort->channel >= kNumPWMHeaders) {
    *status = HAL_LED_CHANNEL_ERROR;
    return HAL_kInvalidHandle;
  }

  HAL_AddressableLEDHandle handle = ledHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto led = ledHandles->Get(handle);
  if (!led) {  // would only occur on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  int16_t index = getHandleIndex(handle);
  SimAddressableLEDData[index].outputPort = digitalPort->channel;
  SimAddressableLEDData[index].length = 1;
  SimAddressableLEDData[index].running = false;
  SimAddressableLEDData[index].initialized = true;
  led->index = index;
  return handle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {
  auto led = ledHandles->Get(handle);
  ledHandles->Free(handle);
  if (!led) {
    return;
  }
  SimAddressableLEDData[led->index].running = false;
  SimAddressableLEDData[led->index].initialized = false;
}

void HAL_SetAddressableLEDColorOrder(HAL_AddressableLEDHandle handle,
                                     HAL_AddressableLEDColorOrder colorOrder,
                                     int32_t* status) {}

void HAL_SetAddressableLEDOutputPort(HAL_AddressableLEDHandle handle,
                                     HAL_DigitalHandle outputPort,
                                     int32_t* status) {
  auto led = ledHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (auto port = digitalChannelHandles->Get(outputPort, HAL_HandleEnum::PWM)) {
    SimAddressableLEDData[led->index].outputPort = port->channel;
  } else {
    SimAddressableLEDData[led->index].outputPort = -1;
  }
}

void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status) {
  auto led = ledHandles->Get(handle);
  if (!led) {
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
  SimAddressableLEDData[led->index].length = length;
}

void HAL_WriteAddressableLEDData(HAL_AddressableLEDHandle handle,
                                 const struct HAL_AddressableLEDData* data,
                                 int32_t length, int32_t* status) {
  auto led = ledHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (length > SimAddressableLEDData[led->index].length) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format(
            "Data length must be less than or equal to {}. {} was requested",
            SimAddressableLEDData[led->index].length.Get(), length));
    return;
  }
  SimAddressableLEDData[led->index].SetData(data, length);
}

void HAL_SetAddressableLEDBitTiming(HAL_AddressableLEDHandle handle,
                                    int32_t highTime0NanoSeconds,
                                    int32_t lowTime0NanoSeconds,
                                    int32_t highTime1NanoSeconds,
                                    int32_t lowTime1NanoSeconds,
                                    int32_t* status) {}

void HAL_SetAddressableLEDSyncTime(HAL_AddressableLEDHandle handle,
                                   int32_t syncTimeMicroSeconds,
                                   int32_t* status) {}

void HAL_StartAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                   int32_t* status) {
  auto led = ledHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  SimAddressableLEDData[led->index].running = true;
}

void HAL_StopAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                  int32_t* status) {
  auto led = ledHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  SimAddressableLEDData[led->index].running = false;
}
}  // extern "C"
