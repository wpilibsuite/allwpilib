/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
 * @param portHandle the port to initialize
 * @return           the created pwm handle
 */
HAL_DigitalHandle HAL_InitializePWMPort(HAL_PortHandle portHandle,
                                        int32_t* status);

/**
 * Frees a PWM port.
 *
 * @param pwmPortHandle the pwm handle
 */
void HAL_FreePWMPort(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Checks if a pwm channel is valid.
 *
 * @param channel the channel to check
 * @return        true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckPWMChannel(int32_t channel);

/**
 * Sets the configuration settings for the PWM channel.
 *
 * All values are in milliseconds.
 *
 * @param pwmPortHandle  the PWM handle
 * @param maxPwm         the maximum PWM value
 * @param deadbandMaxPwm the high range of the center deadband
 * @param centerPwm      the center PWM value
 * @param deadbandMinPwm the low range of the center deadband
 * @param minPwm         the minimum PWM value
 */
void HAL_SetPWMConfig(HAL_DigitalHandle pwmPortHandle, double maxPwm,
                      double deadbandMaxPwm, double centerPwm,
                      double deadbandMinPwm, double minPwm, int32_t* status);

/**
 * Sets the raw configuration settings for the PWM channel.
 *
 * We recommend using HAL_SetPWMConfig() instead, as those values are properly
 * scaled. Usually used for values grabbed by HAL_GetPWMConfigRaw().
 *
 * Values are in raw FPGA units.
 *
 * @param pwmPortHandle  the PWM handle
 * @param maxPwm         the maximum PWM value
 * @param deadbandMaxPwm the high range of the center deadband
 * @param centerPwm      the center PWM value
 * @param deadbandMinPwm the low range of the center deadband
 * @param minPwm         the minimum PWM value
 */
void HAL_SetPWMConfigRaw(HAL_DigitalHandle pwmPortHandle, int32_t maxPwm,
                         int32_t deadbandMaxPwm, int32_t centerPwm,
                         int32_t deadbandMinPwm, int32_t minPwm,
                         int32_t* status);

/**
 * Gets the raw pwm configuration settings for the PWM channel.
 *
 * Values are in raw FPGA units. These units have the potential to change for
 * any FPGA release.
 *
 * @param pwmPortHandle  the PWM handle
 * @param maxPwm         the maximum PWM value
 * @param deadbandMaxPwm the high range of the center deadband
 * @param centerPwm      the center PWM value
 * @param deadbandMinPwm the low range of the center deadband
 * @param minPwm         the minimum PWM value
 */
void HAL_GetPWMConfigRaw(HAL_DigitalHandle pwmPortHandle, int32_t* maxPwm,
                         int32_t* deadbandMaxPwm, int32_t* centerPwm,
                         int32_t* deadbandMinPwm, int32_t* minPwm,
                         int32_t* status);

/**
 * Sets if the FPGA should output the center value if the input value is within
 * the deadband.
 *
 * @param pwmPortHandle     the PWM handle
 * @param eliminateDeadband true to eliminate deadband, otherwise false
 */
void HAL_SetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                 HAL_Bool eliminateDeadband, int32_t* status);

/**
 * Gets the current eliminate deadband value.
 *
 * @param pwmPortHandle the PWM handle
 * @return              true if set, otherwise false
 */
HAL_Bool HAL_GetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                     int32_t* status);

/**
 * Sets a PWM channel to the desired value.
 *
 * The values are in raw FPGA units, and have the potential to change with any
 * FPGA release.
 *
 * @param pwmPortHandle the PWM handle
 * @param value         the PWM value to set
 */
void HAL_SetPWMRaw(HAL_DigitalHandle pwmPortHandle, int32_t value,
                   int32_t* status);

/**
 * Sets a PWM channel to the desired scaled value.
 *
 * The values range from -1 to 1 and the period is controlled by the PWM Period
 * and MinHigh registers.
 *
 * @param pwmPortHandle the PWM handle
 * @param value         the scaled PWM value to set
 */
void HAL_SetPWMSpeed(HAL_DigitalHandle pwmPortHandle, double speed,
                     int32_t* status);

/**
 * Sets a PWM channel to the desired position value.
 *
 * The values range from 0 to 1 and the period is controlled by the PWM Period
 * and MinHigh registers.
 *
 * @param pwmPortHandle the PWM handle
 * @param value         the positional PWM value to set
 */
void HAL_SetPWMPosition(HAL_DigitalHandle pwmPortHandle, double position,
                        int32_t* status);

/**
 * Sets a PWM channel to be disabled.
 *
 * The channel is disabled until the next time it is set. Note this is different
 * from just setting a 0 speed, as this will actively stop all signalling on the
 * channel.
 *
 * @param pwmPortHandle the PWM handle.
 */
void HAL_SetPWMDisabled(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Gets a value from a PWM channel.
 *
 * The values are in raw FPGA units, and have the potential to change with any
 * FPGA release.
 *
 * @param pwmPortHandle the PWM handle
 * @return              the current raw PWM value
 */
int32_t HAL_GetPWMRaw(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Gets a scaled value from a PWM channel.
 *
 * The values range from -1 to 1.
 *
 * @param pwmPortHandle the PWM handle
 * @return              the current speed PWM value
 */
double HAL_GetPWMSpeed(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Gets a position value from a PWM channel.
 *
 * The values range from 0 to 1.
 *
 * @param pwmPortHandle the PWM handle
 * @return              the current positional PWM value
 */
double HAL_GetPWMPosition(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Forces a PWM signal to go to 0 temporarily.
 *
 * @param pwmPortHandle the PWM handle.
 */
void HAL_LatchPWMZero(HAL_DigitalHandle pwmPortHandle, int32_t* status);

/**
 * Sets how how often the PWM signal is squelched, thus scaling the period.
 *
 * @param pwmPortHandle the PWM handle.
 * @param squelchMask   the 2-bit mask of outputs to squelch
 */
void HAL_SetPWMPeriodScale(HAL_DigitalHandle pwmPortHandle, int32_t squelchMask,
                           int32_t* status);

/**
 * Gets the loop timing of the PWM system.
 *
 * @return the loop time
 */
int32_t HAL_GetPWMLoopTiming(int32_t* status);

/**
 * Gets the pwm starting cycle time.
 *
 * This time is relative to the FPGA time.
 *
 * @return the pwm cycle start time
 */
uint64_t HAL_GetPWMCycleStartTime(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
