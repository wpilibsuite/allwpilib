/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/DIO.h"

#include <cmath>

#include "DigitalInternal.h"
#include "handles/HandlesInternal.h"

static_assert(sizeof(uint32_t) <= sizeof(void*),
              "This file shoves uint32_ts into pointers.");

using namespace hal;

// Create a mutex to protect changes to the digital output values
static priority_recursive_mutex digitalDIOMutex;

extern "C" {

/**
 * Create a new instance of a digital port.
 */
void* initializeDigitalPort(HalPortHandle port_handle, int32_t* status) {
  initializeDigital(status);

  if (*status != 0) return nullptr;

  int16_t pin = getPortHandlePin(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return nullptr;
  }

  // Initialize port structure
  DigitalPort* digital_port = new DigitalPort();
  digital_port->pin = (uint8_t)pin;

  return digital_port;
}

void freeDigitalPort(void* digital_port_pointer) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  delete port;
}

/**
 * Allocate a DO PWM Generator.
 * Allocate PWM generators so that they are not accidentally reused.
 *
 * @return PWM Generator refnum
 */
void* allocatePWM(int32_t* status) {
  return (void*)DO_PWMGenerators->Allocate("DO_PWM");
}

/**
 * Free the resource associated with a DO PWM generator.
 *
 * @param pwmGenerator The pwmGen to free that was allocated with
 * AllocateDO_PWM()
 */
void freePWM(void* pwmGenerator, int32_t* status) {
  uint32_t id = (uint32_t)pwmGenerator;
  if (id == ~0ul) return;
  DO_PWMGenerators->Free(id);
}

/**
 * Change the frequency of the DO PWM generator.
 *
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is
 * logarithmic.
 *
 * @param rate The frequency to output all digital output PWM signals.
 */
void setPWMRate(double rate, int32_t* status) {
  // Currently rounding in the log rate domain... heavy weight toward picking a
  // higher freq.
  // TODO: Round in the linear rate domain.
  uint8_t pwmPeriodPower = (uint8_t)(
      log(1.0 / (pwmSystem->readLoopTiming(status) * 0.25E-6 * rate)) /
          log(2.0) +
      0.5);
  digitalSystem->writePWMPeriodPower(pwmPeriodPower, status);
}

/**
 * Configure the duty-cycle of the PWM generator
 *
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param dutyCycle The percent duty cycle to output [0..1].
 */
void setPWMDutyCycle(void* pwmGenerator, double dutyCycle, int32_t* status) {
  uint32_t id = (uint32_t)pwmGenerator;
  if (id == ~0ul) return;
  if (dutyCycle > 1.0) dutyCycle = 1.0;
  if (dutyCycle < 0.0) dutyCycle = 0.0;
  float rawDutyCycle = 256.0 * dutyCycle;
  if (rawDutyCycle > 255.5) rawDutyCycle = 255.5;
  {
    std::lock_guard<priority_recursive_mutex> sync(digitalPwmMutex);
    uint8_t pwmPeriodPower = digitalSystem->readPWMPeriodPower(status);
    if (pwmPeriodPower < 4) {
      // The resolution of the duty cycle drops close to the highest
      // frequencies.
      rawDutyCycle = rawDutyCycle / std::pow(2.0, 4 - pwmPeriodPower);
    }
    if (id < 4)
      digitalSystem->writePWMDutyCycleA(id, (uint8_t)rawDutyCycle, status);
    else
      digitalSystem->writePWMDutyCycleB(id - 4, (uint8_t)rawDutyCycle, status);
  }
}

/**
 * Configure which DO channel the PWM signal is output on
 *
 * @param pwmGenerator The generator index reserved by AllocateDO_PWM()
 * @param channel The Digital Output channel to output on
 */
void setPWMOutputChannel(void* pwmGenerator, uint32_t pin, int32_t* status) {
  uint32_t id = (uint32_t)pwmGenerator;
  if (id > 5) return;
  if (pin >= kNumHeaders) {       // if it is on the MXP
    pin += kMXPDigitalPWMOffset;  // then to write as a digital PWM pin requires
                                  // an offset to write on the correct pin
  }
  digitalSystem->writePWMOutputSelect(id, pin, status);
}

/**
 * Allocate Digital I/O channels.
 * Allocate channels so that they are not accidently reused. Also the direction
 * is set at the time of the allocation.
 *
 * @param channel The Digital I/O channel
 * @param input If true open as input; if false open as output
 * @return Was successfully allocated
 */
