/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Types.h"

extern "C" {
HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(HAL_PortHandle port_handle,
                                                    int32_t* status);
void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analog_port_handle);
HAL_Bool HAL_CheckAnalogModule(int32_t module);
HAL_Bool HAL_CheckAnalogInputChannel(int32_t pin);

void HAL_SetAnalogSampleRate(float samplesPerSecond, int32_t* status);
float HAL_GetAnalogSampleRate(int32_t* status);
void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analog_port_handle,
                              int32_t bits, int32_t* status);
int32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analog_port_handle,
                                 int32_t* status);
void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analog_port_handle,
                                 int32_t bits, int32_t* status);
int32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analog_port_handle,
                                    int32_t* status);
int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analog_port_handle,
                           int32_t* status);
int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analog_port_handle,
                                  int32_t* status);
int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analog_port_handle,
                                  float voltage, int32_t* status);
float HAL_GetAnalogVoltage(HAL_AnalogInputHandle analog_port_handle,
                           int32_t* status);
float HAL_GetAnalogAverageVoltage(HAL_AnalogInputHandle analog_port_handle,
                                  int32_t* status);
int64_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analog_port_handle,
                               int32_t* status);
int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analog_port_handle,
                            int32_t* status);
}
