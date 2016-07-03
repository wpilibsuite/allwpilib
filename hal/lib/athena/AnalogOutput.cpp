/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/AnalogOutput.h"

#include "AnalogInternal.h"
#include "HAL/Errors.h"
#include "PortsInternal.h"
#include "handles/HandlesInternal.h"
#include "handles/IndexedHandleResource.h"

using namespace hal;

namespace {
struct AnalogOutput {
  uint8_t pin;
};
}

static IndexedHandleResource<HalAnalogOutputHandle, AnalogOutput,
                             kNumAnalogOutputs, HalHandleEnum::AnalogOutput>
    analogOutputHandles;

extern "C" {

/**
 * Initialize the analog output port using the given port object.
 */
HalAnalogOutputHandle initializeAnalogOutputPort(HalPortHandle port_handle,
                                                 int32_t* status) {
  initializeAnalog(status);

  if (*status != 0) return HAL_INVALID_HANDLE;

  int16_t pin = getPortHandlePin(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  HalAnalogOutputHandle handle = analogOutputHandles.Allocate(pin, status);

  if (*status != 0)
    return HAL_INVALID_HANDLE;  // failed to allocate. Pass error back.

  auto port = analogOutputHandles.Get(handle);
  if (port == nullptr) {  // would only error on thread issue
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  port->pin = static_cast<uint8_t>(pin);
  return handle;
}

void freeAnalogOutputPort(HalAnalogOutputHandle analog_output_handle) {
  // no status, so no need to check for a proper free.
  analogOutputHandles.Free(analog_output_handle);
}

/**
 * Check that the analog output channel number is value.
 * Verify that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @return Analog channel is valid
 */
bool checkAnalogOutputChannel(uint32_t pin) {
  if (pin < kNumAnalogOutputs) return true;
  return false;
}

void setAnalogOutput(HalAnalogOutputHandle analog_output_handle, double voltage,
                     int32_t* status) {
  auto port = analogOutputHandles.Get(analog_output_handle);
  if (port == nullptr) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  uint16_t rawValue = (uint16_t)(voltage / 5.0 * 0x1000);

  if (voltage < 0.0)
    rawValue = 0;
  else if (voltage > 5.0)
    rawValue = 0x1000;

  analogOutputSystem->writeMXP(port->pin, rawValue, status);
}

double getAnalogOutput(HalAnalogOutputHandle analog_output_handle,
                       int32_t* status) {
  auto port = analogOutputHandles.Get(analog_output_handle);
  if (port == nullptr) {
    *status = PARAMETER_OUT_OF_RANGE;
    return 0.0;
  }

  uint16_t rawValue = analogOutputSystem->readMXP(port->pin, status);

  return rawValue * 5.0 / 0x1000;
}
}
