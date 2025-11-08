// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/PWM.h"

#include <algorithm>
#include <cmath>

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "mockdata/PWMDataInternal.h"
#include "wpi/hal/handles/HandlesInternal.h"

using namespace hal;

namespace hal::init {
void InitializePWM() {}
}  // namespace hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializePWMPort(int32_t channel,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();

  if (channel < 0 || channel >= kNumPWMChannels) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for PWM", 0,
                                     kNumPWMChannels, channel);
    return HAL_kInvalidHandle;
  }

  uint8_t origChannel = static_cast<uint8_t>(channel);

  if (origChannel < kNumPWMHeaders) {
    channel += kNumDigitalChannels;  // remap Headers to end of allocations
  } else {
    channel = remapMXPPWMChannel(channel) + 10;  // remap MXP to proper channel
  }

  HAL_DigitalHandle handle;

  auto port = digitalChannelHandles->Allocate(channel, HAL_HandleEnum::PWM,
                                              &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "PWM or DIO", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for PWM", 0,
                                       kNumPWMChannels, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = origChannel;

  SimPWMData[origChannel].initialized = true;

  // Disable output.
  HAL_SetPWMPulseTimeMicroseconds(handle, 0, status);

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}
void HAL_FreePWMPort(HAL_DigitalHandle pwmPortHandle) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    return;
  }

  SimPWMData[port->channel].initialized = false;

  digitalChannelHandles->Free(pwmPortHandle, HAL_HandleEnum::PWM);
}

HAL_Bool HAL_CheckPWMChannel(int32_t channel) {
  return channel < kNumPWMChannels && channel >= 0;
}

void HAL_SetPWMSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device) {
  auto port = digitalChannelHandles->Get(handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    return;
  }
  SimPWMData[port->channel].simDevice = device;
}

void HAL_SetPWMPulseTimeMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                     int32_t value, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].pulseMicrosecond = value;
}

int32_t HAL_GetPWMPulseTimeMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                        int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimPWMData[port->channel].pulseMicrosecond;
}

void HAL_SetPWMOutputPeriod(HAL_DigitalHandle pwmPortHandle, int32_t period,
                            int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].outputPeriod = period;
}

}  // extern "C"
