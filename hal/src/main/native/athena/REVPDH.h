// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_rev_pdh REV Power Distribution Hub API Functions
 * @ingroup hal_capi
 * @{
 */

struct REV_PDH_Version {
  uint32_t firmwareMajor;
  uint32_t firmwareMinor;
  uint32_t firmwareFix;
  uint32_t hardwareMinor;
  uint32_t hardwareMajor;
  uint32_t uniqueId;
};

/**
 * Storage for REV PDH Faults
 */
struct HAL_REVPDHFaults {
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
struct HAL_REVPDHStickyFaults {
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a REV Power Distribution Hub (PDH) device.
 *
 * @param module       the device CAN ID (1 .. 63)
 * @return the created PDH handle
 */
HAL_REVPDHHandle HAL_REV_InitializePDH(int32_t module,
                                       const char* allocationLocation,
                                       int32_t* status);

/**
 * Frees a PDH device handle.
 *
 * @param handle        the previously created PDH handle
 */
void HAL_REV_FreePDH(HAL_REVPDHHandle handle);

/**
 * Gets the module number for a pdh.
 */
int32_t HAL_REV_GetPDHModuleNumber(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Checks if a PDH module number is valid.
 *
 * Does not check if a PDH device with this module has been initialized.
 *
 * @param module        module number (1 .. 63)
 * @return 1 if the module number is valid; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHModuleNumber(int32_t module);

/**
 * Checks if a PDH channel number is valid.
 *
 * @param module        channel number (0 .. HAL_REV_PDH_NUM_CHANNELS)
 * @return 1 if the channel number is valid; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHChannelNumber(int32_t channel);

/**
 * Gets the current of a PDH channel in Amps.
 *
 * @param handle        PDH handle
 * @param channel       the channel to retrieve the current of (0 ..
 * HAL_REV_PDH_NUM_CHANNELS)
 *
 * @return the current of the PDH channel in Amps
 */
double HAL_REV_GetPDHChannelCurrent(HAL_REVPDHHandle handle, int32_t channel,
                                    int32_t* status);

/**
 * @param handle        PDH handle
 * @param currents      array of currents
 */
void HAL_REV_GetPDHAllChannelCurrents(HAL_REVPDHHandle handle, double* currents,
                                      int32_t* status);

/**
 * Gets the total current of the PDH in Amps, measured to the nearest even
 * integer.
 *
 * @param handle        PDH handle
 *
 * @return the total current of the PDH in Amps
 */
uint16_t HAL_REV_GetPDHTotalCurrent(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Sets the state of the switchable channel on a PDH device.
 *
 * @param handle        PDH handle
 * @param enabled       1 if the switchable channel should be enabled; 0
 * otherwise
 */
void HAL_REV_SetPDHSwitchableChannel(HAL_REVPDHHandle handle, HAL_Bool enabled,
                                     int32_t* status);

/**
 * Gets the current state of the switchable channel on a PDH device.
 *
 * This call relies on a periodic status sent by the PDH device and will be as
 * fresh as the last packet received.
 *
 * @param handle        PDH handle
 * @return 1 if the switchable channel is enabled; 0 otherwise
 */
HAL_Bool HAL_REV_GetPDHSwitchableChannelState(HAL_REVPDHHandle handle,
                                              int32_t* status);

/**
 * Gets the firmware and hardware versions of a PDH device.
 *
 * @param handle        PDH handle
 *
 * @return version information
 */
REV_PDH_Version HAL_REV_GetPDHVersion(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Gets the voltage being supplied to a PDH device.
 *
 * @param handle        PDH handle
 *
 * @return the voltage at the input of the PDH in Volts
 */
double HAL_REV_GetPDHSupplyVoltage(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Gets the faults of a PDH device.
 *
 * @param handle        PDH handle
 *
 * @return the faults of the PDH
 */
HAL_REVPDHFaults HAL_GetREVPDHFaults(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Gets the sticky faults of a PDH device.
 *
 * @param handle        PDH handle
 *
 * @return the sticky faults of the PDH
 */
HAL_REVPDHStickyFaults HAL_GetREVPDHStickyFaults(HAL_REVPDHHandle handle,
                                                 int32_t* status);

/**
 * Clears the sticky faults on a PDH device.
 *
 * @param handle        PDH handle
 */
void HAL_REV_ClearPDHFaults(HAL_REVPDHHandle handle, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
