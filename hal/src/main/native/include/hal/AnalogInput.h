// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_analoginput Analog Input Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the analog input port using the given port object.
 *
 * @param[in] channel the smartio channel.
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status the error code, or 0 for success
 * @return the created analog input handle
 */
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    int32_t channel, const char* allocationLocation, int32_t* status);

/**
 * Frees an analog input port.
 *
 * @param analogPortHandle Handle to the analog port.
 */
void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle);

/**
 * Checks that the analog module number is valid.
 *
 * @param[in] module The analog module number.
 * @return Analog module is valid and present
 */
HAL_Bool HAL_CheckAnalogModule(int32_t module);

/**
 * Checks that the analog output channel number is valid.
 * Verifies that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @param[in] channel The analog output channel number.
 * @return Analog channel is valid
 */
HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel);

/**
 * Indicates the analog input is used by a simulated device.
 *
 * @param handle the analog input handle
 * @param device simulated device handle
 */
void HAL_SetAnalogInputSimDevice(HAL_AnalogInputHandle handle,
                                 HAL_SimDeviceHandle device);

/**
 * Gets a sample straight from the channel on this module.
 *
 * The sample is a 12-bit value representing the 0V to 5V range of the A/D
 * converter in the module. The units are in A/D converter codes.  Use
 * GetVoltage() to get the analog value in calibrated units.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return A sample straight from the channel on this module.
 */
int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status);

/**
 * Converts a voltage to a raw value for a specified channel.
 *
 * This process depends on the calibration of each channel, so the channel must
 * be specified.
 *
 * @todo This assumes raw values.  Oversampling not supported as is.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[in] voltage The voltage to convert.
 * @param[out] status the error code, or 0 for success
 * @return The raw value for the channel.
 */
int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status);

/**
 * Gets a scaled sample straight from the channel on this module.
 *
 * The value is scaled to units of Volts.
 *
 * @param[in] analogPortHandle Handle to the analog port to use.
 * @param[out] status the error code, or 0 for success
 * @return A scaled sample straight from the channel on this module.
 */
double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status);

/**
 *  Get the analog voltage from a raw value.
 *
 * @param[in] analogPortHandle  Handle to the analog port the values were read
 *                              from.
 * @param[in] rawValue          The raw analog value
 * @param[out] status           Error status variable. 0 on success.
 * @return The voltage relating to the value
 */
double HAL_GetAnalogValueToVolts(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t rawValue, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
