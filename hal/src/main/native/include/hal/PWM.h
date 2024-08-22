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
 * @param[in] portHandle the port to initialize
 * @param[in] allocationLocation  the location where the allocation is occurring
 *                                (can be null)
 * @param[out] status             Error status variable. 0 on success.
 * @return the created pwm handle
 */
HAL_DigitalHandle HAL_InitializePWMPort(HAL_PortHandle portHandle,
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
 * Sets the configuration settings for the PWM channel.
 *
 * All values are in microseconds.
 *
 * @param[in] pwmPortHandle  the PWM handle
 * @param[in] maxPwm         the maximum PWM value
 * @param[in] deadbandMaxPwm the high range of the center deadband
 * @param[in] centerPwm      the center PWM value
 * @param[in] deadbandMinPwm the low range of the center deadband
 * @param[in] minPwm         the minimum PWM value
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetPWMConfigMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                  int32_t maxPwm, int32_t deadbandMaxPwm,
                                  int32_t centerPwm, int32_t deadbandMinPwm,
                                  int32_t minPwm, int32_t* status);

/**
 * Gets the pwm configuration settings for the PWM channel.
 *
 * Values are in microseconds.
 *
 * @param[in] pwmPortHandle  the PWM handle
 * @param[in] maxPwm         the maximum PWM value
 * @param[in] deadbandMaxPwm the high range of the center deadband
 * @param[in] centerPwm      the center PWM value
 * @param[in] deadbandMinPwm the low range of the center deadband
 * @param[in] minPwm         the minimum PWM value
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_GetPWMConfigMicroseconds(HAL_DigitalHandle pwmPortHandle,
                                  int32_t* maxPwm, int32_t* deadbandMaxPwm,
                                  int32_t* centerPwm, int32_t* deadbandMinPwm,
                                  int32_t* minPwm, int32_t* status);

/**
 * Sets if the FPGA should output the center value if the input value is within
 * the deadband.
 *
 * @param[in] pwmPortHandle     the PWM handle
 * @param[in] eliminateDeadband true to eliminate deadband, otherwise false
 * @param[out] status           Error status variable. 0 on success.
 */
void HAL_SetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                 HAL_Bool eliminateDeadband, int32_t* status);

/**
 * Gets the current eliminate deadband value.
 *
 * @param[in] pwmPortHandle the PWM handle
 * @param[out] status       Error status variable. 0 on success.
 * @return true if set, otherwise false
 */
HAL_Bool HAL_GetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                     int32_t* status);

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
 * Sets a PWM channel to the desired scaled value.
 *
 * The values range from -1 to 1 and the period is controlled by the PWM Period
 * and MinHigh registers.
 *
 * @param[in] pwmPortHandle the PWM handle
 * @param[in] speed         the scaled PWM value to set
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetPWMSpeed(HAL_DigitalHandle pwmPortHandle, double speed,
                     int32_t* status);

/**
 * Sets a PWM channel to the desired position value.
 *
 * The values range from 0 to 1 and the period is controlled by the PWM Period
 * and MinHigh registers.
 *
 * @param[in] pwmPortHandle the PWM handle
 * @param[in] position      the positional PWM value to set
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetPWMPosition(HAL_DigitalHandle pwmPortHandle, double position,
                        int32_t* status);

/**
 * Sets a PWM channel to be disabled.
 *
 * The channel is disabled until the next time it is set. Note this is different
 * from just setting a 0 speed, as this will actively stop all signaling on the
 * channel.
 *
 * @param[in] pwmPortHandle the PWM handle.
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetPWMDisabled(HAL_DigitalHandle pwmPortHandle, int32_t* status);

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
 * Gets a scaled value from a PWM channel.
 *
 * The values range from -1 to 1.
 *
 * @param[in] pwmPortHandle the PWM handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the current speed PWM value
 */
double HAL_GetPWMSpeed(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Gets a position value from a PWM channel.
 *
 * The values range from 0 to 1.
 *
 * @param[in] pwmPortHandle the PWM handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the current positional PWM value
 */
double HAL_GetPWMPosition(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Forces a PWM signal to go to 0 temporarily.
 *
 * @param[in] pwmPortHandle the PWM handle.
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_LatchPWMZero(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Sets how how often the PWM signal is squelched, thus scaling the period.
 *
 * @param[in] pwmPortHandle the PWM handle.
 * @param[in] squelchMask   the 2-bit mask of outputs to squelch
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetPWMPeriodScale(HAL_DigitalHandle pwmPortHandle, int32_t squelchMask,
                           int32_t* status);

/**
 * Sets the PWM output to be a continuous high signal while enabled.
 *
 * @param[in] pwmPortHandle the PWM handle.
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetPWMAlwaysHighMode(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Gets the loop timing of the PWM system.
 *
 * @param[out] status Error status variable. 0 on success.
 * @return the loop time in clock ticks
 */
int32_t HAL_GetPWMLoopTiming(int32_t* status);

/**
 * Gets the pwm starting cycle time.
 *
 * This time is relative to the FPGA time.
 *
 * @param[out] status Error status variable. 0 on success.
 * @return the pwm cycle start time
 */
uint64_t HAL_GetPWMCycleStartTime(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
