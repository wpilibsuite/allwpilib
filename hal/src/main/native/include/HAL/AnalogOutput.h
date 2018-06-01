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
 * Initialize the analog output port using the given port object.
 */
HAL_AnalogOutputHandle HAL_InitializeAnalogOutputPort(HAL_PortHandle portHandle,
                                                      int32_t* status);

void HAL_FreeAnalogOutputPort(HAL_AnalogOutputHandle analogOutputHandle);

void HAL_SetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                         double voltage, int32_t* status);

double HAL_GetAnalogOutput(HAL_AnalogOutputHandle analogOutputHandle,
                           int32_t* status);

/**
 * Check that the analog output channel number is value.
 * Verify that the analog channel number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @return Analog channel is valid
 */
HAL_Bool HAL_CheckAnalogOutputChannel(int32_t channel);
#ifdef __cplusplus
}  // extern "C"
#endif
