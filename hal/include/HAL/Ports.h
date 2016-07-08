/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
int32_t HAL_GetNumAccumulators(void);
int32_t HAL_GetNumAnalogTriggers(void);
int32_t HAL_GetNumAnalogInputs(void);
int32_t HAL_GetNumAnalogOutputs(void);
int32_t HAL_GetNumCounters(void);
int32_t HAL_GetNumDigitalHeaders(void);
int32_t HAL_GetNumPWMHeaders(void);
int32_t HAL_GetNumDigitalPins(void);
int32_t HAL_GetNumPWMPins(void);
int32_t HAL_GetNumDigitalPWMOutputs(void);
int32_t HAL_GetNumEncoders(void);
int32_t HAL_GetNumInterrupts(void);
int32_t HAL_GetNumRelayPins(void);
int32_t HAL_GetNumRelayHeaders(void);
int32_t HAL_GetNumPCMModules(void);
int32_t HAL_GetNumSolenoidPins(void);
int32_t HAL_GetNumPDPModules(void);
int32_t HAL_GetNumPDPChannels(void);
int32_t HAL_GetNumCanTalons(void);
}
