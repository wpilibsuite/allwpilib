// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_analogoutput Analog Output Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the analog output port using the given port object.
 *
 * @param handle handle to the port
 * @return       the created analog output handle
 */
HAL_AnalogOutputHandle HAL_InitializeAnalogOutputPort(HAL_PortHandle portHandle,
                                                      int32_t* status);

/**
 * Frees an analog output port.
 *
 * @param analogOutputHandle the analog output handle
 */
void HAL_FreeAnalogOutputPort(HAL_AnalogOutputHandle analogOutputHandle);

/**
 * Sets an analog output value.
 *
 * @param analogOutputHandle the analog output handle
 * @param voltage            the voltage (0-5v) to output
 */
void HAL_SetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                         double voltage, int32_t* status);

/**
 * Gets the current analog output value.
 *
 * @param analogOutputHandle the analog output handle
 * @return                   the current output voltage (0-5v)
 */
double HAL_GetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                           int32_t* status);

/**
 * Checks that the analog output channel number is value.
 *
 * Verifies that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @return Analog channel is valid
 */
HAL_Bool HAL_CheckAnalogOutputChannel(int32_t channel);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
