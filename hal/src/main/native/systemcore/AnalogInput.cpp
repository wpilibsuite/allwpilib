// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/AnalogInput.h"

#include <cstdio>
#include <thread>

#include "HALInitializer.hpp"
#include "PortsInternal.hpp"
#include "SmartIo.hpp"
#include "wpi/hal/ErrorHandling.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/hal/monotonic_clock.hpp"

namespace wpi::hal::init {
void InitializeAnalogInput() {}
}  // namespace wpi::hal::init

using namespace wpi::hal;

extern "C" {

HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    int32_t channel, const char* allocationLocation, int32_t* status) {
  wpi::hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = MakeErrorIndexOutOfRange(RESOURCE_OUT_OF_RANGE,
                                       "Invalid Index for Analog", 0,
                                       kNumSmartIo, channel);
    return HAL_INVALID_HANDLE;
  }

  auto resource =
      smartIoHandles->Allocate(channel, HAL_HandleEnum::ANALOG_INPUT, "Analog");

  if (!resource) {
    *status = resource.error();
    return HAL_INVALID_HANDLE;  // failed to allocate. Pass error back.
  }

  auto [handle, port] = *resource;
  port->channel = channel;

  *status = port->InitializeMode(SmartIoMode::AnalogInput);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::ANALOG_INPUT);
    return HAL_INVALID_HANDLE;
  }

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle) {
  auto port =
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::ANALOG_INPUT);
  if (port == nullptr) {
    return;
  }

  smartIoHandles->Free(analogPortHandle, HAL_HandleEnum::ANALOG_INPUT);

  // Wait for no other object to hold this handle.
  auto start = wpi::hal::monotonic_clock::now();
  while (port.use_count() != 1) {
    auto current = wpi::hal::monotonic_clock::now();
    if (start + std::chrono::seconds(1) < current) {
      std::puts("DIO handle free timeout");
      std::fflush(stdout);
      break;
    }
    std::this_thread::yield();
  }
}

HAL_Bool HAL_CheckAnalogModule(int32_t module) {
  return module == 1;
}

HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel) {
  return channel < kNumSmartIo && channel >= 0;
}

void HAL_SetAnalogInputSimDevice(HAL_AnalogInputHandle handle,
                                 HAL_SimDeviceHandle device) {}

int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status) {
  auto port =
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::ANALOG_INPUT);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  uint16_t ret = 0;
  *status = port->GetAnalogInput(&ret);
  return ret;
}

int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status) {
  return static_cast<int32_t>(voltage * 4095.0 / 3.3);
}

double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  auto port =
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::ANALOG_INPUT);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  uint16_t ret = 0;
  *status = port->GetAnalogInput(&ret);
  // Returns millivolts
  return ret / 1000.0;
}

double HAL_GetAnalogValueToVolts(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t rawValue, int32_t* status) {
  return rawValue / 4095.0 * 3.3;
}

}  // extern "C"
