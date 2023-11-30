// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

/**
 * @defgroup hal_dutycycle DutyCycle Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize a DutyCycle input.
 *
 * @param[in] digitalSourceHandle the digital source to use (either a
 *                                 HAL_DigitalHandle or a
 *                                 HAL_AnalogTriggerHandle)
 * @param[in] triggerType the analog trigger type of the source if it is
 *                         an analog trigger
 * @param[out] status Error status variable. 0 on success.
 * @return the created duty cycle handle
 */
HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle,
                                            HAL_AnalogTriggerType triggerType,
                                            int32_t* status);

/**
 * Free a DutyCycle.
 *
 * @param dutyCycleHandle the duty cycle handle
 */
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle);

/**
 * Indicates the duty cycle is used by a simulated device.
 *
 * @param handle the duty cycle handle
 * @param device simulated device handle
 */
void HAL_SetDutyCycleSimDevice(HAL_DutyCycleHandle handle,
                               HAL_SimDeviceHandle device);

/**
 * Get the frequency of the duty cycle signal.
 *
 * @param[in] dutyCycleHandle the duty cycle handle
 * @param[out] status Error status variable. 0 on success.
 * @return frequency in Hertz
 */
int32_t HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status);

/**
 * Get the output ratio of the duty cycle signal.
 *
 * <p> 0 means always low, 1 means always high.
 *
 * @param[in] dutyCycleHandle the duty cycle handle
 * @param[out] status Error status variable. 0 on success.
 * @return output ratio between 0 and 1
 */
double HAL_GetDutyCycleOutput(HAL_DutyCycleHandle dutyCycleHandle,
                              int32_t* status);

/**
 * Get the raw high time of the duty cycle signal.
 *
 * @param[in] dutyCycleHandle the duty cycle handle
 * @param[out] status Error status variable. 0 on success.
 * @return high time of last pulse in nanoseconds
 */
int32_t HAL_GetDutyCycleHighTime(HAL_DutyCycleHandle dutyCycleHandle,
                                 int32_t* status);

/**
 * Get the scale factor of the output.
 *
 * <p> An output equal to this value is always high, and then linearly scales
 * down to 0. Divide a raw result by this in order to get the
 * percentage between 0 and 1. Used by DMA.
 *
 * @param[in] dutyCycleHandle the duty cycle handle
 * @param[out] status Error status variable. 0 on success.
 * @return the output scale factor
 */
int32_t HAL_GetDutyCycleOutputScaleFactor(HAL_DutyCycleHandle dutyCycleHandle,
                                          int32_t* status);

/**
 * Get the FPGA index for the DutyCycle.
 *
 * @param[in] dutyCycleHandle the duty cycle handle
 * @param[out] status Error status variable. 0 on success.
 * @return the FPGA index
 */
int32_t HAL_GetDutyCycleFPGAIndex(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
