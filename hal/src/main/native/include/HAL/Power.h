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
 * Get the roboRIO input voltage
 */
double HAL_GetVinVoltage(int32_t* status);

/**
 * Get the roboRIO input current
 */
double HAL_GetVinCurrent(int32_t* status);

/**
 * Get the 6V rail voltage
 */
double HAL_GetUserVoltage6V(int32_t* status);

/**
 * Get the 6V rail current
 */
double HAL_GetUserCurrent6V(int32_t* status);

/**
 * Get the active state of the 6V rail
 */
HAL_Bool HAL_GetUserActive6V(int32_t* status);

/**
 * Get the fault count for the 6V rail
 */
int32_t HAL_GetUserCurrentFaults6V(int32_t* status);

/**
 * Get the 5V rail voltage
 */
double HAL_GetUserVoltage5V(int32_t* status);

/**
 * Get the 5V rail current
 */
double HAL_GetUserCurrent5V(int32_t* status);

/**
 * Get the active state of the 5V rail
 */
HAL_Bool HAL_GetUserActive5V(int32_t* status);

/**
 * Get the fault count for the 5V rail
 */
int32_t HAL_GetUserCurrentFaults5V(int32_t* status);

/**
 * Get the 3.3V rail voltage
 */
double HAL_GetUserVoltage3V3(int32_t* status);

/**
 * Get the 3.3V rail current
 */
double HAL_GetUserCurrent3V3(int32_t* status);

/**
 * Get the active state of the 3.3V rail
 */
HAL_Bool HAL_GetUserActive3V3(int32_t* status);

/**
 * Get the fault count for the 3.3V rail
 */
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
