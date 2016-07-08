/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Ports.h"

#include "PortsInternal.h"

using namespace hal;

extern "C" {
int32_t HAL_getNumAccumulators(void) { return kNumAccumulators; }
int32_t HAL_getNumAnalogTriggers(void) { return kNumAnalogTriggers; }
int32_t HAL_getNumAnalogInputs(void) { return kNumAnalogInputs; }
int32_t HAL_getNumAnalogOutputs(void) { return kNumAnalogOutputs; }
int32_t HAL_getNumCounters(void) { return kNumCounters; }
int32_t HAL_getNumDigitalHeaders(void) { return kNumDigitalHeaders; }
int32_t HAL_getNumPWMHeaders(void) { return kNumPWMHeaders; }
int32_t HAL_getNumDigitalPins(void) { return kNumDigitalPins; }
int32_t HAL_getNumPWMPins(void) { return kNumPWMPins; }
int32_t HAL_getNumDigitalPWMOutputs(void) { return kNumDigitalPWMOutputs; }
int32_t HAL_getNumEncoders(void) { return kNumEncoders; }
int32_t HAL_getNumInterrupts(void) { return kNumInterrupts; }
int32_t HAL_getNumRelayPins(void) { return kNumRelayPins; }
int32_t HAL_getNumRelayHeaders(void) { return kNumRelayHeaders; }
int32_t HAL_getNumPCMModules(void) { return kNumPCMModules; }
int32_t HAL_getNumSolenoidPins(void) { return kNumSolenoidPins; }
int32_t HAL_getNumPDPModules(void) { return kNumPDPModules; }
int32_t HAL_getNumPDPChannels(void) { return kNumPDPChannels; }
int32_t HAL_getNumCanTalons(void) { return kNumCanTalons; }
}
