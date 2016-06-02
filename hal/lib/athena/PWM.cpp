/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/PWM.h"

#include "DigitalInternal.h"

static_assert(sizeof(uint32_t) <= sizeof(void*),
              "This file shoves uint32_ts into pointers.");

using namespace hal;

extern "C" {
bool checkPWMChannel(void* digital_port_pointer) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  return port->pin < kPwmPins;
}

/**
 * Check a port to make sure that it is not nullptr and is a valid PWM port.
 *
 * Sets the status to contain the appropriate error.
 *
 * @return true if the port passed validation.
 */
static bool verifyPWMChannel(DigitalPort* port, int32_t* status) {
  if (port == nullptr) {
    *status = NULL_PARAMETER;
    return false;
  } else if (!checkPWMChannel(port)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  } else {
    return true;
  }
}

/**
 * Set a PWM channel to the desired value. The values range from 0 to 255 and
 * the period is controlled
 * by the PWM Period and MinHigh registers.
 *
 * @param channel The PWM channel to set.
 * @param value The PWM value to set.
 */
void setPWM(void* digital_port_pointer, unsigned short value, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyPWMChannel(port, status)) {
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
unsigned short getPWM(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyPWMChannel(port, status)) {
    return 0;
  }

  if (port->pin < tPWM::kNumHdrRegisters) {
    return pwmSystem->readHdr(port->pin, status);
  } else {
    return pwmSystem->readMXP(port->pin - tPWM::kNumHdrRegisters, status);
  }
}

void latchPWMZero(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyPWMChannel(port, status)) {
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
void setPWMPeriodScale(void* digital_port_pointer, uint32_t squelchMask,
                       int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyPWMChannel(port, status)) {
    return;
  }

  if (port->pin < tPWM::kNumPeriodScaleHdrElements) {
    pwmSystem->writePeriodScaleHdr(port->pin, squelchMask, status);
  } else {
    pwmSystem->writePeriodScaleMXP(port->pin - tPWM::kNumPeriodScaleHdrElements,
                                   squelchMask, status);
  }
}

bool allocatePWMChannel(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyPWMChannel(port, status)) {
    return false;
  }

  char buf[64];
  snprintf(buf, 64, "PWM %d", port->pin);
  if (PWMChannels->Allocate(port->pin, buf) == ~0ul) {
    *status = RESOURCE_IS_ALLOCATED;
    return false;
  }

  if (port->pin > tPWM::kNumHdrRegisters - 1) {
    snprintf(buf, 64, "PWM %d and DIO %d", port->pin,
             remapMXPPWMChannel(port->pin) + 10);
    if (DIOChannels->Allocate(remapMXPPWMChannel(port->pin) + 10, buf) == ~0ul)
      return false;
    uint32_t bitToSet = 1 << remapMXPPWMChannel(port->pin);
    short specialFunctions =
        digitalSystem->readEnableMXPSpecialFunction(status);
    digitalSystem->writeEnableMXPSpecialFunction(specialFunctions | bitToSet,
                                                 status);
  }
  return true;
}

void freePWMChannel(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!port) return;
  if (!verifyPWMChannel(port, status)) {
    return;
  }

  PWMChannels->Free(port->pin);
  if (port->pin > tPWM::kNumHdrRegisters - 1) {
    DIOChannels->Free(remapMXPPWMChannel(port->pin) + 10);
    uint32_t bitToUnset = 1 << remapMXPPWMChannel(port->pin);
    short specialFunctions =
        digitalSystem->readEnableMXPSpecialFunction(status);
    digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToUnset,
                                                 status);
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
