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
 * @defgroup hal_analogaccumulator Analog Accumulator Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Is the channel attached to an accumulator.
 *
 * @param analogPortHandle Handle to the analog port.
 * @return The analog channel is attached to an accumulator.
 */
HAL_Bool HAL_IsAccumulatorChannel(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status);

/**
 * Initialize the accumulator.
 *
 * @param analogPortHandle Handle to the analog port.
 */
void HAL_InitAccumulator(HAL_AnalogInputHandle analogPortHandle,
                         int32_t* status);

/**
 * Resets the accumulator to the initial value.
 *
 * @param analogPortHandle Handle to the analog port.
 */
void HAL_ResetAccumulator(HAL_AnalogInputHandle analogPortHandle,
                          int32_t* status);

/**
 * Set the center value of the accumulator.
 *
 * The center value is subtracted from each A/D value before it is added to the
 * accumulator. This is used for the center value of devices like gyros and
 * accelerometers to make integration work and to take the device offset into
 * account when integrating.
 *
 * This center value is based on the output of the oversampled and averaged
 * source from channel 1. Because of this, any non-zero oversample bits will
 * affect the size of the value for this field.
 *
 * @param analogPortHandle Handle to the analog port.
 * @param center The center value of the accumulator.
 */
void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analogPortHandle,
                              int32_t center, int32_t* status);

/**
 * Set the accumulator's deadband.
 *
 * @param analogPortHandle Handle to the analog port.
 * @param deadband The deadband of the accumulator.
 */
void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analogPortHandle,
                                int32_t deadband, int32_t* status);

/**
 * Read the accumulated value.
 *
 * Read the value that has been accumulating on channel 1.
 * The accumulator is attached after the oversample and average engine.
 *
 * @param analogPortHandle Handle to the analog port.
 * @return The 64-bit value accumulated since the last Reset().
 */
int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status);

/**
 * Read the number of accumulated values.
 *
 * Read the count of the accumulated values since the accumulator was last
 * Reset().
 *
 * @param analogPortHandle Handle to the analog port.
 * @return The number of times samples from the channel were accumulated.
 */
int64_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status);

/**
 * Read the accumulated value and the number of accumulated values atomically.
 *
 * This function reads the value and count from the FPGA atomically.
 * This can be used for averaging.
 *
 * @param analogPortHandle Handle to the analog port.
 * @param value Pointer to the 64-bit accumulated output.
 * @param count Pointer to the number of accumulation cycles.
 */
void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analogPortHandle,
                              int64_t* value, int64_t* count, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
