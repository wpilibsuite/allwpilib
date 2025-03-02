// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
 * Gets the number of can buses in the current system.
 *
 * @return the number of can buses
 */
int32_t HAL_GetNumCanBuses(void);

/**
 * Gets the number of smart IO channels in the current system.
 *
 * @return the number of SmartIO
 */
int32_t HAL_GetNumSmartIo(void);

/**
 * Gets the number of PCM modules in the current system.
 *
 * @return the number of PCM modules
 */
int32_t HAL_GetNumCTREPCMModules(void);

/**
 * Gets the number of solenoid channels in the current system.
 *
 * @return the number of solenoid channels
 */
int32_t HAL_GetNumCTRESolenoidChannels(void);

/**
 * Gets the number of PDP modules in the current system.
 *
 * @return the number of PDP modules
 */
int32_t HAL_GetNumCTREPDPModules(void);

/**
 * Gets the number of PDP channels in the current system.
 *
 * @return the number of PDP channels
 */
int32_t HAL_GetNumCTREPDPChannels(void);

/**
 * Gets the number of PDH modules in the current system.
 *
 * @return the number of PDH modules
 */
int32_t HAL_GetNumREVPDHModules(void);

/**
 * Gets the number of PDH channels in the current system.
 *
 * @return the number of PDH channels
 */
int32_t HAL_GetNumREVPDHChannels(void);

/**
 * Gets the number of PH modules in the current system.
 *
 * @return the number of PH modules
 */
int32_t HAL_GetNumREVPHModules(void);

/**
 * Gets the number of PH channels in the current system.
 *
 * @return the number of PH channels
 */
int32_t HAL_GetNumREVPHChannels(void);

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
