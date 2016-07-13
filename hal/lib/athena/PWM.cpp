/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/PWM.h"

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "PortsInternal.h"
#include "handles/HandlesInternal.h"

using namespace hal;

static inline int32_t GetMaxPositivePwm(DigitalPort* port) {
  return port->maxPwm;
}
static inline int32_t GetMinPositivePwm(DigitalPort* port) {
  return port->eliminateDeadband ? port->deadbandMaxPwm : port->centerPwm + 1;
}
static inline int32_t GetCenterPwm(DigitalPort* port) {
  return port->centerPwm;
}
static inline int32_t GetMaxNegativePwm(DigitalPort* port) {
  return port->eliminateDeadband ? port->deadbandMinPwm : port->centerPwm - 1;
}
static inline int32_t GetMinNegativePwm(DigitalPort* port) {
  return port->minPwm;
}
static inline int32_t GetPositiveScaleFactor(DigitalPort* port) {
  return GetMaxPositivePwm(port) - GetMinPositivePwm(port);
}  ///< The scale for positive speeds.
static inline int32_t GetNegativeScaleFactor(DigitalPort* port) {
  return GetMaxNegativePwm(port) - GetMinNegativePwm(port);
}  ///< The scale for negative speeds.
static inline int32_t GetFullRangeScaleFactor(DigitalPort* port) {
  return GetMaxPositivePwm(port) - GetMinNegativePwm(port);
}  ///< The scale for positions.

