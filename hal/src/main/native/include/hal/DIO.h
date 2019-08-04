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
 * @param portHandle the port handle to create from
 * @param input      true for input, false for output
 * @return           the created digital handle
 */
HAL_DigitalHandle HAL_InitializeDIOPort(HAL_PortHandle portHandle,
                                        HAL_Bool input, int32_t* status);

/**
 * Checks if a DIO channel is valid.
 *
 * @param channel the channel number to check
 * @return        true if the channel is correct, otherwise false
 */
HAL_Bool HAL_CheckDIOChannel(int32_t channel);

void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle);

/**
 * Allocates a DO PWM Generator.
 *
 * @return the allocated digital PWM handle
 */
HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status);

/**
 * Frees the resource associated with a DO PWM generator.
 *
 * @param pwmGenerator the digital PWM handle
 */
void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator, int32_t* status);

/**
 * Changes the frequency of the DO PWM generator.
 *
 * The valid range is from 0.6 Hz to 19 kHz.
 *
 *  The frequency resolution is logarithmic.
 *
 * @param rate the frequency to output all digital output PWM signals
 */
void HAL_SetDigitalPWMRate(double rate, int32_t* status);

/**
 * Configures the duty-cycle of the PWM generator.
 *
 * @param pwmGenerator the digital PWM handle
 * @param dutyCycle    the percent duty cycle to output [0..1]
 */
void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status);

/**
 * Configures which DO channel the PWM signal is output on.
 *
 * @param pwmGenerator the digital PWM handle
 * @param channel      the channel to output on
 */
void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t channel, int32_t* status);

/**
 * Writes a digital value to a DIO channel.
 *
 * @param dioPortHandle the digital port handle
 * @param value         the state to set the digital channel (if it is
 * configured as an output)
 */
void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status);

/**
 * Sets the direction of a DIO channel.
 *
 * @param dioPortHandle the digital port handle
 * @param input         true to set input, false for output
 */
void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status);

/**
 * Reads a digital value from a DIO channel.
 *
 * @param dioPortHandle the digital port handle
 * @return              the state of the specified channel
 */
HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Reads the direction of a DIO channel.
 *
 * @param dioPortHandle the digital port handle
 * @return              true for input, false for output
 */
HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Generates a single digital pulse.
 *
 * Write a pulse to the specified digital output channel. There can only be a
 * single pulse going at any time.
 *
 * @param dioPortHandle the digital port handle
 * @param pulseLength   the active length of the pulse (in seconds)
 */
void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLength,
               int32_t* status);

/**
 * Checks a DIO line to see if it is currently generating a pulse.
 *
 * @return true if a pulse is in progress, otherwise false
 */
HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Checks if any DIO line is currently generating a pulse.
 *
 * @return true if a pulse on some line is in progress
 */
HAL_Bool HAL_IsAnyPulsing(int32_t* status);

/**
 * Writes the filter index from the FPGA.
 *
 * Set the filter index used to filter out short pulses.
 *
 * @param dioPortHandle the digital port handle
 * @param filterIndex   the filter index (Must be in the range 0 - 3, where 0
 * means "none" and 1 - 3 means filter # filterIndex - 1)
 */
void HAL_SetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t filterIndex,
                         int32_t* status);

/**
 * Reads the filter index from the FPGA.
 *
 * Gets the filter index used to filter out short pulses.
 *
 * @param dioPortHandle the digital port handle
 * @return filterIndex  the filter index (Must be in the range 0 - 3,
 * where 0 means "none" and 1 - 3 means filter # filterIndex - 1)
 */
int32_t HAL_GetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Sets the filter period for the specified filter index.
 *
 * Sets the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  That can be changed later.
 *
 * @param filterIndex the filter index, 0 - 2
 * @param value       the number of cycles that the signal must not transition
 * to be counted as a transition.
 */
void HAL_SetFilterPeriod(int32_t filterIndex, int64_t value, int32_t* status);

/**
 * Gets the filter period for the specified filter index.
 *
 * Gets the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  Set status to NiFpga_Status_SoftwareFault if the
 * filter values miss-match.
 *
 * @param filterIndex the filter index, 0 - 2
 * @param value       the number of cycles that the signal must not transition
 * to be counted as a transition.
 */
int64_t HAL_GetFilterPeriod(int32_t filterIndex, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
