// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogInput.h"

#include <cstdio>
#include <thread>

#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "SmartIo.h"
#include "hal/Errors.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/HandlesInternal.h"

namespace hal::init {
void InitializeAnalogInput() {}
}  // namespace hal::init

using namespace hal;

extern "C" {

HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    int32_t channel, const char* allocationLocation, int32_t* status) {
  hal::init::CheckInit();

  if (channel < 0 || channel >= kNumSmartIo) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog", 0,
                                     kNumSmartIo, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port = smartIoHandles->Allocate(channel, HAL_HandleEnum::AnalogInput,
                                       &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "SmartIo", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog", 0,
                                       kNumSmartIo, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = channel;

  *status = port->InitializeMode(SmartIoMode::AnalogInput);
  if (*status != 0) {
    smartIoHandles->Free(handle, HAL_HandleEnum::AnalogInput);
    return HAL_kInvalidHandle;
  }

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
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::AnalogInput);
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
      smartIoHandles->Get(analogPortHandle, HAL_HandleEnum::AnalogInput);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  uint16_t ret = 0;
  *status = port->GetAnalogInput(&ret);
  return ret / 4095.0 * 3.3;
}

double HAL_GetAnalogValueToVolts(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t rawValue, int32_t* status) {
  return rawValue / 4095.0 * 3.3;
}

}  // extern "C"
