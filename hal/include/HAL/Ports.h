/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
int32_t HAL_getNumAccumulators(void);
int32_t HAL_getNumAnalogTriggers(void);
int32_t HAL_getNumAnalogInputs(void);
int32_t HAL_getNumAnalogOutputs(void);
int32_t HAL_getNumCounters(void);
int32_t HAL_getNumDigitalHeaders(void);
int32_t HAL_getNumPWMHeaders(void);
int32_t HAL_getNumDigitalPins(void);
int32_t HAL_getNumPWMPins(void);
int32_t HAL_getNumDigitalPWMOutputs(void);
int32_t HAL_getNumEncoders(void);
int32_t HAL_getNumInterrupts(void);
int32_t HAL_getNumRelayPins(void);
int32_t HAL_getNumRelayHeaders(void);
int32_t HAL_getNumPCMModules(void);
int32_t HAL_getNumSolenoidPins(void);
int32_t HAL_getNumPDPModules(void);
int32_t HAL_getNumPDPChannels(void);
int32_t HAL_getNumCanTalons(void);
}
