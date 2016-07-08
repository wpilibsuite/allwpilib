/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

extern "C" {
bool HAL_IsAccumulatorChannel(HAL_AnalogInputHandle analog_port_handle,
                              int32_t* status);
void HAL_InitAccumulator(HAL_AnalogInputHandle analog_port_handle,
                         int32_t* status);
void HAL_ResetAccumulator(HAL_AnalogInputHandle analog_port_handle,
                          int32_t* status);
void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analog_port_handle,
                              int32_t center, int32_t* status);
void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analog_port_handle,
                                int32_t deadband, int32_t* status);
int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analog_port_handle,
                                int32_t* status);
uint32_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analog_port_handle,
                                 int32_t* status);
void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analog_port_handle,
                              int64_t* value, uint32_t* count, int32_t* status);
}
