// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/AnalogInput.h"

#include "AnalogInternal.hpp"
#include "HALInitializer.hpp"
#include "PortsInternal.hpp"
#include "mockdata/AnalogInDataInternal.hpp"
#include "wpi/hal/ErrorHandling.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeAnalogInput() {}
}  // namespace wpi::hal::init

extern "C" {
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    int32_t channel, const char* allocationLocation, int32_t* status) {
  wpi::hal::init::CheckInit();
  if (channel < 0 || channel >= kNumAnalogInputs) {
    *status = MakeErrorIndexOutOfRange(HAL_RESOURCE_OUT_OF_RANGE,
                                       "Invalid Index for Analog Input", 0,
                                       kNumAnalogInputs, channel);
    return HAL_INVALID_HANDLE;
  }

  auto resource = analogInputHandles->Allocate(channel, "Analog");

  if (!resource) {
    *status = resource.error();
    return HAL_INVALID_HANDLE;  // failed to allocate. Pass error back.
  }

  auto [handle, analog_port] = *resource;
  analog_port->channel = static_cast<uint8_t>(channel);

  SimAnalogInData[channel].initialized = true;
  SimAnalogInData[channel].simDevice = 0;

  analog_port->previousAllocation =
      allocationLocation ? allocationLocation : "";

  return handle;
}
void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle) {
  auto port = analogInputHandles->Get(analogPortHandle);
  // no status, so no need to check for a proper free.
  analogInputHandles->Free(analogPortHandle);
  if (port == nullptr) {
    return;
  }
  SimAnalogInData[port->channel].initialized = false;
}

HAL_Bool HAL_CheckAnalogModule(int32_t module) {
  return module == 1;
}

HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel) {
  return channel < kNumAnalogInputs && channel >= 0;
}

void HAL_SetAnalogInputSimDevice(HAL_AnalogInputHandle handle,
                                 HAL_SimDeviceHandle device) {
  auto port = analogInputHandles->Get(handle);
  if (port == nullptr) {
    return;
  }
  SimAnalogInData[port->channel].simDevice = device;
}

int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  double voltage = SimAnalogInData[port->channel].voltage;
  return HAL_GetAnalogVoltsToValue(analogPortHandle, voltage, status);
}

int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status) {
  return static_cast<int32_t>(voltage * 4095.0 / 3.3);
}

double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  return SimAnalogInData[port->channel].voltage;
}

double HAL_GetAnalogValueToVolts(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t rawValue, int32_t* status) {
  return rawValue / 4095.0 * 3.3;
}
}  // extern "C"
