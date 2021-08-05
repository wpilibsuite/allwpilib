// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_pd Power Distribution Functions
 * @ingroup hal_capi
 * Functions to control Power Distribution devices.
 * @{
 */

// clang-format off
/**
 * The acceptable accelerometer ranges.
 */
HAL_ENUM(HAL_PowerDistributionType) {
  HAL_PowerDistributionType_kAutomatic = 0,
  HAL_PowerDistributionType_kCTRE = 1,
  HAL_PowerDistributionType_kRev = 2,
};
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a Power Distribution Panel.
 *
 * @param  module the module number to initialize
 * @param  type   the type of module to intialize
 * @return the created PowerDistribution
 */
HAL_PowerDistributionHandle HAL_InitializePowerDistribution(
    int32_t moduleNumber, HAL_PowerDistributionType type, int32_t* status);

/**
 * Cleans a PowerDistribution module.
 *
 * @param handle the module handle
 */
void HAL_CleanPowerDistribution(HAL_PowerDistributionHandle handle);

/**
 * Checks if a PowerDistribution channel is valid.
 *
 * @param handle the module handle
 * @param channel the channel to check
 * @return        true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckPowerDistributionChannel(HAL_PowerDistributionHandle handle,
                                           int32_t channel);

/**
 * Checks if a PowerDistribution module is valid.
 *
 * @param channel the module to check
 * @return        true if the module is valid, otherwise false
 */
HAL_Bool HAL_CheckPowerDistributionModule(int32_t module,
                                          HAL_PowerDistributionType type);

/**
 * Gets the type of PowerDistribution module.
 *
 * @return the type of module
 */
HAL_PowerDistributionType HAL_GetPowerDistributionType(
    HAL_PowerDistributionHandle handle, int32_t* status);

/**
 * Gets the temperature of the PowerDistribution.
 *
 * @param handle the module handle
 * @return       the module temperature (celsius)
 */
double HAL_GetPowerDistributionTemperature(HAL_PowerDistributionHandle handle,
                                           int32_t* status);

/**
 * Gets the PowerDistribution input voltage.
 *
 * @param handle the module handle
 * @return       the input voltage (volts)
 */
double HAL_GetPowerDistributionVoltage(HAL_PowerDistributionHandle handle,
                                       int32_t* status);

/**
 * Gets the current of a specific PowerDistribution channel.
 *
 * @param module  the module
 * @param channel the channel
 * @return        the channel current (amps)
 */
double HAL_GetPowerDistributionChannelCurrent(
    HAL_PowerDistributionHandle handle, int32_t channel, int32_t* status);

/**
 * Gets the current of all 16 channels on the PowerDistribution.
 *
 * The array must be large enough to hold all channels.
 *
 * @param handle the module handle
 * @param currents the currents (output)
 * @param currentsLength the length of the currents array
 */
void HAL_GetPowerDistributionAllChannelCurrents(
    HAL_PowerDistributionHandle handle, double* currents,
    int32_t currentsLength, int32_t* status);

/**
 * Gets the total current of the PowerDistribution.
 *
 * @param handle the module handle
 * @return       the total current (amps)
 */
double HAL_GetPowerDistributionTotalCurrent(HAL_PowerDistributionHandle handle,
                                            int32_t* status);

/**
 * Gets the total power of the PowerDistribution.
 *
 * @param handle the module handle
 * @return       the total power (watts)
 */
double HAL_GetPowerDistributionTotalPower(HAL_PowerDistributionHandle handle,
                                          int32_t* status);

/**
 * Gets the total energy of the PowerDistribution.
 *
 * @param handle the module handle
 * @return       the total energy (joules)
 */
double HAL_GetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status);

/**
 * Resets the PowerDistribution accumulated energy.
 *
 * @param handle the module handle
 */
void HAL_ResetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status);

/**
 * Clears any PowerDistribution sticky faults.
 *
 * @param handle the module handle
 */
void HAL_ClearPowerDistributionStickyFaults(HAL_PowerDistributionHandle handle,
                                            int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
