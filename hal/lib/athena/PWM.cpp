/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/PWM.h"

#include "DigitalInternal.h"
#include "PortsInternal.h"
#include "handles/HandlesInternal.h"

using namespace hal;

extern "C" {

HalDigitalHandle initializePWMPort(HalPortHandle port_handle, int32_t* status) {
  initializeDigital(status);

  if (*status != 0) return HAL_INVALID_HANDLE;

  int16_t pin = getPortHandlePin(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  uint8_t origPin = static_cast<uint8_t>(pin);

  if (origPin < kNumPWMHeaders) {
    pin += kNumDigitalPins;  // remap Headers to end of allocations
  } else {
    pin = remapMXPPWMChannel(pin) + 10;  // remap MXP to proper channel
  }

  auto handle = digitalPinHandles.Allocate(pin, HalHandleEnum::PWM, status);

  if (*status != 0)
    return HAL_INVALID_HANDLE;  // failed to allocate. Pass error back.

  auto port = digitalPinHandles.Get(handle, HalHandleEnum::PWM);
  if (port == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_INVALID_HANDLE;
  }

  port->pin = origPin;

  uint32_t bitToSet = 1 << remapMXPPWMChannel(port->pin);
  short specialFunctions = digitalSystem->readEnableMXPSpecialFunction(status);
  digitalSystem->writeEnableMXPSpecialFunction(specialFunctions | bitToSet,
                                               status);

  return handle;
}
void freePWMPort(HalDigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HalHandleEnum::PWM);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (port->pin > tPWM::kNumHdrRegisters - 1) {
    uint32_t bitToUnset = 1 << remapMXPPWMChannel(port->pin);
    short specialFunctions =
        digitalSystem->readEnableMXPSpecialFunction(status);
    digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToUnset,
                                                 status);
  }

  digitalPinHandles.Free(pwm_port_handle, HalHandleEnum::PWM);
}

bool checkPWMChannel(uint8_t pin) { return pin < kNumPWMPins; }

/**
 * Set a PWM channel to the desired value. The values range from 0 to 255 and
 * the period is controlled
 * by the PWM Period and MinHigh registers.
 *
 * @param channel The PWM channel to set.
 * @param value The PWM value to set.
 */
void setPWM(HalDigitalHandle pwm_port_handle, unsigned short value,
            int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HalHandleEnum::PWM);
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
 * Get a value from a PWM channel. The values range from 0 to 255.
 *
 * @param channel The PWM channel to read from.
 * @return The raw PWM value.
 */
unsigned short getPWM(HalDigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HalHandleEnum::PWM);
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

void latchPWMZero(HalDigitalHandle pwm_port_handle, int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HalHandleEnum::PWM);
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
void setPWMPeriodScale(HalDigitalHandle pwm_port_handle, uint32_t squelchMask,
                       int32_t* status) {
  auto port = digitalPinHandles.Get(pwm_port_handle, HalHandleEnum::PWM);
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
uint16_t getLoopTiming(int32_t* status) {
  return pwmSystem->readLoopTiming(status);
}
}
