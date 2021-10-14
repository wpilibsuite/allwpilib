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
  uint32_t hardwareRev;
  uint32_t uniqueId;
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
 * Checks if a PDH channel is currently experiencing a brownout condition.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 * @param channel       the channel to retrieve the brownout status of
 *
 * @return 1 if the channel is experiencing a brownout; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHChannelBrownout(HAL_REVPDHHandle handle,
                                         int32_t channel, int32_t* status);

/**
 * Gets the voltage being supplied to a PDH device.
 *
 * @param handle        PDH handle
 *
 * @return the voltage at the input of the PDH in Volts
 */
double HAL_REV_GetPDHSupplyVoltage(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Checks if a PDH device is currently enabled.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the PDH is enabled; 0 otherwise
 */
HAL_Bool HAL_REV_IsPDHEnabled(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Checks if the input voltage on a PDH device is currently below the minimum
 * voltage.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the PDH is experiencing a brownout; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHBrownout(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Checks if the CAN RX or TX error levels on a PDH device have exceeded the
 * warning threshold.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device has exceeded the warning threshold; 0
 * otherwise
 */
HAL_Bool HAL_REV_CheckPDHCANWarning(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Checks if a PDH device is currently malfunctioning.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device is in a hardware fault state; 0
 * otherwise
 */
HAL_Bool HAL_REV_CheckPDHHardwareFault(HAL_REVPDHHandle handle,
                                       int32_t* status);

/**
 * Checks if the input voltage on a PDH device has gone below the specified
 * minimum voltage.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device has had a brownout; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHStickyBrownout(HAL_REVPDHHandle handle,
                                        int32_t* status);

/**
 * Checks if the CAN RX or TX error levels on a PDH device have exceeded the
 * warning threshold.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device has exceeded the CAN warning threshold;
 * 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHStickyCANWarning(HAL_REVPDHHandle handle,
                                          int32_t* status);

/**
 * Checks if the CAN bus on a PDH device has previously experienced a 'Bus Off'
 * event.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device has experienced a 'Bus Off' event; 0
 * otherwise
 */
HAL_Bool HAL_REV_CheckPDHStickyCANBusOff(HAL_REVPDHHandle handle,
                                         int32_t* status);

/**
 * Checks if a PDH device has malfunctioned.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device has had a malfunction; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHStickyHardwareFault(HAL_REVPDHHandle handle,
                                             int32_t* status);

/**
 * Checks if the firmware on a PDH device has malfunctioned and reset during
 * operation.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device has had a malfunction and reset; 0
 * otherwise
 */
HAL_Bool HAL_REV_CheckPDHStickyFirmwareFault(HAL_REVPDHHandle handle,
                                             int32_t* status);

/**
 * Checks if a brownout has happened on channels 20-23 of a PDH device while it
 * was enabled.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 * @param channel       PDH channel to retrieve sticky brownout status (20 ..
 * 23)
 *
 *
 * @return 1 if the channel has had a brownout; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHStickyChannelBrownout(HAL_REVPDHHandle handle,
                                               int32_t channel,
                                               int32_t* status);

/**
 * Checks if a PDH device has reset.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 *
 * @return 1 if the device has reset; 0 otherwise
 */
HAL_Bool HAL_REV_CheckPDHStickyHasReset(HAL_REVPDHHandle handle,
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
 * Clears the sticky faults on a PDH device.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 */
void HAL_REV_ClearPDHFaults(HAL_REVPDHHandle handle, int32_t* status);

/**
 * Identifies a PDH device by blinking its LED.
 *
 * NOTE: Not implemented in firmware as of 2021-04-23.
 *
 * @param handle        PDH handle
 */
void HAL_REV_IdentifyPDH(HAL_REVPDHHandle handle, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
