/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_pdp PDP Functions
 * @ingroup hal_capi
 * Functions to control the Power Distribution Panel.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a Power Distribution Panel.
 *
 * @param  module the module number to initialize
 * @return the created PDP
 */
HAL_PDPHandle HAL_InitializePDP(int32_t module, int32_t* status);

/**
 * Cleans a PDP module.
 *
 * @param handle the module handle
 */
void HAL_CleanPDP(HAL_PDPHandle handle);

/**
 * Checks if a PDP channel is valid.
 *
 * @param channel the channel to check
 * @return        true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckPDPChannel(int32_t channel);

/**
 * Checks if a PDP module is valid.
 *
 * @param channel the module to check
 * @return        true if the module is valid, otherwise false
 */
HAL_Bool HAL_CheckPDPModule(int32_t module);

/**
 * Gets the temperature of the PDP.
 *
 * @param handle the module handle
 * @return       the module temperature (celsius)
 */
double HAL_GetPDPTemperature(HAL_PDPHandle handle, int32_t* status);

/**
 * Gets the PDP input voltage.
 *
 * @param handle the module handle
 * @return       the input voltage (volts)
 */
double HAL_GetPDPVoltage(HAL_PDPHandle handle, int32_t* status);

/**
 * Gets the current of a specific PDP channel.
 *
 * @param module  the module
 * @param channel the channel
 * @return        the channel current (amps)
 */
double HAL_GetPDPChannelCurrent(HAL_PDPHandle handle, int32_t channel,
                                int32_t* status);

/**
 * Gets the total current of the PDP.
 *
 * @param handle the module handle
 * @return       the total current (amps)
 */
double HAL_GetPDPTotalCurrent(HAL_PDPHandle handle, int32_t* status);

/**
 * Gets the total power of the PDP.
 *
 * @param handle the module handle
 * @return       the total power (watts)
 */
double HAL_GetPDPTotalPower(HAL_PDPHandle handle, int32_t* status);

/**
 * Gets the total energy of the PDP.
 *
 * @param handle the module handle
 * @return       the total energy (joules)
 */
double HAL_GetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status);

/**
 * Resets the PDP accumulated energy.
 *
 * @param handle the module handle
 */
void HAL_ResetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status);

/**
 * Clears any PDP sticky faults.
 *
 * @param handle the module handle
 */
void HAL_ClearPDPStickyFaults(HAL_PDPHandle handle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
