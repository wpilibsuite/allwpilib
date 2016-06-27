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
bool isAccumulatorChannel(HalAnalogInputHandle analog_port_handle,
                          int32_t* status);
void initAccumulator(HalAnalogInputHandle analog_port_handle, int32_t* status);
void resetAccumulator(HalAnalogInputHandle analog_port_handle, int32_t* status);
void setAccumulatorCenter(HalAnalogInputHandle analog_port_handle,
                          int32_t center, int32_t* status);
void setAccumulatorDeadband(HalAnalogInputHandle analog_port_handle,
                            int32_t deadband, int32_t* status);
int64_t getAccumulatorValue(HalAnalogInputHandle analog_port_handle,
                            int32_t* status);
uint32_t getAccumulatorCount(HalAnalogInputHandle analog_port_handle,
                             int32_t* status);
void getAccumulatorOutput(HalAnalogInputHandle analog_port_handle,
                          int64_t* value, uint32_t* count, int32_t* status);
}
