/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

/**
 * @defgroup hal_ports Ports Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the number of analog accumulators in the current system.
 *
 * @return the number of analog accumulators
 */
int32_t HAL_GetNumAccumulators(void);

/**
 * Gets the number of analog triggers in the current system.
 *
 * @return the number of analog triggers
 */
int32_t HAL_GetNumAnalogTriggers(void);

/**
 * Gets the number of analog inputs in the current system.
 *
 * @return the number of analog inputs
 */
int32_t HAL_GetNumAnalogInputs(void);

/**
 * Gets the number of analog outputs in the current system.
 *
 * @return the number of analog outputs
 */
int32_t HAL_GetNumAnalogOutputs(void);

/**
 * Gets the number of analog counters in the current system.
 *
 * @return the number of counters
 */
int32_t HAL_GetNumCounters(void);

/**
 * Gets the number of digital headers in the current system.
 *
 * @return the number of digital headers
 */
int32_t HAL_GetNumDigitalHeaders(void);

/**
 * Gets the number of PWM headers in the current system.
 *
 * @return the number of PWM headers
 */
int32_t HAL_GetNumPWMHeaders(void);

/**
 * Gets the number of digital channels in the current system.
 *
 * @return the number of digital channels
 */
int32_t HAL_GetNumDigitalChannels(void);

/**
 * Gets the number of PWM channels in the current system.
 *
 * @return the number of PWM channels
 */
int32_t HAL_GetNumPWMChannels(void);

/**
 * Gets the number of digital IO PWM outputs in the current system.
 *
 * @return the number of digital IO PWM outputs
 */
int32_t HAL_GetNumDigitalPWMOutputs(void);

/**
 * Gets the number of quadrature encoders in the current system.
 *
 * @return the number of quadrature encoders
 */
int32_t HAL_GetNumEncoders(void);

/**
 * Gets the number of interrupts in the current system.
 *
 * @return the number of interrupts
 */
int32_t HAL_GetNumInterrupts(void);

/**
 * Gets the number of relay channels in the current system.
 *
 * @return the number of relay channels
 */
int32_t HAL_GetNumRelayChannels(void);

/**
 * Gets the number of relay headers in the current system.
 *
 * @return the number of relay headers
 */
int32_t HAL_GetNumRelayHeaders(void);

/**
 * Gets the number of PCM modules in the current system.
 *
 * @return the number of PCM modules
 */
int32_t HAL_GetNumPCMModules(void);

/**
 * Gets the number of solenoid channels in the current system.
 *
 * @return the number of solenoid channels
 */
int32_t HAL_GetNumSolenoidChannels(void);

/**
 * Gets the number of PDP modules in the current system.
 *
 * @return the number of PDP modules
 */
int32_t HAL_GetNumPDPModules(void);

/**
 * Gets the number of PDP channels in the current system.
 *
 * @return the number of PDP channels
 */
int32_t HAL_GetNumPDPChannels(void);

/**
 * Gets the number of duty cycle inputs in the current system.
 *
 * @return the number of Duty Cycle inputs
 */
int32_t HAL_GetNumDutyCycles(void);

/**
 * Gets the number of addressable LED generators in the current system.
 *
 * @return the number of Addressable LED generators
 */
int32_t HAL_GetNumAddressableLEDs(void);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
