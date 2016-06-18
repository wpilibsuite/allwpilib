/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/AnalogOutput.h"

#include "AnalogInternal.h"
#include "HAL/Errors.h"
#include "handles/HandlesInternal.h"

using namespace hal;

extern "C" {

/**
 * Initialize the analog output port using the given port object.
 */
void* initializeAnalogOutputPort(HalPortHandle port_handle, int32_t* status) {
  initializeAnalog(status);

  if (*status != 0) return nullptr;

  int16_t pin = getPortHandlePin(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return nullptr;
  }

  // Initialize port structure
  AnalogPort* analog_port = new AnalogPort();
  analog_port->pin = (uint8_t)pin;
  analog_port->accumulator = nullptr;
  return analog_port;
}

void freeAnalogOutputPort(void* analog_port_pointer) {
  AnalogPort* port = (AnalogPort*)analog_port_pointer;
  if (!port) return;
  delete port->accumulator;
  delete port;
}

/**
 * Check that the analog output channel number is value.
 * Verify that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @return Analog channel is valid
 */
bool checkAnalogOutputChannel(uint32_t pin) {
  if (pin < kAnalogOutputPins) return true;
  return false;
}

void setAnalogOutput(void* analog_port_pointer, double voltage,
                     int32_t* status) {
  AnalogPort* port = (AnalogPort*)analog_port_pointer;

  uint16_t rawValue = (uint16_t)(voltage / 5.0 * 0x1000);

  if (voltage < 0.0)
    rawValue = 0;
  else if (voltage > 5.0)
    rawValue = 0x1000;

  analogOutputSystem->writeMXP(port->pin, rawValue, status);
}

double getAnalogOutput(void* analog_port_pointer, int32_t* status) {
  AnalogPort* port = (AnalogPort*)analog_port_pointer;

  uint16_t rawValue = analogOutputSystem->readMXP(port->pin, status);

  return rawValue * 5.0 / 0x1000;
}
}
