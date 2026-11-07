// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

/**
 * @defgroup hal_power Power Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the Systemcore input voltage.
 *
 * @param[out] status the error code, or 0 for success
 * @return the input voltage (volts)
 */
double HAL_GetVinVoltage(int32_t* status);

/**
 * Gets the 3V3 rail voltage.
 *
 * @param[out] status the error code, or 0 for success
 * @return the 3V3 rail voltage (volts)
 */
double HAL_GetUserVoltage3V3(int32_t* status);

/**
 * Gets the 3V3 rail current.
 *
 * @param[out] status the error code, or 0 for success
 * @return the 3V3 rail current (amps)
 */
double HAL_GetUserCurrent3V3(int32_t* status);

/**
 * Gets the active state of the 3V3 rail.
 *
 * @param[out] status the error code, or 0 for success
 * @return true if the rail is active, otherwise false
 */
HAL_Bool HAL_GetUserActive3V3(int32_t* status);

/**
 * Gets the fault count for the 3V3 rail. Capped at 255.
 *
 * @param[out] status the error code, or 0 for success
 * @return the number of 3V3 fault counts
 */
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status);

/**
 * Enables or disables the 3V3 rail.
 *
 * @param enabled whether the rail should be enabled
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetUserRailEnabled3V3(HAL_Bool enabled, int32_t* status);

/**
 * Resets the overcurrent fault counters for all user rails to 0.
 * @param[out] status the error code, or 0 for success
 */
void HAL_ResetUserCurrentFaults(int32_t* status);

/**
 * Set the voltages where the Systemcore will enter and recover from brownout.
 *
 * @param[in] brownoutVoltage the voltage where the Systemcore will enter
 *                            brownout
 * @param[in] recoveryVoltage the voltage where the Systemcore will recover
 *                            from brownout
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetBrownoutVoltages(double brownoutVoltage, double recoveryVoltage,
                             int32_t* status);

/**
 * Get the current CPU temperature in degrees Celsius
 *
 * @param[out] status the error code, or 0 for success
 * @return current CPU temperature in degrees Celsius
 */
double HAL_GetCPUTemp(int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
