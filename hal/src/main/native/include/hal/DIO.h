// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_dio DIO Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a new instance of a digital port.
 *
 * @param[in] channel            the smartio channel
 * @param[in] input              true for input, false for output
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status            Error status variable. 0 on success.
 * @return the created digital handle
 */
HAL_DigitalHandle HAL_InitializeDIOPort(int32_t channel, HAL_Bool input,
                                        const char* allocationLocation,
                                        int32_t* status);

/**
 * Checks if a DIO channel is valid.
 *
 * @param channel the channel number to check
 * @return true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckDIOChannel(int32_t channel);

/**
 * Frees a DIO port.
 *
 * @param dioPortHandle the DIO channel handle
 */
void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle);

/**
 * Indicates the DIO channel is used by a simulated device.
 *
 * @param handle the DIO channel handle
 * @param device simulated device handle
 */
void HAL_SetDIOSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device);

/**
 * Allocates a DO PWM Generator.
 *
 * @param[out] status Error status variable. 0 on success.
 * @return the allocated digital PWM handle
 */
HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status);

/**
 * Frees the resource associated with a DO PWM generator.
 *
 * @param[in] pwmGenerator the digital PWM handle
 */
void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator);

/**
 * Changes the frequency of the DO PWM generator.
 *
 * The valid range is from 0.6 Hz to 19 kHz.
 *
 * The frequency resolution is logarithmic.
 *
 * @param[in] rate the frequency to output all digital output PWM signals
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetDigitalPWMRate(double rate, int32_t* status);

/**
 * Configures the duty-cycle of the PWM generator.
 *
 * @param[in] pwmGenerator the digital PWM handle
 * @param[in] dutyCycle    the percent duty cycle to output [0..1]
 * @param[out] status      Error status variable. 0 on success.
 */
void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status);

/**
 * Configures the digital PWM to be a PPS signal with specified duty cycle.
 *
 * @param[in] pwmGenerator the digital PWM handle
 * @param[in] dutyCycle    the percent duty cycle to output [0..1]
 * @param[out] status      Error status variable. 0 on success.
 */
void HAL_SetDigitalPWMPPS(HAL_DigitalPWMHandle pwmGenerator, double dutyCycle,
                          int32_t* status);

/**
 * Configures which DO channel the PWM signal is output on.
 *
 * @param[in] pwmGenerator the digital PWM handle
 * @param[in] channel      the channel to output on
 * @param[out] status      Error status variable. 0 on success.
 */
void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t channel, int32_t* status);

/**
 * Writes a digital value to a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[in] value         the state to set the digital channel (if it is
 *                          configured as an output)
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status);

/**
 * Sets the direction of a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[in] input         true to set input, false for output
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status);

/**
 * Reads a digital value from a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the state of the specified channel
 */
HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Reads the direction of a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[out] status       Error status variable. 0 on success.
 * @return true for input, false for output
 */
HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Generates a single digital pulse.
 *
 * Write a pulse to the specified digital output channel. There can only be a
 * single pulse going at any time.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[in] pulseLength   the active length of the pulse in seconds
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLength,
               int32_t* status);

/**
 * Generates a single digital pulse on multiple channels.
 *
 * Write a pulse to the channels enabled by the mask. There can only be a
 * single pulse going at any time.
 *
 * @param[in] channelMask the channel mask
 * @param[in] pulseLength the active length of the pulse in seconds
 * @param[out] status     Error status variable. 0 on success.
 */
void HAL_PulseMultiple(uint32_t channelMask, double pulseLength,
                       int32_t* status);

/**
 * Checks a DIO line to see if it is currently generating a pulse.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[out] status Error status variable. 0 on success.
 * @return true if a pulse is in progress, otherwise false
 */
HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Checks if any DIO line is currently generating a pulse.
 *
 * @param[out] status Error status variable. 0 on success.
 * @return true if a pulse on some line is in progress
 */
HAL_Bool HAL_IsAnyPulsing(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
