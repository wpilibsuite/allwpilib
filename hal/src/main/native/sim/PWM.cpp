// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/PWM.h"

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/PWMDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializePWM() {}
}  // namespace hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializePWMPort(HAL_PortHandle portHandle,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
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

  // Defaults to allow an always valid config.
  HAL_SetPWMConfigMicroseconds(handle, 2000, 1501, 1500, 1499, 1000, status);

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}
void HAL_FreePWMPort(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].initialized = false;

  digitalChannelHandles->Free(pwmPortHandle, HAL_HandleEnum::PWM);
}

HAL_Bool HAL_CheckPWMChannel(int32_t channel) {
  return channel < kNumPWMChannels && channel >= 0;
}

void HAL_SetPWMConfigMicroseconds(HAL_DigitalHandle pwmPortHandle, int32_t max,
                                  int32_t deadbandMax, int32_t center,
                                  int32_t deadbandMin, int32_t min,
                                  int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  port->maxPwm = max;
  port->deadbandMaxPwm = deadbandMax;
  port->deadbandMinPwm = deadbandMin;
  port->centerPwm = center;
  port->minPwm = min;
  port->configSet = true;
}

void HAL_GetPWMConfigMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                  int32_t* maxPwm, int32_t* deadbandMaxPwm,
                                  int32_t* centerPwm, int32_t* deadbandMinPwm,
                                  int32_t* minPwm, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  *maxPwm = port->maxPwm;
  *deadbandMaxPwm = port->deadbandMaxPwm;
  *deadbandMinPwm = port->deadbandMinPwm;
  *centerPwm = port->centerPwm;
  *minPwm = port->minPwm;
}

void HAL_SetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                 HAL_Bool eliminateDeadband, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  port->eliminateDeadband = eliminateDeadband;
}

HAL_Bool HAL_GetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                     int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return port->eliminateDeadband;
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

void HAL_SetPWMSpeed(HAL_DigitalHandle pwmPortHandle, double speed,
                     int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (!port->configSet) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  if (speed < -1.0) {
    speed = -1.0;
  } else if (speed > 1.0) {
    speed = 1.0;
  }

  SimPWMData[port->channel].speed = speed;
}

void HAL_SetPWMPosition(HAL_DigitalHandle pwmPortHandle, double pos,
                        int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (!port->configSet) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  if (pos < 0.0) {
    pos = 0.0;
  } else if (pos > 1.0) {
    pos = 1.0;
  }

  SimPWMData[port->channel].position = pos;
}

void HAL_SetPWMDisabled(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  SimPWMData[port->channel].pulseMicrosecond = 0;
  SimPWMData[port->channel].position = 0;
  SimPWMData[port->channel].speed = 0;
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

double HAL_GetPWMSpeed(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  if (!port->configSet) {
    *status = INCOMPATIBLE_STATE;
    return 0;
  }

  double speed = SimPWMData[port->channel].speed;
  if (speed > 1) {
    speed = 1;
  }
  if (speed < -1) {
    speed = -1;
  }
  return speed;
}

double HAL_GetPWMPosition(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  if (!port->configSet) {
    *status = INCOMPATIBLE_STATE;
    return 0;
  }

  double position = SimPWMData[port->channel].position;
  if (position > 1) {
    position = 1;
  }
  if (position < 0) {
    position = 0;
  }
  return position;
}

void HAL_LatchPWMZero(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].zeroLatch = true;
  SimPWMData[port->channel].zeroLatch = false;
}

void HAL_SetPWMAlwaysHighMode(HAL_DigitalHandle pwmPortHandle,
                              int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].pulseMicrosecond = 0xFFFF;
  SimPWMData[port->channel].position = 0xFFFF;
  SimPWMData[port->channel].speed = 0xFFFF;
}

void HAL_SetPWMPeriodScale(HAL_DigitalHandle pwmPortHandle, int32_t squelchMask,
                           int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].periodScale = squelchMask;
}

int32_t HAL_GetPWMLoopTiming(int32_t* status) {
  return kExpectedLoopTiming;
}

uint64_t HAL_GetPWMCycleStartTime(int32_t* status) {
  return 0;
}
}  // extern "C"