bool allocateDIO(void* digital_port_pointer, bool input, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (port == nullptr) {
    *status = NULL_PARAMETER;
    return false;
  }
  char buf[64];
  snprintf(buf, 64, "DIO %d", port->pin);
  if (DIOChannels->Allocate(port->pin, buf) == ~0ul) {
    *status = RESOURCE_IS_ALLOCATED;
    return false;
  }

  {
    std::lock_guard<priority_recursive_mutex> sync(digitalDIOMutex);

    tDIO::tOutputEnable outputEnable = digitalSystem->readOutputEnable(status);

    if (port->pin < kNumHeaders) {
      uint32_t bitToSet = 1 << port->pin;
      if (input) {
        outputEnable.Headers =
            outputEnable.Headers & (~bitToSet);  // clear the bit for read
      } else {
        outputEnable.Headers =
            outputEnable.Headers | bitToSet;  // set the bit for write
      }
    } else {
      uint32_t bitToSet = 1 << remapMXPChannel(port->pin);

      // Disable special functions on this pin
      short specialFunctions =
          digitalSystem->readEnableMXPSpecialFunction(status);
      digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToSet,
                                                   status);

      if (input) {
        outputEnable.MXP =
            outputEnable.MXP & (~bitToSet);  // clear the bit for read
      } else {
        outputEnable.MXP =
            outputEnable.MXP | bitToSet;  // set the bit for write
      }
    }

    digitalSystem->writeOutputEnable(outputEnable, status);
  }
  return true;
}

/**
 * Free the resource associated with a digital I/O channel.
 *
 * @param channel The Digital I/O channel to free
 */
void freeDIO(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!port) return;
  DIOChannels->Free(port->pin);
}

/**
 * Write a digital I/O bit to the FPGA.
 * Set a single value on a digital I/O channel.
 *
 * @param channel The Digital I/O channel
 * @param value The state to set the digital channel (if it is configured as an
 * output)
 */
void setDIO(void* digital_port_pointer, short value, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (value != 0 && value != 1) {
    if (value != 0) value = 1;
  }
  {
    std::lock_guard<priority_recursive_mutex> sync(digitalDIOMutex);
    tDIO::tDO currentDIO = digitalSystem->readDO(status);

    if (port->pin < kNumHeaders) {
      if (value == 0) {
        currentDIO.Headers = currentDIO.Headers & ~(1 << port->pin);
      } else if (value == 1) {
        currentDIO.Headers = currentDIO.Headers | (1 << port->pin);
      }
    } else {
      if (value == 0) {
        currentDIO.MXP = currentDIO.MXP & ~(1 << remapMXPChannel(port->pin));
      } else if (value == 1) {
        currentDIO.MXP = currentDIO.MXP | (1 << remapMXPChannel(port->pin));
      }

      uint32_t bitToSet = 1 << remapMXPChannel(port->pin);
      short specialFunctions =
          digitalSystem->readEnableMXPSpecialFunction(status);
      digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToSet,
                                                   status);
    }
    digitalSystem->writeDO(currentDIO, status);
  }
}

/**
 * Read a digital I/O bit from the FPGA.
 * Get a single value from a digital I/O channel.
 *
 * @param channel The digital I/O channel
 * @return The state of the specified channel
 */
bool getDIO(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  tDIO::tDI currentDIO = digitalSystem->readDI(status);
  // Shift 00000001 over channel-1 places.
  // AND it against the currentDIO
  // if it == 0, then return false
  // else return true

  if (port->pin < kNumHeaders) {
    return ((currentDIO.Headers >> port->pin) & 1) != 0;
  } else {
    // Disable special functions
    uint32_t bitToSet = 1 << remapMXPChannel(port->pin);
    short specialFunctions =
        digitalSystem->readEnableMXPSpecialFunction(status);
    digitalSystem->writeEnableMXPSpecialFunction(specialFunctions & ~bitToSet,
                                                 status);

    return ((currentDIO.MXP >> remapMXPChannel(port->pin)) & 1) != 0;
  }
}

/**
 * Read the direction of a the Digital I/O lines
 * A 1 bit means output and a 0 bit means input.
 *
 * @param channel The digital I/O channel
 * @return The direction of the specified channel
 */
bool getDIODirection(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  tDIO::tOutputEnable currentOutputEnable =
      digitalSystem->readOutputEnable(status);
  // Shift 00000001 over port->pin-1 places.
  // AND it against the currentOutputEnable
  // if it == 0, then return false
  // else return true

  if (port->pin < kNumHeaders) {
    return ((currentOutputEnable.Headers >> port->pin) & 1) != 0;
  } else {
    return ((currentOutputEnable.MXP >> remapMXPChannel(port->pin)) & 1) != 0;
  }
}

