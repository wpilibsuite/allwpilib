// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_power Power Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the roboRIO input voltage.
 *
 * @return the input voltage (volts)
 */
double HAL_GetVinVoltage(int32_t* status);

/**
 * Gets the roboRIO input current.
 *
 * @return the input current (amps)
 */
double HAL_GetVinCurrent(int32_t* status);

/**
 * Gets the 6V rail voltage.
 *
 * @return the 6V rail voltage (volts)
 */
double HAL_GetUserVoltage6V(int32_t* status);

/**
 * Gets the 6V rail current.
 *
 * @return the 6V rail current (amps)
 */
double HAL_GetUserCurrent6V(int32_t* status);

/**
 * Gets the active state of the 6V rail.
 *
 * @return true if the rail is active, otherwise false
 */
HAL_Bool HAL_GetUserActive6V(int32_t* status);

/**
 * Gets the fault count for the 6V rail.
 *
 * @return the number of 6V fault counts
 */
int32_t HAL_GetUserCurrentFaults6V(int32_t* status);

/**
 * Gets the 5V rail voltage.
 *
 * @return the 5V rail voltage (volts)
 */
double HAL_GetUserVoltage5V(int32_t* status);

/**
 * Gets the 5V rail current.
 *
 * @return the 5V rail current (amps)
 */
double HAL_GetUserCurrent5V(int32_t* status);

/**
 * Gets the active state of the 5V rail.
 *
 * @return true if the rail is active, otherwise false
 */
HAL_Bool HAL_GetUserActive5V(int32_t* status);

/**
 * Gets the fault count for the 5V rail.
 *
 * @return the number of 5V fault counts
 */
int32_t HAL_GetUserCurrentFaults5V(int32_t* status);

/**
 * Gets the 3V3 rail voltage.
 *
 * @return the 3V3 rail voltage (volts)
 */
double HAL_GetUserVoltage3V3(int32_t* status);

/**
 * Gets the 3V3 rail current.
 *
 * @return the 3V3 rail current (amps)
 */
double HAL_GetUserCurrent3V3(int32_t* status);

/**
 * Gets the active state of the 3V3 rail.
 *
 * @return true if the rail is active, otherwise false
 */
HAL_Bool HAL_GetUserActive3V3(int32_t* status);

/**
 * Gets the fault count for the 3V3 rail.
 *
 * @return the number of 3V3 fault counts
 */
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
