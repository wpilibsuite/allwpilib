// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/PWM.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <thread>

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

static inline double GetMaxPositivePwm(DigitalPort* port) {
  return port->maxPwm;
}

static inline double GetMinPositivePwm(DigitalPort* port) {
  if (port->eliminateDeadband) {
    return port->deadbandMaxPwm;
  } else {
    return port->centerPwm + 0.001;
  }
}

static inline double GetCenterPwm(DigitalPort* port) {
  return port->centerPwm;
}

static inline double GetMaxNegativePwm(DigitalPort* port) {
  if (port->eliminateDeadband) {
    return port->deadbandMinPwm;
  } else {
    return port->centerPwm - 0.001;
  }
}

static inline double GetMinNegativePwm(DigitalPort* port) {
  return port->minPwm;
}

static inline double GetPositiveScaleFactor(DigitalPort* port) {
  return GetMaxPositivePwm(port) - GetMinPositivePwm(port);
}  ///< The scale for positive speeds.

static inline double GetNegativeScaleFactor(DigitalPort* port) {
  return GetMaxNegativePwm(port) - GetMinNegativePwm(port);
}  ///< The scale for negative speeds.

static inline double GetFullRangeScaleFactor(DigitalPort* port) {
  return GetMaxPositivePwm(port) - GetMinNegativePwm(port);
}  ///< The scale for positions.

namespace hal::init {
void InitializePWM() {}
}  // namespace hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializePWMPort(HAL_PortHandle portHandle,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();
  initializeDigital(status);

  if (*status != 0) {
    return HAL_kInvalidHandle;
  }

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex || channel >= kNumPWMChannels) {
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
      hal::SetLastErrorPreviouslyAllocated(status, "PWM or DIO", origChannel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for PWM", 0,
                                       kNumPWMChannels, origChannel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = origChannel;

  if (port->channel > tPWM::kNumHdrRegisters - 1) {
    int32_t bitToSet = 1 << remapMXPPWMChannel(port->channel);
    uint16_t specialFunctions =
        digitalSystem->readEnableMXPSpecialFunction(status);
    digitalSystem->writeEnableMXPSpecialFunction(specialFunctions | bitToSet,
                                                 status);
  }

  // Defaults to allow an always valid config.
  HAL_SetPWMConfig(handle, 2.0, 1.501, 1.5, 1.499, 1.0, status);

  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}
void HAL_FreePWMPort(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  digitalChannelHandles->Free(pwmPortHandle, HAL_HandleEnum::PWM);

  // Wait for no other object to hold this handle.
  auto start = hal::fpga_clock::now();
  while (port.use_count() != 1) {
    auto current = hal::fpga_clock::now();
    if (start + std::chrono::seconds(1) < current) {
      std::puts("PWM handle free timeout");
      std::fflush(stdout);
      break;
    }
    std::this_thread::yield();
  }

  if (port->channel > tPWM::kNumHdrRegisters - 1) {
    int32_t bitToUnset = 1 << remapMXPPWMChannel(port->channel);
    uint16_t specialFunctions =
        digitalSystem->readEnableMXPSpecialFunction(status);
    digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToUnset,
                                                 status);
  }
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

  port->maxPwm = max;
  port->deadbandMaxPwm = deadbandMax;
  port->deadbandMinPwm = deadbandMin;
  port->centerPwm = center;
  port->minPwm = min;
  port->configSet = true;
}

