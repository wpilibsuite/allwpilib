// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_pwm PWM Output Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a PWM port.
 *
 * @param[in] channel the smartio channel
 * @param[in] allocationLocation  the location where the allocation is occurring
 *                                (can be null)
 * @param[out] status             Error status variable. 0 on success.
 * @return the created pwm handle
 */
HAL_DigitalHandle HAL_InitializePWMPort(int32_t channel,
                                        const char* allocationLocation,
                                        int32_t* status);

/**
 * Frees a PWM port.
 *
 * @param[in] pwmPortHandle the pwm handle
 */
void HAL_FreePWMPort(HAL_DigitalHandle pwmPortHandle);

/**
 * Checks if a pwm channel is valid.
 *
 * @param channel the channel to check
 * @return true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckPWMChannel(int32_t channel);

/**
 * Indicates the pwm is used by a simulated device.
 *
 * @param handle the pwm handle
 * @param device simulated device handle
 */
void HAL_SetPWMSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device);

/**
 * Sets a PWM channel to the desired pulse width in microseconds.
 *
 *
 * @param[in] pwmPortHandle the PWM handle
 * @param[in] microsecondPulseTime  the PWM value to set
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetPWMPulseTimeMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                     int32_t microsecondPulseTime,
                                     int32_t* status);

/**
 * Gets the current microsecond pulse time from a PWM channel.
 *
 * @param[in] pwmPortHandle the PWM handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the current PWM microsecond pulse time
 */
int32_t HAL_GetPWMPulseTimeMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                        int32_t* status);

/**
 * Sets the PWM output period.
 *
 * @param[in] pwmPortHandle the PWM handle.
 * @param[in] period   0 for 5ms, 1 or 2 for 10ms, 3 for 20ms
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetPWMOutputPeriod(HAL_DigitalHandle pwmPortHandle, int32_t period,
                            int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
