// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogInput.h"

#include "SmartIo.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/cpp/fpga_clock.h"
#include "mockdata/AnalogInDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogInput() {}
}  // namespace hal::init

extern "C" {
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    int32_t channel, const char* allocationLocation, int32_t* status) {
  hal::init::CheckInit();
  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog Input",
                                     0, kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port = smartIoHandles->Allocate(channel, HAL_HandleEnum::AnalogInput,
                                       &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "Analog Input", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog Input",
                                       0, kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = static_cast<uint8_t>(channel);

  SimAnalogInData[channel].initialized = true;
  SimAnalogInData[channel].simDevice = 0;

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}
void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle) {
  auto port =
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::AnalogInput);
  if (port == nullptr) {
    return;
  }

  smartIoHandles->Free(analogPortHandle, HAL_HandleEnum::AnalogInput);

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
  SimAnalogInData[port->channel].initialized = false;
}

HAL_Bool HAL_CheckAnalogModule(int32_t module) {
  return module == 1;
}

HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel) {
  return channel < kNumSmartIo && channel >= 0;
}

void HAL_SetAnalogInputSimDevice(HAL_AnalogInputHandle handle,
                                 HAL_SimDeviceHandle device) {
  auto port = smartIoHandles->Get(handle, HAL_HandleEnum::AnalogInput);
  if (port == nullptr) {
    return;
  }
  SimAnalogInData[port->channel].simDevice = device;
}

void HAL_SetAnalogSampleRate(double samplesPerSecond, int32_t* status) {
  // No op
}
double HAL_GetAnalogSampleRate(int32_t* status) {
  return 0;
}
void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                              int32_t bits, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
int32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}
void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t bits, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
int32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}
int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status) {
  auto port =
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::AnalogInput);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  double voltage = SimAnalogInData[port->channel].voltage;
  return (voltage / 3.3) * 4095.0;
}
int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  // No averaging supported
  return HAL_GetAnalogValue(analogPortHandle, status);
}
int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status) {
                                    *status = HAL_HANDLE_ERROR;
                                    return 0;
}
double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  auto port =
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::AnalogInput);
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
  auto port =
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::AnalogInput);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  // No averaging supported
  return SimAnalogInData[port->channel].voltage;
}
int32_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analogPortHandle,
                               int32_t* status) {
  return 0;
}
int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  return 0;
}
}  // extern "C"