void HAL_GetPWMConfig(HAL_DigitalHandle pwmPortHandle, double* maxPwm,
                      double* deadbandMaxPwm, double* centerPwm,
                      double* deadbandMinPwm, double* minPwm, int32_t* status) {
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

void HAL_SetPWMPulseTime(HAL_DigitalHandle pwmPortHandle, double value,
                         int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (port->channel < tPWM::kNumHdrRegisters) {
    pwmSystem->writeHdr(port->channel, value * 1.0e3, status);
  } else {
    pwmSystem->writeMXP(port->channel - tPWM::kNumHdrRegisters, value * 1.0e3,
                        status);
  }
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

  DigitalPort* dPort = port.get();

  if (std::isfinite(speed)) {
    speed = std::clamp(speed, -1.0, 1.0);
  } else {
    speed = 0.0;
  }

  // calculate the desired output pwm value by scaling the speed appropriately
  double rawValue;
  if (speed == 0.0) {
    rawValue = GetCenterPwm(dPort);
  } else if (speed > 0.0) {
    rawValue = speed * GetPositiveScaleFactor(dPort) + GetMinPositivePwm(dPort);
  } else {
    rawValue = speed * GetNegativeScaleFactor(dPort) + GetMaxNegativePwm(dPort);
  }

  if (!((rawValue >= GetMinNegativePwm(dPort)) &&
        (rawValue <= GetMaxPositivePwm(dPort))) ||
      rawValue == kPwmDisabled) {
    *status = HAL_PWM_SCALE_ERROR;
    return;
  }

  HAL_SetPWMPulseTime(pwmPortHandle, rawValue, status);
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
  DigitalPort* dPort = port.get();

  if (pos < 0.0) {
    pos = 0.0;
  } else if (pos > 1.0) {
    pos = 1.0;
  }

  // note, need to perform the multiplication below as floating point before
  // converting to int
  double rawValue =
      (pos * GetFullRangeScaleFactor(dPort)) + GetMinNegativePwm(dPort);

  if (rawValue == kPwmDisabled) {
    *status = HAL_PWM_SCALE_ERROR;
    return;
  }

  HAL_SetPWMPulseTime(pwmPortHandle, rawValue, status);
}

void HAL_SetPWMDisabled(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  HAL_SetPWMPulseTime(pwmPortHandle, kPwmDisabled, status);
}

double HAL_GetPWMPulseTime(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (port->channel < tPWM::kNumHdrRegisters) {
    return pwmSystem->readHdr(port->channel, status) / 1.0e3;
  } else {
    return pwmSystem->readMXP(port->channel - tPWM::kNumHdrRegisters, status) /
           1.0e3;
  }
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

  double value = HAL_GetPWMPulseTime(pwmPortHandle, status);
  if (*status != 0) {
    return 0;
  }
  DigitalPort* dPort = port.get();

  if (value == kPwmDisabled) {
    return 0.0;
  } else if (value > GetMaxPositivePwm(dPort)) {
    return 1.0;
  } else if (value < GetMinNegativePwm(dPort)) {
    return -1.0;
  } else if (value > GetMinPositivePwm(dPort)) {
    return value - GetMinPositivePwm(dPort) / GetPositiveScaleFactor(dPort);
  } else if (value < GetMaxNegativePwm(dPort)) {
    return value - GetMaxNegativePwm(dPort) / GetNegativeScaleFactor(dPort);
  } else {
    return 0.0;
  }
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

  double value = HAL_GetPWMPulseTime(pwmPortHandle, status);
  if (*status != 0) {
    return 0;
  }
  DigitalPort* dPort = port.get();

  if (value < GetMinNegativePwm(dPort)) {
    return 0.0;
  } else if (value > GetMaxPositivePwm(dPort)) {
    return 1.0;
  } else {
    return value - GetMinNegativePwm(dPort) / GetFullRangeScaleFactor(dPort);
  }
}

void HAL_LatchPWMZero(HAL_DigitalHandle pwmPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  pwmSystem->writeZeroLatch(port->channel, true, status);
  pwmSystem->writeZeroLatch(port->channel, false, status);
}

void HAL_SetPWMPeriodScale(HAL_DigitalHandle pwmPortHandle, int32_t squelchMask,
                           int32_t* status) {
  auto port = digitalChannelHandles->Get(pwmPortHandle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (port->channel < tPWM::kNumPeriodScaleHdrElements) {
    pwmSystem->writePeriodScaleHdr(port->channel, squelchMask, status);
  } else {
    pwmSystem->writePeriodScaleMXP(
        port->channel - tPWM::kNumPeriodScaleHdrElements, squelchMask, status);
  }
}

void HAL_SetPWMAlwaysHighMode(HAL_DigitalHandle pwmPortHandle,
                              int32_t* status) {
  HAL_SetPWMPulseTime(pwmPortHandle, 0xFFFF, status);
}

int32_t HAL_GetPWMLoopTiming(int32_t* status) {
  initializeDigital(status);
  if (*status != 0) {
    return 0;
  }
  return pwmSystem->readLoopTiming(status);
}

uint64_t HAL_GetPWMCycleStartTime(int32_t* status) {
  initializeDigital(status);
  if (*status != 0) {
    return 0;
  }

  uint64_t upper1 = pwmSystem->readCycleStartTimeUpper(status);
  uint32_t lower = pwmSystem->readCycleStartTime(status);
  uint64_t upper2 = pwmSystem->readCycleStartTimeUpper(status);
  if (*status != 0) {
    return 0;
  }
  if (upper1 != upper2) {
    // Rolled over between the lower call, reread lower
    lower = pwmSystem->readCycleStartTime(status);
    if (*status != 0) {
      return 0;
    }
  }
  return (upper2 << 32) + lower;
}

}  // extern "C"
