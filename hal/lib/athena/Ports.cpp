/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Ports.h"

#include "PortsInternal.h"

extern "C" {
int32_t HAL_GetNumAccumulators(void) { return hal::kNumAccumulators; }
int32_t HAL_GetNumAnalogTriggers(void) { return hal::kNumAnalogTriggers; }
int32_t HAL_GetNumAnalogInputs(void) { return hal::kNumAnalogInputs; }
int32_t HAL_GetNumAnalogOutputs(void) { return hal::kNumAnalogOutputs; }
int32_t HAL_GetNumCounters(void) { return hal::kNumCounters; }
int32_t HAL_GetNumDigitalHeaders(void) { return hal::kNumDigitalHeaders; }
int32_t HAL_GetNumPWMHeaders(void) { return hal::kNumPWMHeaders; }
int32_t HAL_GetNumDigitalChannels(void) { return hal::kNumDigitalChannels; }
int32_t HAL_GetNumPWMChannels(void) { return hal::kNumPWMChannels; }
int32_t HAL_GetNumDigitalPWMOutputs(void) { return hal::kNumDigitalPWMOutputs; }
int32_t HAL_GetNumEncoders(void) { return hal::kNumEncoders; }
int32_t HAL_GetNumInterrupts(void) { return hal::kNumInterrupts; }
int32_t HAL_GetNumRelayChannels(void) { return hal::kNumRelayChannels; }
int32_t HAL_GetNumRelayHeaders(void) { return hal::kNumRelayHeaders; }
int32_t HAL_GetNumPCMModules(void) { return hal::kNumPCMModules; }
int32_t HAL_GetNumSolenoidChannels(void) { return hal::kNumSolenoidChannels; }
int32_t HAL_GetNumPDPModules(void) { return hal::kNumPDPModules; }
int32_t HAL_GetNumPDPChannels(void) { return hal::kNumPDPChannels; }
}
