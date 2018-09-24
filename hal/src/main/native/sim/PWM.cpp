/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/PWM.h"

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/PWMDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializePWM() {}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_DigitalHandle HAL_InitializePWMPort(HAL_PortHandle portHandle,
                                        int32_t* status) {
  hal::init::CheckInit();
  if (*status != 0) return HAL_kInvalidHandle;

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  uint8_t origChannel = static_cast<uint8_t>(channel);

  if (origChannel < kNumPWMHeaders) {
    channel += kNumDigitalChannels;  // remap Headers to end of allocations
  } else {
    channel = remapMXPPWMChannel(channel) + 10;  // remap MXP to proper channel
  }

  auto handle =
      digitalChannelHandles->Allocate(channel, HAL_HandleEnum::PWM, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  auto port = digitalChannelHandles->Get(handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  port->channel = origChannel;

  SimPWMData[origChannel].initialized = true;

  // Defaults to allow an always valid config.
  HAL_SetPWMConfig(handle, 2.0, 1.501, 1.5, 1.499, 1.0, status);

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

void HAL_SetPWMConfig(HAL_DigitalHandle pwmPortHandle, double max,
                      double deadbandMax, double center, double deadbandMin,
                      double min, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  // calculate the loop time in milliseconds
  double loopTime =
      HAL_GetPWMLoopTiming(status) / (kSystemClockTicksPerMicrosecond * 1e3);
  if (*status != 0) return;

  int32_t maxPwm = static_cast<int32_t>((max - kDefaultPwmCenter) / loopTime +
                                        kDefaultPwmStepsDown - 1);
  int32_t deadbandMaxPwm = static_cast<int32_t>(
      (deadbandMax - kDefaultPwmCenter) / loopTime + kDefaultPwmStepsDown - 1);
  int32_t centerPwm = static_cast<int32_t>(
      (center - kDefaultPwmCenter) / loopTime + kDefaultPwmStepsDown - 1);
  int32_t deadbandMinPwm = static_cast<int32_t>(
      (deadbandMin - kDefaultPwmCenter) / loopTime + kDefaultPwmStepsDown - 1);
  int32_t minPwm = static_cast<int32_t>((min - kDefaultPwmCenter) / loopTime +
                                        kDefaultPwmStepsDown - 1);

  port->maxPwm = maxPwm;
  port->deadbandMaxPwm = deadbandMaxPwm;
  port->deadbandMinPwm = deadbandMinPwm;
  port->centerPwm = centerPwm;
  port->minPwm = minPwm;
  port->configSet = true;
}

void HAL_SetPWMConfigRaw(HAL_DigitalHandle pwmPortHandle, int32_t maxPwm,
                         int32_t deadbandMaxPwm, int32_t centerPwm,
                         int32_t deadbandMinPwm, int32_t minPwm,
                         int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  port->maxPwm = maxPwm;
  port->deadbandMaxPwm = deadbandMaxPwm;
  port->deadbandMinPwm = deadbandMinPwm;
  port->centerPwm = centerPwm;
  port->minPwm = minPwm;
}

void HAL_GetPWMConfigRaw(HAL_DigitalHandle pwmPortHandle, int32_t* maxPwm,
                         int32_t* deadbandMaxPwm, int32_t* centerPwm,
                         int32_t* deadbandMinPwm, int32_t* minPwm,
                         int32_t* status) {
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

void HAL_SetPWMRaw(HAL_DigitalHandle pwmPortHandle, int32_t value,
                   int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].rawValue = value;
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
  SimPWMData[port->channel].rawValue = 0;
  SimPWMData[port->channel].position = 0;
  SimPWMData[port->channel].speed = 0;
}

int32_t HAL_GetPWMRaw(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimPWMData[port->channel].rawValue;
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
  if (speed > 1) speed = 1;
  if (speed < -1) speed = -1;
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
  if (position > 1) position = 1;
  if (position < 0) position = 0;
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

void HAL_SetPWMPeriodScale(HAL_DigitalHandle pwmPortHandle, int32_t squelchMask,
                           int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPWMData[port->channel].periodScale = squelchMask;
}

int32_t HAL_GetPWMLoopTiming(int32_t* status) { return kExpectedLoopTiming; }

uint64_t HAL_GetPWMCycleStartTime(int32_t* status) { return 0; }
}  // extern "C"