extern "C" {

HAL_DigitalHandle HAL_InitializePWMPort(HAL_PortHandle port_handle,
                                        int32_t* status) {
  initializeDigital(status);

  if (*status != 0) return HAL_kInvalidHandle;

  int16_t pin = getPortHandlePin(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  uint8_t origPin = static_cast<uint8_t>(pin);

  if (origPin < kNumPWMHeaders) {
    pin += kNumDigitalPins;  // remap Headers to end of allocations
  } else {
    pin = remapMXPPWMChannel(pin) + 10;  // remap MXP to proper channel
  }

  auto handle = digitalPinHandles.Allocate(pin, HAL_HandleEnum::PWM, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  auto port = digitalPinHandles.Get(handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  port->pin = origPin;

  int32_t bitToSet = 1 << remapMXPPWMChannel(port->pin);
  uint16_t specialFunctions =
      digitalSystem->readEnableMXPSpecialFunction(status);
  digitalSystem->writeEnableMXPSpecialFunction(specialFunctions | bitToSet,
                                               status);

  return handle;
}
void HAL_FreePWMPort(HAL_DigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (port->pin > tPWM::kNumHdrRegisters - 1) {
    int32_t bitToUnset = 1 << remapMXPPWMChannel(port->pin);
    uint16_t specialFunctions =
        digitalSystem->readEnableMXPSpecialFunction(status);
    digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToUnset,
                                                 status);
  }

  digitalPinHandles.Free(pwm_port_handle, HAL_HandleEnum::PWM);
}

HAL_Bool HAL_CheckPWMChannel(int32_t pin) {
  return (pin < kNumPWMPins) && (pin >= 0);
}

void HAL_SetPWMConfig(HAL_DigitalHandle pwm_port_handle, double max,
                      double deadbandMax, double center, double deadbandMin,
                      double min, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  // calculate the loop time in milliseconds
  double loopTime =
      HAL_GetLoopTiming(status) / (kSystemClockTicksPerMicrosecond * 1e3);
  if (*status != 0) return;

  int32_t maxPwm = (int32_t)((max - kDefaultPwmCenter) / loopTime +
                             kDefaultPwmStepsDown - 1);
  int32_t deadbandMaxPwm = (int32_t)(
      (deadbandMax - kDefaultPwmCenter) / loopTime + kDefaultPwmStepsDown - 1);
  int32_t centerPwm = (int32_t)((center - kDefaultPwmCenter) / loopTime +
                                kDefaultPwmStepsDown - 1);
  int32_t deadbandMinPwm = (int32_t)(
      (deadbandMin - kDefaultPwmCenter) / loopTime + kDefaultPwmStepsDown - 1);
  int32_t minPwm = (int32_t)((min - kDefaultPwmCenter) / loopTime +
                             kDefaultPwmStepsDown - 1);

  port->maxPwm = maxPwm;
  port->deadbandMaxPwm = deadbandMaxPwm;
  port->deadbandMinPwm = deadbandMinPwm;
  port->centerPwm = centerPwm;
  port->minPwm = minPwm;
  port->configSet = true;
}

void HAL_SetPWMConfigRaw(HAL_DigitalHandle pwm_port_handle, int32_t maxPwm,
                         int32_t deadbandMaxPwm, int32_t centerPwm,
                         int32_t deadbandMinPwm, int32_t minPwm,
                         int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
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

void HAL_GetPWMConfigRaw(HAL_DigitalHandle pwm_port_handle, int32_t* maxPwm,
                         int32_t* deadbandMaxPwm, int32_t* centerPwm,
                         int32_t* deadbandMinPwm, int32_t* minPwm,
                         int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
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

void HAL_SetPWMEliminateDeadband(HAL_DigitalHandle pwm_port_handle,
                                 HAL_Bool eliminateDeadband, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  port->eliminateDeadband = eliminateDeadband;
}

HAL_Bool HAL_GetPWMEliminateDeadband(HAL_DigitalHandle pwm_port_handle,
                                     int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return port->eliminateDeadband;
}

/**
 * Set a PWM channel to the desired value. The values range from 0 to 255 and
 * the period is controlled
 * by the PWM Period and MinHigh registers.
 *
 * @param channel The PWM channel to set.
 * @param value The PWM value to set.
 */
void HAL_SetPWMRaw(HAL_DigitalHandle pwm_port_handle, int32_t value,
                   int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (port->pin < tPWM::kNumHdrRegisters) {
    pwmSystem->writeHdr(port->pin, value, status);
  } else {
    pwmSystem->writeMXP(port->pin - tPWM::kNumHdrRegisters, value, status);
  }
}

/**
 * Set a PWM channel to the desired scaled value. The values range from -1 to 1
 * and
 * the period is controlled
 * by the PWM Period and MinHigh registers.
 *
 * @param channel The PWM channel to set.
 * @param value The scaled PWM value to set.
 */
void HAL_SetPWMSpeed(HAL_DigitalHandle pwm_port_handle, double speed,
                     int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (!port->configSet) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  DigitalPort* dPort = port.get();

  if (speed < -1.0) {
    speed = -1.0;
  } else if (speed > 1.0) {
    speed = 1.0;
  }

  // calculate the desired output pwm value by scaling the speed appropriately
  int32_t rawValue;
  if (speed == 0.0) {
    rawValue = GetCenterPwm(dPort);
  } else if (speed > 0.0) {
    rawValue = static_cast<int32_t>(
        speed * static_cast<double>(GetPositiveScaleFactor(dPort)) +
        static_cast<double>(GetMinPositivePwm(dPort)) + 0.5);
  } else {
    rawValue = static_cast<int32_t>(
        speed * static_cast<double>(GetNegativeScaleFactor(dPort)) +
        static_cast<double>(GetMaxNegativePwm(dPort)) + 0.5);
  }

  if (!((rawValue >= GetMinNegativePwm(dPort)) &&
        (rawValue <= GetMaxPositivePwm(dPort))) ||
      rawValue == kPwmDisabled) {
    *status = HAL_PWM_SCALE_ERROR;
    return;
  }

  HAL_SetPWMRaw(pwm_port_handle, rawValue, status);
}

/**
 * Set a PWM channel to the desired position value. The values range from 0 to 1
 * and
 * the period is controlled
 * by the PWM Period and MinHigh registers.
 *
 * @param channel The PWM channel to set.
 * @param value The scaled PWM value to set.
 */
void HAL_SetPWMPosition(HAL_DigitalHandle pwm_port_handle, double pos,
                        int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
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
  int32_t rawValue = static_cast<int32_t>(
      (pos * static_cast<double>(GetFullRangeScaleFactor(dPort))) +
      GetMinNegativePwm(dPort));

  if (rawValue == kPwmDisabled) {
    *status = HAL_PWM_SCALE_ERROR;
    return;
  }

  HAL_SetPWMRaw(pwm_port_handle, rawValue, status);
}

void HAL_SetPWMDisabled(HAL_DigitalHandle pwm_port_handle, int32_t* status) {
  HAL_SetPWMRaw(pwm_port_handle, kPwmDisabled, status);
}

/**
 * Get a value from a PWM channel. The values range from 0 to 255.
 *
 * @param channel The PWM channel to read from.
 * @return The raw PWM value.
 */
int32_t HAL_GetPWMRaw(HAL_DigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  if (port->pin < tPWM::kNumHdrRegisters) {
    return pwmSystem->readHdr(port->pin, status);
  } else {
    return pwmSystem->readMXP(port->pin - tPWM::kNumHdrRegisters, status);
  }
}

/**
 * Get a scaled value from a PWM channel. The values range from -1 to 1.
 *
 * @param channel The PWM channel to read from.
 * @return The scaled PWM value.
 */
double HAL_GetPWMSpeed(HAL_DigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  if (!port->configSet) {
    *status = INCOMPATIBLE_STATE;
    return 0;
  }

  int32_t value = HAL_GetPWMRaw(pwm_port_handle, status);
  if (*status != 0) return 0;
  DigitalPort* dPort = port.get();

  if (value == kPwmDisabled) {
    return 0.0;
  } else if (value > GetMaxPositivePwm(dPort)) {
    return 1.0;
  } else if (value < GetMinNegativePwm(dPort)) {
    return -1.0;
  } else if (value > GetMinPositivePwm(dPort)) {
    return static_cast<double>(value - GetMinPositivePwm(dPort)) /
           static_cast<double>(GetPositiveScaleFactor(dPort));
  } else if (value < GetMaxNegativePwm(dPort)) {
    return static_cast<double>(value - GetMaxNegativePwm(dPort)) /
           static_cast<double>(GetNegativeScaleFactor(dPort));
  } else {
    return 0.0;
  }
}

/**
 * Get a position value from a PWM channel. The values range from 0 to 1.
 *
 * @param channel The PWM channel to read from.
 * @return The scaled PWM value.
 */
double HAL_GetPWMPosition(HAL_DigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  if (!port->configSet) {
    *status = INCOMPATIBLE_STATE;
    return 0;
  }

  int32_t value = HAL_GetPWMRaw(pwm_port_handle, status);
  if (*status != 0) return 0;
  DigitalPort* dPort = port.get();

  if (value < GetMinNegativePwm(dPort)) {
    return 0.0;
  } else if (value > GetMaxPositivePwm(dPort)) {
    return 1.0;
  } else {
    return static_cast<double>(value - GetMinNegativePwm(dPort)) /
           static_cast<double>(GetFullRangeScaleFactor(dPort));
  }
}

void HAL_LatchPWMZero(HAL_DigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  pwmSystem->writeZeroLatch(port->pin, true, status);
  pwmSystem->writeZeroLatch(port->pin, false, status);
}

/**
 * Set how how often the PWM signal is squelched, thus scaling the period.
 *
 * @param channel The PWM channel to configure.
 * @param squelchMask The 2-bit mask of outputs to squelch.
 */
void HAL_SetPWMPeriodScale(HAL_DigitalHandle pwm_port_handle,
                           int32_t squelchMask, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HAL_HandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (port->pin < tPWM::kNumPeriodScaleHdrElements) {
    pwmSystem->writePeriodScaleHdr(port->pin, squelchMask, status);
  } else {
    pwmSystem->writePeriodScaleMXP(port->pin - tPWM::kNumPeriodScaleHdrElements,
                                   squelchMask, status);
  }
}

/**
 * Get the loop timing of the PWM system
 *
 * @return The loop time
 */
int32_t HAL_GetLoopTiming(int32_t* status) {
  return pwmSystem->readLoopTiming(status);
}
}
