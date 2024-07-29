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

/**
 * The types of power distribution devices.
 */
HAL_ENUM(HAL_PowerDistributionType) {
  HAL_PowerDistributionType_kAutomatic = 0,
  HAL_PowerDistributionType_kCTRE = 1,
  HAL_PowerDistributionType_kRev = 2,
};

#define HAL_DEFAULT_POWER_DISTRIBUTION_MODULE -1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a Power Distribution Panel.
 *
 * @param[in] moduleNumber       the module number to initialize
 * @param[in] type               the type of module to initialize
 * @param[in] allocationLocation the location where the allocation is occurring
 * @param[out] status            Error status variable. 0 on success.
 * @return the created PowerDistribution handle
 */
HAL_PowerDistributionHandle HAL_InitializePowerDistribution(
    int32_t moduleNumber, HAL_PowerDistributionType type,
    const char* allocationLocation, int32_t* status);

/**
 * Gets the module number for a specific handle.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the module number
 */
int32_t HAL_GetPowerDistributionModuleNumber(HAL_PowerDistributionHandle handle,
                                             int32_t* status);

/**
 * Cleans a PowerDistribution module.
 *
 * @param handle the module handle
 */
void HAL_CleanPowerDistribution(HAL_PowerDistributionHandle handle);

/**
 * Checks if a PowerDistribution channel is valid.
 *
 * @param handle  the module handle
 * @param channel the channel to check
 * @return true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckPowerDistributionChannel(HAL_PowerDistributionHandle handle,
                                           int32_t channel);

/**
 * Checks if a PowerDistribution module is valid.
 *
 * @param module the module to check
 * @param type   the type of module
 * @return true if the module is valid, otherwise false
 */
HAL_Bool HAL_CheckPowerDistributionModule(int32_t module,
                                          HAL_PowerDistributionType type);

/**
 * Gets the type of PowerDistribution module.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the type of module
 */
HAL_PowerDistributionType HAL_GetPowerDistributionType(
    HAL_PowerDistributionHandle handle, int32_t* status);

/**
 * Gets the number of channels for this handle.
 *
 * @param[in] handle the handle
 * @param[out] status Error status variable. 0 on success.
 * @return number of channels
 */
int32_t HAL_GetPowerDistributionNumChannels(HAL_PowerDistributionHandle handle,
                                            int32_t* status);

/**
 * Gets the temperature of the Power Distribution Panel.
 *
 * Not supported on the Rev PDH and returns 0.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the module temperature (celsius)
 */
double HAL_GetPowerDistributionTemperature(HAL_PowerDistributionHandle handle,
                                           int32_t* status);

/**
 * Gets the PowerDistribution input voltage.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the input voltage (volts)
 */
double HAL_GetPowerDistributionVoltage(HAL_PowerDistributionHandle handle,
                                       int32_t* status);

/**
 * Gets the current of a specific PowerDistribution channel.
 *
 * @param[in] handle   the module handle
 * @param[in] channel  the channel
 * @param[out] status  Error status variable. 0 on success.
 * @return the channel current (amps)
 */
double HAL_GetPowerDistributionChannelCurrent(
    HAL_PowerDistributionHandle handle, int32_t channel, int32_t* status);

/**
 * Gets the current of all channels on the PowerDistribution.
 *
 * The array must be large enough to hold all channels.
 *
 * @param[in] handle         the module handle
 * @param[out] currents      the currents
 * @param[in] currentsLength the length of the currents array
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_GetPowerDistributionAllChannelCurrents(
    HAL_PowerDistributionHandle handle, double* currents,
    int32_t currentsLength, int32_t* status);

/**
 * Gets the total current of the PowerDistribution.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the total current (amps)
 */
double HAL_GetPowerDistributionTotalCurrent(HAL_PowerDistributionHandle handle,
                                            int32_t* status);

/**
 * Gets the total power of the Power Distribution Panel.
 *
 * Not supported on the Rev PDH and returns 0.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the total power (watts)
 */
double HAL_GetPowerDistributionTotalPower(HAL_PowerDistributionHandle handle,
                                          int32_t* status);

/**
 * Gets the total energy of the Power Distribution Panel.
 *
 * Not supported on the Rev PDH and returns 0.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the total energy (joules)
 */
double HAL_GetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status);

/**
 * Resets the PowerDistribution accumulated energy.
 *
 * Not supported on the Rev PDH and does nothing.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_ResetPowerDistributionTotalEnergy(HAL_PowerDistributionHandle handle,
                                           int32_t* status);

/**
 * Clears any PowerDistribution sticky faults.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_ClearPowerDistributionStickyFaults(HAL_PowerDistributionHandle handle,
                                            int32_t* status);

/**
 * Power on/off switchable channel.
 *
 * This is a REV PDH-specific function. This function will no-op on CTRE PDP.
 *
 * @param[in] handle the module handle
 * @param[in] enabled true to turn on switchable channel
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, HAL_Bool enabled, int32_t* status);

/**
 * Returns true if switchable channel is powered on.
 *
 * This is a REV PDH-specific function. This function will no-op on CTRE PDP.
 *
 * @param[in] handle the module handle
 * @param[out] status Error status variable. 0 on success.
 * @return the state of the switchable channel
 */
