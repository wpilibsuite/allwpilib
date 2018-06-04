/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the roboRIO input voltage.
 *
 * @return the input voltage (volts)
 */
double HAL_GetVinVoltage(int32_t* status);

/**
 * Get the roboRIO input current.
 *
 * @return the input current (amps)
 */
double HAL_GetVinCurrent(int32_t* status);

/**
 * Get the 6V rail voltage.
 *
 * @return the 6V rail voltage (volts)
 */
double HAL_GetUserVoltage6V(int32_t* status);

/**
 * Get the 6V rail current.
 *
 * @return the 6V rail current (amps)
 */
double HAL_GetUserCurrent6V(int32_t* status);

/**
 * Get the active state of the 6V rail.
 *
 * @return true if the rail is active, otherwise false
 */
HAL_Bool HAL_GetUserActive6V(int32_t* status);

/**
 * Get the fault count for the 6V rail.
 *
 * @return the number of 6V fault counts
 */
int32_t HAL_GetUserCurrentFaults6V(int32_t* status);

/**
 * Get the 5V rail voltage.
 *
 * @return the 5V rail voltage (volts)
 */
double HAL_GetUserVoltage5V(int32_t* status);

/**
 * Get the 5V rail current.
 *
 * @return the 5V rail current (amps)
 */
double HAL_GetUserCurrent5V(int32_t* status);

/**
 * Get the active state of the 5V rail.
 *
 * @return true if the rail is active, otherwise false
 */
HAL_Bool HAL_GetUserActive5V(int32_t* status);

/**
 * Get the fault count for the 5V rail.
 *
 * @return the number of 5V fault counts
 */
int32_t HAL_GetUserCurrentFaults5V(int32_t* status);

/**
 * Get the 3V3 rail voltage.
 *
 * @return the 3V3 rail voltage (volts)
 */
double HAL_GetUserVoltage3V3(int32_t* status);

/**
 * Get the 3V3 rail current.
 *
 * @return the 3V3 rail current (amps)
 */
double HAL_GetUserCurrent3V3(int32_t* status);

/**
 * Get the active state of the 3V3 rail.
 *
 * @return true if the rail is active, otherwise false
 */
HAL_Bool HAL_GetUserActive3V3(int32_t* status);

/**
 * Get the fault count for the 3V3 rail.
 *
 * @return the number of 3V3 fault counts
 */
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
