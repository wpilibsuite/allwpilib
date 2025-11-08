// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/AnalogInput.h"

#include "AnalogInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "mockdata/AnalogInDataInternal.h"
#include "wpi/hal/handles/HandlesInternal.h"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeAnalogInput() {}
}  // namespace wpi::hal::init

extern "C" {
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    int32_t channel, const char* allocationLocation, int32_t* status) {
  wpi::hal::init::CheckInit();
  if (channel < 0 || channel >= kNumAnalogInputs) {
    *status = RESOURCE_OUT_OF_RANGE;
    wpi::hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog Input",
                                     0, kNumAnalogInputs, channel);
    return HAL_kInvalidHandle;
  }

  HAL_AnalogInputHandle handle;
  auto analog_port = analogInputHandles->Allocate(channel, &handle, status);

  if (*status != 0) {
    if (analog_port) {
      wpi::hal::SetLastErrorPreviouslyAllocated(status, "Analog Input", channel,
                                           analog_port->previousAllocation);
    } else {
      wpi::hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog Input",
                                       0, kNumAnalogInputs, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  analog_port->channel = static_cast<uint8_t>(channel);
  analog_port->isAccumulator = false;

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

void HAL_SetAnalogSampleRate(double samplesPerSecond, int32_t* status) {
  // No op
}
double HAL_GetAnalogSampleRate(int32_t* status) {
  return kDefaultSampleRate;
}
void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                              int32_t bits, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogInData[port->channel].averageBits = bits;
}
int32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogInData[port->channel].averageBits;
}
void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t bits, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogInData[port->channel].oversampleBits = bits;
}
int32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                    int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogInData[port->channel].oversampleBits;
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
int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  // No averaging supported
  return HAL_GetAnalogValue(analogPortHandle, status);
}
int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status) {
  if (voltage > 5.0) {
    voltage = 5.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  if (voltage < 0.0) {
    voltage = 0.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  int32_t value =
      static_cast<int32_t>((voltage + offset * 1.0e-9) / (LSBWeight * 1.0e-9));
  return value;
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
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  double voltage = LSBWeight * 1.0e-9 * rawValue - offset * 1.0e-9;
  return voltage;
}

double HAL_GetAnalogAverageVoltage(HAL_AnalogInputHandle analogPortHandle,
                                   int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  // No averaging supported
  return SimAnalogInData[port->channel].voltage;
}
int32_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analogPortHandle,
                               int32_t* status) {
  return 1220703;
}
int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  return 0;
}
}  // extern "C"