HAL_Bool HAL_GetPowerDistributionSwitchableChannel(
    HAL_PowerDistributionHandle handle, int32_t* status);

/**
 * Power distribution version.
 */
struct HAL_PowerDistributionVersion {
  /// Firmware major version number.
  uint32_t firmwareMajor;
  /// Firmware minor version number.
  uint32_t firmwareMinor;
  /// Firmware fix version number.
  uint32_t firmwareFix;
  /// Hardware minor version number.
  uint32_t hardwareMinor;
  /// Hardware major version number.
  uint32_t hardwareMajor;
  /// Unique ID.
  uint32_t uniqueId;
};

struct HAL_PowerDistributionFaults {
  uint32_t channel0BreakerFault : 1;
  uint32_t channel1BreakerFault : 1;
  uint32_t channel2BreakerFault : 1;
  uint32_t channel3BreakerFault : 1;
  uint32_t channel4BreakerFault : 1;
  uint32_t channel5BreakerFault : 1;
  uint32_t channel6BreakerFault : 1;
  uint32_t channel7BreakerFault : 1;
  uint32_t channel8BreakerFault : 1;
  uint32_t channel9BreakerFault : 1;
  uint32_t channel10BreakerFault : 1;
  uint32_t channel11BreakerFault : 1;
  uint32_t channel12BreakerFault : 1;
  uint32_t channel13BreakerFault : 1;
  uint32_t channel14BreakerFault : 1;
  uint32_t channel15BreakerFault : 1;
  uint32_t channel16BreakerFault : 1;
  uint32_t channel17BreakerFault : 1;
  uint32_t channel18BreakerFault : 1;
  uint32_t channel19BreakerFault : 1;
  uint32_t channel20BreakerFault : 1;
  uint32_t channel21BreakerFault : 1;
  uint32_t channel22BreakerFault : 1;
  uint32_t channel23BreakerFault : 1;
  uint32_t brownout : 1;
  uint32_t canWarning : 1;
  uint32_t hardwareFault : 1;
};

/**
 * Storage for REV PDH Sticky Faults
 */
struct HAL_PowerDistributionStickyFaults {
  uint32_t channel0BreakerFault : 1;
  uint32_t channel1BreakerFault : 1;
  uint32_t channel2BreakerFault : 1;
  uint32_t channel3BreakerFault : 1;
  uint32_t channel4BreakerFault : 1;
  uint32_t channel5BreakerFault : 1;
  uint32_t channel6BreakerFault : 1;
  uint32_t channel7BreakerFault : 1;
  uint32_t channel8BreakerFault : 1;
  uint32_t channel9BreakerFault : 1;
  uint32_t channel10BreakerFault : 1;
  uint32_t channel11BreakerFault : 1;
  uint32_t channel12BreakerFault : 1;
  uint32_t channel13BreakerFault : 1;
  uint32_t channel14BreakerFault : 1;
  uint32_t channel15BreakerFault : 1;
  uint32_t channel16BreakerFault : 1;
  uint32_t channel17BreakerFault : 1;
  uint32_t channel18BreakerFault : 1;
  uint32_t channel19BreakerFault : 1;
  uint32_t channel20BreakerFault : 1;
  uint32_t channel21BreakerFault : 1;
  uint32_t channel22BreakerFault : 1;
  uint32_t channel23BreakerFault : 1;
  uint32_t brownout : 1;
  uint32_t canWarning : 1;
  uint32_t canBusOff : 1;
  uint32_t hasReset : 1;
};

/**
 * Get the version of the PowerDistribution.
 *
 * @param[in] handle the module handle
 * @param[out] version the HAL_PowerDistributionVersion to populate
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetPowerDistributionVersion(HAL_PowerDistributionHandle handle,
                                     HAL_PowerDistributionVersion* version,
                                     int32_t* status);
/**
 * Get the current faults of the PowerDistribution.
 *
 * @param[in] handle the module handle
 * @param[out] faults the HAL_PowerDistributionFaults to populate
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetPowerDistributionFaults(HAL_PowerDistributionHandle handle,
                                    HAL_PowerDistributionFaults* faults,
                                    int32_t* status);

/**
 * Gets the sticky faults of the PowerDistribution.
 *
 * @param[in] handle the module handle
 * @param[out] stickyFaults the HAL_PowerDistributionStickyFaults to populate
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetPowerDistributionStickyFaults(
    HAL_PowerDistributionHandle handle,
    HAL_PowerDistributionStickyFaults* stickyFaults, int32_t* status);

void HAL_StartPowerDistributionStream(HAL_PowerDistributionHandle handle,
                                      int32_t* status);

typedef struct HAL_PowerDistributionChannelData {
  float current;
  int32_t channel;
  uint32_t timestamp;
} HAL_PowerDistributionChannelData;

HAL_PowerDistributionChannelData* HAL_GetPowerDistributionStreamData(
    HAL_PowerDistributionHandle handle, int32_t* count, int32_t* status);

void HAL_FreePowerDistributionStreamData(HAL_PowerDistributionChannelData* data,
                                         int32_t count);

void HAL_StopPowerDistributionStream(HAL_PowerDistributionHandle handle,
                                     int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
