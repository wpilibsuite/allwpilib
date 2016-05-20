#include "HAL/Digital/PWM.hpp"

#include "HAL/HAL.hpp"
#include "ChipObject.h"
#include "DigitalInternal.hpp"

static_assert(sizeof(uint32_t) <= sizeof(void *), "This file shoves uint32_ts into pointers.");

extern "C"
{
int getMaxPositivePwm(PWMPort *port) {
    return port->maxPwm;
  }

  int getMinPositivePwm(PWMPort *port) {
    return port->eliminateDeadband ? port->deadbandMaxPwm : port->centerPwm + 1;
  }

int getCenterPwm(PWMPort *port) {
    return port->centerPwm;
  }

int getMaxNegativePwm(PWMPort *port) {
  return port->eliminateDeadband ? port->deadbandMinPwm : port->centerPwm - 1;
}

int getMinNegativePwm(PWMPort *port) {
  return port->minPwm;
}

int getPositiveScaleFactor(PWMPort *port) {
  return getMaxPositivePwm(port) - getMinPositivePwm(port);
} // /< The scale for positive speeds.

int getNegativeScaleFactor(PWMPort *port) {
  return getMaxNegativePwm(port) - getMinNegativePwm(port);
} // /< The scale for negative speeds.

int getFullRangeScaleFactor(PWMPort *port) {
  return getMaxPositivePwm(port) - getMinNegativePwm(port);
} // /< The scale for positions.


bool checkPWMChannel(void* pwm_port_pointer) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  return port->port.pin < kPwmPins;
}

uint32_t remapMXPPWMChannel(uint32_t pin) {
	if(pin < 14) {
		return pin - 10;	//first block of 4 pwms (MXP 0-3)
	} else {
		return pin - 6;	//block of PWMs after SPI
	}
}

/**
 * Check a port to make sure that it is not NULL and is a valid PWM port.
 *
 * Sets the status to contain the appropriate error.
 *
 * @return true if the port passed validation.
 */
static bool verifyPWMChannel(PWMPort *port, int32_t *status) {
  if (port == NULL) {
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
 * Create a new instance of a pwm port.
 */
void* initializePWMPort(void* port_pointer, int32_t *status) {
  initializeDigital(status);
  Port* port = (Port*) port_pointer;

  // Initialize port structure
  PWMPort* pwm_port = new PWMPort();
  pwm_port->port = *port;

  return pwm_port;
}

void freePWMPort(void* pwm_port_pointer) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  delete port;
}

void setPWMBoundsDouble(void* pwm_port_pointer, double max, double deadbandMax,
                        double center, double deadbandMin, double min,
                        int32_t *status) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  double loopTime =
      getLoopTiming(status) / (kSystemClockTicksPerMicrosecond * 1e3);
      
  if (*status != 0) return;
  
  port->maxPwm = (int32_t)((max - kDefaultPwmCenter) / loopTime +
                       kDefaultPwmStepsDown - 1);
  port->deadbandMaxPwm = (int32_t)((deadbandMax - kDefaultPwmCenter) / loopTime +
                               kDefaultPwmStepsDown - 1);
  port->centerPwm = (int32_t)((center - kDefaultPwmCenter) / loopTime +
                          kDefaultPwmStepsDown - 1);
  port->deadbandMinPwm = (int32_t)((deadbandMin - kDefaultPwmCenter) / loopTime +
                               kDefaultPwmStepsDown - 1);
  port->minPwm = (int32_t)((min - kDefaultPwmCenter) / loopTime +
                       kDefaultPwmStepsDown - 1);
}

void setPWMBounds(void* pwm_port_pointer, int32_t max, int32_t deadbandMax,
                  int32_t center, int32_t deadbandMin, int32_t min) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  port->maxPwm = max;
  port->deadbandMaxPwm = deadbandMax;
  port->centerPwm = center;
  port->deadbandMinPwm = deadbandMin;
  port->minPwm = min;
}

void getPWMBounds(void* pwm_port_pointer, int32_t *max, int32_t *deadbandMax,
                  int32_t *center, int32_t *deadbandMin, int32_t *min) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  *max = port->maxPwm;
  *deadbandMax = port->deadbandMaxPwm;
  *center = port->centerPwm;
  *deadbandMin = port->deadbandMinPwm;
  *min = port->minPwm;
}

void setPWMEliminateDeadband(void* pwm_port_pointer, bool eliminateDeadband) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  port->eliminateDeadband = eliminateDeadband;
}
bool getPWMEliminateDeadband(void* pwm_port_pointer) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  return port->eliminateDeadband;
}

/**
 * Set a PWM channel to the desired value. The values range from 0 to 255 and the period is controlled
 * by the PWM Period and MinHigh registers.
 *
 * @param channel The PWM channel to set.
 * @param value The PWM value to set.
 */
void setPWM(void* pwm_port_pointer, float value, int32_t *status) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  if (!verifyPWMChannel(port, status)) { return; }
  
  // clamp speed to be in the range 1.0 >= speed >= -1.0
    if (value < -1.0) {
      value = -1.0;
    } else if (value > 1.0) {
      value = 1.0;
    }

    // calculate the desired output pwm value by scaling the speed appropriately
    unsigned short rawValue;
    if (value == 0.0) {
      rawValue = getCenterPwm(port);
    } else if (value > 0.0) {
      rawValue =
          (unsigned short) (value * ((double) getPositiveScaleFactor(port)) + ((double) getMinPositivePwm(port)) + 0.5);
    } else {
      rawValue =
          (unsigned short) (value * ((double) getNegativeScaleFactor(port)) + ((double) getMaxNegativePwm(port)) + 0.5);
    }

  if(port->port.pin < tPWM::kNumHdrRegisters) {
    pwmSystem->writeHdr(port->port.pin, rawValue, status);
  } else {
    pwmSystem->writeMXP(port->port.pin - tPWM::kNumHdrRegisters, rawValue, status);
  }
}

