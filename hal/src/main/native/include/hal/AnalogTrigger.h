/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_analogtrigger Analog Trigger Functions
 * @ingroup hal_capi
 * @{
 */

// clang-format off
/**
 * The type of analog trigger to trigger on.
 */
HAL_ENUM(HAL_AnalogTriggerType) {
  HAL_Trigger_kInWindow = 0,
  HAL_Trigger_kState = 1,
  HAL_Trigger_kRisingPulse = 2,
  HAL_Trigger_kFallingPulse = 3
};
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes an analog trigger.
 *
 * @param portHandle the analog input to use for triggering
 * @return           the created analog trigger handle
 */
HAL_AnalogTriggerHandle HAL_InitializeAnalogTrigger(
    HAL_AnalogInputHandle portHandle, int32_t* status);

/**
 * Initializes an analog trigger with a Duty Cycle input
 *
 */
HAL_AnalogTriggerHandle HAL_InitializeAnalogTriggerDutyCycle(
    HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);

/**
 * Frees an analog trigger.
 *
 * @param analogTriggerHandle the trigger handle
 */
void HAL_CleanAnalogTrigger(HAL_AnalogTriggerHandle analogTriggerHandle,
                            int32_t* status);

/**
 * Sets the raw ADC upper and lower limits of the analog trigger.
 *
 * HAL_SetAnalogTriggerLimitsVoltage or HAL_SetAnalogTriggerLimitsDutyCycle
 * is likely better in most cases.
 *
 * @param analogTriggerHandle the trigger handle
 * @param lower               the lower ADC value
 * @param upper               the upper ADC value
 */
void HAL_SetAnalogTriggerLimitsRaw(HAL_AnalogTriggerHandle analogTriggerHandle,
                                   int32_t lower, int32_t upper,
                                   int32_t* status);

/**
 * Sets the upper and lower limits of the analog trigger.
 *
 * The limits are given as floating point voltage values.
 *
 * @param analogTriggerHandle the trigger handle
 * @param lower               the lower voltage value
 * @param upper               the upper voltage value
 */
void HAL_SetAnalogTriggerLimitsVoltage(
    HAL_AnalogTriggerHandle analogTriggerHandle, double lower, double upper,
    int32_t* status);

void HAL_SetAnalogTriggerLimitsDutyCycle(
    HAL_AnalogTriggerHandle analogTriggerHandle, double lower, double upper,
    int32_t* status);

/**
 * Configures the analog trigger to use the averaged vs. raw values.
 *
 * If the value is true, then the averaged value is selected for the analog
 * trigger, otherwise the immediate value is used.
 *
 * This is not allowed to be used if filtered mode is set.
 * This is not allowed to be used with Duty Cycle based inputs.
 *
 * @param analogTriggerHandle the trigger handle
 * @param useAveragedValue    true to use averaged values, false for raw
 */
void HAL_SetAnalogTriggerAveraged(HAL_AnalogTriggerHandle analogTriggerHandle,
                                  HAL_Bool useAveragedValue, int32_t* status);

/**
 * Configures the analog trigger to use a filtered value.
 *
 * The analog trigger will operate with a 3 point average rejection filter. This
 * is designed to help with 360 degree pot applications for the period where the
 * pot crosses through zero.
 *
 * This is not allowed to be used if averaged mode is set.
 *
 * @param analogTriggerHandle the trigger handle
 * @param useFilteredValue    true to use filtered values, false for average or
 * raw
 */
void HAL_SetAnalogTriggerFiltered(HAL_AnalogTriggerHandle analogTriggerHandle,
                                  HAL_Bool useFilteredValue, int32_t* status);

/**
 * Returns the InWindow output of the analog trigger.
 *
 * True if the analog input is between the upper and lower limits.
 *
 * @param analogTriggerHandle the trigger handle
 * @return                    the InWindow output of the analog trigger
 */
HAL_Bool HAL_GetAnalogTriggerInWindow(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status);

/**
 * Returns the TriggerState output of the analog trigger.
 *
 * True if above upper limit.
 * False if below lower limit.
 * If in Hysteresis, maintain previous state.
 *
 * @param analogTriggerHandle the trigger handle
 * @return                    the TriggerState output of the analog trigger
 */
HAL_Bool HAL_GetAnalogTriggerTriggerState(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status);

/**
 * Gets the state of the analog trigger output.
 *
 * @param analogTriggerHandle the trigger handle
 * @param type                the type of trigger to trigger on
 * @return                    the state of the analog trigger output
 */
HAL_Bool HAL_GetAnalogTriggerOutput(HAL_AnalogTriggerHandle analogTriggerHandle,
                                    HAL_AnalogTriggerType type,
                                    int32_t* status);

/**
 * Get the FPGA index for the AnlogTrigger.
 *
 * @param analogTriggerHandle the trigger handle
 * @return the FPGA index
 */
int32_t HAL_GetAnalogTriggerFPGAIndex(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
