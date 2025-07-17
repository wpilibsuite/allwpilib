// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

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
 * @param[in] channel            the smartio channel
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status Error status variable. 0 on success.
 * @return the created duty cycle handle
 */
HAL_DutyCycleHandle HAL_InitializeDutyCycle(int32_t channel,
                                            const char* allocationLocation,
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
double HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
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

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