/**
 * Get a value from a PWM channel. The values range from 0 to 255.
 *
 * @param channel The PWM channel to read from.
 * @return The raw PWM value.
 */
float getPWM(void* pwm_port_pointer, int32_t *status) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  if (!verifyPWMChannel(port, status)) { return 0; }
  
  unsigned short value = 0;

  if(port->port.pin < tPWM::kNumHdrRegisters) {
    value = pwmSystem->readHdr(port->port.pin, status);
  } else {
    value = pwmSystem->readMXP(port->port.pin - tPWM::kNumHdrRegisters, status);
  }
  
  if (value == kPwmDisabled) {
    return 0.0;
  } else if (value > getMaxPositivePwm(port)) {
    return 1.0;
  } else if (value < getMinNegativePwm(port)) {
    return -1.0;
  } else if (value > getMinPositivePwm(port)) {
    return (float)(value - getMinPositivePwm(port)) /
           (float)getPositiveScaleFactor(port);
  } else if (value < getMaxNegativePwm(port)) {
    return (float)(value - getMaxNegativePwm(port)) /
           (float)getNegativeScaleFactor(port);
  } else {
    return 0.0;
  }
}

/**
 * Set a PWM channel to the desired value. The values range from 0 to 255 and the period is controlled
 * by the PWM Period and MinHigh registers.
 *
 * @param channel The PWM channel to set.
 * @param value The PWM value to set.
 */
void setPWMRaw(void* pwm_port_pointer, unsigned short value, int32_t *status) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  if (!verifyPWMChannel(port, status)) { return; }

  if(port->port.pin < tPWM::kNumHdrRegisters) {
    pwmSystem->writeHdr(port->port.pin, value, status);
  } else {
    pwmSystem->writeMXP(port->port.pin - tPWM::kNumHdrRegisters, value, status);
  }
}

/**
 * Get a value from a PWM channel. The values range from 0 to 255.
 *
 * @param channel The PWM channel to read from.
 * @return The raw PWM value.
 */
unsigned short getPWMRaw(void* pwm_port_pointer, int32_t *status) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  if (!verifyPWMChannel(port, status)) { return 0; }

  if(port->port.pin < tPWM::kNumHdrRegisters) {
    return pwmSystem->readHdr(port->port.pin, status);
  } else {
    return pwmSystem->readMXP(port->port.pin - tPWM::kNumHdrRegisters, status);
  }
}

void latchPWMZero(void* pwm_port_pointer, int32_t *status) {
	PWMPort* port = (PWMPort*) pwm_port_pointer;
	if (!verifyPWMChannel(port, status)) { return; }

	pwmSystem->writeZeroLatch(port->port.pin, true, status);
	pwmSystem->writeZeroLatch(port->port.pin, false, status);
}

/**
 * Set how how often the PWM signal is squelched, thus scaling the period.
 *
 * @param channel The PWM channel to configure.
 * @param squelchMask The 2-bit mask of outputs to squelch.
 */
void setPWMPeriodScale(void* pwm_port_pointer, uint32_t squelchMask, int32_t *status) {
  PWMPort* port = (PWMPort*) pwm_port_pointer;
  if (!verifyPWMChannel(port, status)) { return; }

  if(port->port.pin < tPWM::kNumPeriodScaleHdrElements) {
    pwmSystem->writePeriodScaleHdr(port->port.pin, squelchMask, status);
  } else {
    pwmSystem->writePeriodScaleMXP(port->port.pin - tPWM::kNumPeriodScaleHdrElements, squelchMask, status);
  }
}


bool allocatePWMChannel(void* pwm_port_pointer, int32_t *status) {
	PWMPort* port = (PWMPort*) pwm_port_pointer;
	if (!verifyPWMChannel(port, status)) { return false; }

	char buf[64];
	snprintf(buf, 64, "PWM %d", port->port.pin);
	if (PWMChannels->Allocate(port->port.pin, buf) == ~0ul) {
		*status = RESOURCE_IS_ALLOCATED;
		return false;
    }

	if (port->port.pin > tPWM::kNumHdrRegisters-1) {
		snprintf(buf, 64, "PWM %d and DIO %d", port->port.pin, remapMXPPWMChannel(port->port.pin) + 10);
		if (DIOChannels->Allocate(remapMXPPWMChannel(port->port.pin) + 10, buf) == ~0ul) return false;
		uint32_t bitToSet = 1 << remapMXPPWMChannel(port->port.pin);
		short specialFunctions = digitalSystem->readEnableMXPSpecialFunction(status);
		digitalSystem->writeEnableMXPSpecialFunction(specialFunctions | bitToSet, status);
	}
	return true;
}

void freePWMChannel(void* pwm_port_pointer, int32_t *status) {
    PWMPort* port = (PWMPort*) pwm_port_pointer;
    if (!port) return;
    if (!verifyPWMChannel(port, status)) { return; }

    PWMChannels->Free(port->port.pin);
    if(port->port.pin > tPWM::kNumHdrRegisters-1) {
        DIOChannels->Free(remapMXPPWMChannel(port->port.pin) + 10);
        uint32_t bitToUnset = 1 << remapMXPPWMChannel(port->port.pin);
        short specialFunctions = digitalSystem->readEnableMXPSpecialFunction(status);
        digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToUnset, status);
    }
}
} // extern "C"