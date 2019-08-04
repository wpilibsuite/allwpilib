/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

extern "C" {

HAL_FPGAEncoderHandle HAL_InitializeFPGAEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, int32_t* index, int32_t* status);
void HAL_FreeFPGAEncoder(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                         int32_t* status);

/**
 * Reset the Encoder distance to zero.
 * Resets the current count to zero on the encoder.
 */
void HAL_ResetFPGAEncoder(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                          int32_t* status);

/**
 * Gets the fpga value from the encoder.
 * The fpga value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 * @return Current fpga count from the encoder
 */
int32_t HAL_GetFPGAEncoder(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                           int32_t* status);  // Raw value

/**
 * Returns the period of the most recent pulse.
 * Returns the period of the most recent Encoder pulse in seconds.
 * This method compenstates for the decoding type.
 *
 * @deprecated Use GetRate() in favor of this method.  This returns unscaled
 * periods and GetRate() scales using value from SetDistancePerPulse().
 *
 * @return Period in seconds of the most recent pulse.
 */
double HAL_GetFPGAEncoderPeriod(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                int32_t* status);

/**
 * Sets the maximum period for stopped detection.
 * Sets the value that represents the maximum period of the Encoder before it
 * will assume that the attached device is stopped. This timeout allows users
 * to determine if the wheels or other shaft has stopped rotating.
 * This method compensates for the decoding type.
 *
 * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled
 * periods and SetMinRate() scales using value from SetDistancePerPulse().
 *
 * @param maxPeriod The maximum time between rising and falling edges before the
 * FPGA will
 * report the device stopped. This is expressed in seconds.
 */
void HAL_SetFPGAEncoderMaxPeriod(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                 double maxPeriod, int32_t* status);

/**
 * Determine if the encoder is stopped.
 * Using the MaxPeriod value, a boolean is returned that is true if the encoder
 * is considered stopped and false if it is still moving. A stopped encoder is
 * one where the most recent pulse width exceeds the MaxPeriod.
 * @return True if the encoder is considered stopped.
 */
HAL_Bool HAL_GetFPGAEncoderStopped(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                   int32_t* status);

/**
 * The last direction the encoder value changed.
 * @return The last direction the encoder value changed.
 */
HAL_Bool HAL_GetFPGAEncoderDirection(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                     int32_t* status);

/**
 * Set the direction sensing for this encoder.
 * This sets the direction sensing on the encoder so that it could count in the
 * correct software direction regardless of the mounting.
 * @param reverseDirection true if the encoder direction should be reversed
 */
void HAL_SetFPGAEncoderReverseDirection(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                        HAL_Bool reverseDirection,
                                        int32_t* status);

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void HAL_SetFPGAEncoderSamplesToAverage(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                        int32_t samplesToAverage,
                                        int32_t* status);

/**
 * Get the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
int32_t HAL_GetFPGAEncoderSamplesToAverage(
    HAL_FPGAEncoderHandle fpgaEncoderHandle, int32_t* status);

/**
 * Set an index source for an encoder, which is an input that resets the
 * encoder's count.
 */
void HAL_SetFPGAEncoderIndexSource(HAL_FPGAEncoderHandle fpgaEncoderHandle,
                                   HAL_Handle digitalSourceHandle,
                                   HAL_AnalogTriggerType analogTriggerType,
                                   HAL_Bool activeHigh, HAL_Bool edgeSensitive,
                                   int32_t* status);

}  // extern "C"
