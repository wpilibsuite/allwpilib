/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create a new instance of a digital port.
 */
HAL_DigitalHandle HAL_InitializeDIOPort(HAL_PortHandle portHandle,
                                        HAL_Bool input, int32_t* status);

HAL_Bool HAL_CheckDIOChannel(int32_t channel);

void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle);

/**
 * Allocate a DO PWM Generator.
 * Allocate PWM generators so that they are not accidentally reused.
 *
 * @return PWM Generator handle
 */
HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status);

/**
 * Free the resource associated with a DO PWM generator.
 *
 * @param pwmGenerator The pwmGen to free that was allocated with
 * allocateDigitalPWM()
 */
void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator, int32_t* status);

/**
 * Change the frequency of the DO PWM generator.
 *
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is
 * logarithmic.
 *
 * @param rate The frequency to output all digital output PWM signals.
 */
void HAL_SetDigitalPWMRate(double rate, int32_t* status);

/**
 * Configure the duty-cycle of the PWM generator
 *
 * @param pwmGenerator The generator index reserved by allocateDigitalPWM()
 * @param dutyCycle The percent duty cycle to output [0..1].
 */
void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status);

/**
 * Configure which DO channel the PWM signal is output on
 *
 * @param pwmGenerator The generator index reserved by allocateDigitalPWM()
 * @param channel The Digital Output channel to output on
 */
void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t channel, int32_t* status);

/**
 * Write a digital I/O bit to the FPGA.
 * Set a single value on a digital I/O channel.
 *
 * @param channel The Digital I/O channel
 * @param value The state to set the digital channel (if it is configured as an
 * output)
 */
void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status);

/**
 * Set direction of a DIO channel.
 *
 * @param channel The Digital I/O channel
 * @param input true to set input, false for output
 */
void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status);

/**
 * Read a digital I/O bit from the FPGA.
 * Get a single value from a digital I/O channel.
 *
 * @param channel The digital I/O channel
 * @return The state of the specified channel
 */
HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Read the direction of a the Digital I/O lines
 * A 1 bit means output and a 0 bit means input.
 *
 * @param channel The digital I/O channel
 * @return The direction of the specified channel
 */
HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Generate a single pulse.
 * Write a pulse to the specified digital output channel. There can only be a
 * single pulse going at any time.
 *
 * @param channel The Digital Output channel that the pulse should be output on
 * @param pulseLength The active length of the pulse (in seconds)
 */
void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLength,
               int32_t* status);

/**
 * Check a DIO line to see if it is currently generating a pulse.
 *
 * @return A pulse is in progress
 */
HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Check if any DIO line is currently generating a pulse.
 *
 * @return A pulse on some line is in progress
 */
HAL_Bool HAL_IsAnyPulsing(int32_t* status);

/**
 * Write the filter index from the FPGA.
 * Set the filter index used to filter out short pulses.
 *
 * @param dioPortHandle Handle to the digital I/O channel
 * @param filterIndex The filter index.  Must be in the range 0 - 3, where 0
 *                    means "none" and 1 - 3 means filter # filterIndex - 1.
 */
void HAL_SetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t filterIndex,
                         int32_t* status);

/**
 * Read the filter index from the FPGA.
 * Get the filter index used to filter out short pulses.
 *
 * @param dioPortHandle Handle to the digital I/O channel
 * @return filterIndex The filter index.  Must be in the range 0 - 3,
 * where 0 means "none" and 1 - 3 means filter # filterIndex - 1.
 */
int32_t HAL_GetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Set the filter period for the specified filter index.
 *
 * Set the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  That can be changed later.
 *
 * @param filterIndex The filter index, 0 - 2.
 * @param value The number of cycles that the signal must not transition to be
 * counted as a transition.
 */
void HAL_SetFilterPeriod(int32_t filterIndex, int64_t value, int32_t* status);

/**
 * Get the filter period for the specified filter index.
 *
 * Get the filter period in FPGA cycles.  Even though there are 2 different
 * filter index domains (MXP vs HDR), ignore that distinction for now since it
 * compilicates the interface.  Set status to NiFpga_Status_SoftwareFault if the
 * filter values miss-match.
 *
 * @param filterIndex The filter index, 0 - 2.
 * @param value The number of cycles that the signal must not transition to be
 * counted as a transition.
 */
int64_t HAL_GetFilterPeriod(int32_t filterIndex, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