/**
 * Generate a single pulse.
 * Write a pulse to the specified digital output channel. There can only be a
 * single pulse going at any time.
 *
 * @param channel The Digital Output channel that the pulse should be output on
 * @param pulseLength The active length of the pulse (in seconds)
 */
void pulse(void* digital_port_pointer, double pulseLength, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  tDIO::tPulse pulse;

  if (port->pin < kNumHeaders) {
    pulse.Headers = 1 << port->pin;
  } else {
    pulse.MXP = 1 << remapMXPChannel(port->pin);
  }

  digitalSystem->writePulseLength(
      (uint8_t)(1.0e9 * pulseLength / (pwmSystem->readLoopTiming(status) * 25)),
      status);
  digitalSystem->writePulse(pulse, status);
}

/**
 * Check a DIO line to see if it is currently generating a pulse.
 *
 * @return A pulse is in progress
 */
bool isPulsing(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  tDIO::tPulse pulseRegister = digitalSystem->readPulse(status);

  if (port->pin < kNumHeaders) {
    return (pulseRegister.Headers & (1 << port->pin)) != 0;
  } else {
    return (pulseRegister.MXP & (1 << remapMXPChannel(port->pin))) != 0;
  }
}

/**
 * Check if any DIO line is currently generating a pulse.
 *
 * @return A pulse on some line is in progress
 */
bool isAnyPulsing(int32_t* status) {
  tDIO::tPulse pulseRegister = digitalSystem->readPulse(status);
  return pulseRegister.Headers != 0 && pulseRegister.MXP != 0;
}

/**
 * Write the filter index from the FPGA.
 * Set the filter index used to filter out short pulses.
 *
 * @param digital_port_pointer The digital I/O channel
 * @param filter_index The filter index.  Must be in the range 0 - 3,
 * where 0 means "none" and 1 - 3 means filter # filter_index - 1.
 */
void setFilterSelect(void* digital_port_pointer, int filter_index,
                     int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;

  std::lock_guard<priority_recursive_mutex> sync(digitalDIOMutex);
  if (port->pin < kNumHeaders) {
    digitalSystem->writeFilterSelectHdr(port->pin, filter_index, status);
  } else {
    digitalSystem->writeFilterSelectMXP(remapMXPChannel(port->pin),
                                        filter_index, status);
  }
}

/**
 * Read the filter index from the FPGA.
 * Get the filter index used to filter out short pulses.
 *
 * @param digital_port_pointer The digital I/O channel
 * @return filter_index The filter index.  Must be in the range 0 - 3,
 * where 0 means "none" and 1 - 3 means filter # filter_index - 1.
 */
int getFilterSelect(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;

  std::lock_guard<priority_recursive_mutex> sync(digitalDIOMutex);
  if (port->pin < kNumHeaders) {
    return digitalSystem->readFilterSelectHdr(port->pin, status);
  } else {
    return digitalSystem->readFilterSelectMXP(remapMXPChannel(port->pin),
                                              status);
  }
}

/**
 * Set the filter period for the specified filter index.
 *
 * Set the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  That can be changed later.
 *
 * @param filter_index The filter index, 0 - 2.
 * @param value The number of cycles that the signal must not transition to be
 * counted as a transition.
 */
void setFilterPeriod(int filter_index, uint32_t value, int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(digitalDIOMutex);
  digitalSystem->writeFilterPeriodHdr(filter_index, value, status);
  if (*status == 0) {
    digitalSystem->writeFilterPeriodMXP(filter_index, value, status);
  }
}

/**
 * Get the filter period for the specified filter index.
 *
 * Get the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  Set status to NiFpga_Status_SoftwareFault if the
 * filter values miss-match.
 *
 * @param filter_index The filter index, 0 - 2.
 * @param value The number of cycles that the signal must not transition to be
 * counted as a transition.
 */
uint32_t getFilterPeriod(int filter_index, int32_t* status) {
  uint32_t hdr_period = 0;
  uint32_t mxp_period = 0;
  {
    std::lock_guard<priority_recursive_mutex> sync(digitalDIOMutex);
    hdr_period = digitalSystem->readFilterPeriodHdr(filter_index, status);
    if (*status == 0) {
      mxp_period = digitalSystem->readFilterPeriodMXP(filter_index, status);
    }
  }
  if (hdr_period != mxp_period) {
    *status = NiFpga_Status_SoftwareFault;
    return -1;
  }
  return hdr_period;
}
}
