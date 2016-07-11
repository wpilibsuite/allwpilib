/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Types.h"

enum HAL_AnalogTriggerType {
  HAL_Trigger_kInWindow = 0,
  HAL_Trigger_kState = 1,
  HAL_Trigger_kRisingPulse = 2,
  HAL_Trigger_kFallingPulse = 3
};

extern "C" {
HAL_AnalogTriggerHandle HAL_InitializeAnalogTrigger(
    HAL_AnalogInputHandle port_handle, int32_t* index, int32_t* status);
void HAL_CleanAnalogTrigger(HAL_AnalogTriggerHandle analog_trigger_handle,
                            int32_t* status);
void HAL_SetAnalogTriggerLimitsRaw(
    HAL_AnalogTriggerHandle analog_trigger_handle, int32_t lower, int32_t upper,
    int32_t* status);
void HAL_SetAnalogTriggerLimitsVoltage(
    HAL_AnalogTriggerHandle analog_trigger_handle, double lower, double upper,
    int32_t* status);
void HAL_SetAnalogTriggerAveraged(HAL_AnalogTriggerHandle analog_trigger_handle,
                                  HAL_Bool useAveragedValue, int32_t* status);
void HAL_SetAnalogTriggerFiltered(HAL_AnalogTriggerHandle analog_trigger_handle,
                                  HAL_Bool useFilteredValue, int32_t* status);
HAL_Bool HAL_GetAnalogTriggerInWindow(
    HAL_AnalogTriggerHandle analog_trigger_handle, int32_t* status);
HAL_Bool HAL_GetAnalogTriggerTriggerState(
    HAL_AnalogTriggerHandle analog_trigger_handle, int32_t* status);
HAL_Bool HAL_GetAnalogTriggerOutput(
    HAL_AnalogTriggerHandle analog_trigger_handle, HAL_AnalogTriggerType type,
    int32_t* status);
}
