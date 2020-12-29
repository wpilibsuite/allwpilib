// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

/**
 * @defgroup hal_encoder Encoder Functions
 * @ingroup hal_capi
 * @{
 */

// clang-format off
/**
 * The type of index pulse for the encoder.
 */
HAL_ENUM(HAL_EncoderIndexingType) {
  HAL_kResetWhileHigh,
  HAL_kResetWhileLow,
  HAL_kResetOnFallingEdge,
  HAL_kResetOnRisingEdge
};

/**
 * The encoding scaling of the encoder.
 */
HAL_ENUM(HAL_EncoderEncodingType) {
  HAL_Encoder_k1X,
  HAL_Encoder_k2X,
  HAL_Encoder_k4X
};
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes an encoder.
 *
 * @param digitalSourceHandleA the A source (either a HAL_DigitalHandle or a
 * HAL_AnalogTriggerHandle)
 * @param analogTriggerTypeA   the analog trigger type of the A source if it is
 * an analog trigger
 * @param digitalSourceHandleB the B source (either a HAL_DigitalHandle or a
 * HAL_AnalogTriggerHandle)
 * @param analogTriggerTypeB   the analog trigger type of the B source if it is
 * an analog trigger
 * @param reverseDirection     true to reverse the counting direction from
 * standard, otherwise false
 * @param encodingType         the encoding type
   @return                     the created encoder handle
 */
HAL_EncoderHandle HAL_InitializeEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, HAL_EncoderEncodingType encodingType,
    int32_t* status);

/**
 * Frees an encoder.
 *
 * @param encoderHandle the encoder handle
 */
void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Indicates the encoder is used by a simulated device.
 *
 * @param handle the encoder handle
 * @param device simulated device handle
 */
void HAL_SetEncoderSimDevice(HAL_EncoderHandle handle,
                             HAL_SimDeviceHandle device);

/**
 * Gets the current counts of the encoder after encoding type scaling.
 *
 * This is scaled by the value passed duing initialization to encodingType.
 *
 * @param encoderHandle the encoder handle
 * @return the current scaled count
 */
int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Gets the raw counts of the encoder.
 *
 * This is not scaled by any values.
 *
 * @param encoderHandle the encoder handle
 * @return              the raw encoder count
 */
int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Gets the encoder scale value.
 *
 * This is set by the value passed during initialization to encodingType.
 *
 * @param encoderHandle the encoder handle
 * @return              the encoder scale value
 */
int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status);

/**
 * Reads the current encoder value.
 *
 * Read the value at this instant. It may still be running, so it reflects the
 * current value. Next time it is read, it might have a different value.
 *
 * @param encoderHandle the encoder handle
 * @return              the current encoder value
 */
void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);

/*
 * Gets the Period of the most recent count.
 *
 * Returns the time interval of the most recent count. This can be used for
 * velocity calculations to determine shaft speed.
 *
 * @param encoderHandle the encoder handle
 * @returns             the period of the last two pulses in units of seconds
 */
double HAL_GetEncoderPeriod(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Sets the maximum period where the device is still considered "moving".
 *
 * Sets the maximum period where the device is considered moving. This value is
 * used to determine the "stopped" state of the encoder using the
 * HAL_GetEncoderStopped method.
 *
 * @param encoderHandle the encoder handle
 * @param maxPeriod     the maximum period where the counted device is
 * considered moving in seconds
 */
void HAL_SetEncoderMaxPeriod(HAL_EncoderHandle encoderHandle, double maxPeriod,
                             int32_t* status);

/**
 * Determines if the clock is stopped.
 *
 * Determines if the clocked input is stopped based on the MaxPeriod value set
 * using the SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the
 * device (and encoder) are assumed to be stopped and it returns true.
 *
 * @param encoderHandle the encoder handle
 * @return              true if the most recent encoder period exceeds the
 * MaxPeriod value set by SetMaxPeriod
 */
HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status);

/**
 * Gets the last direction the encoder value changed.
 *
 * @param encoderHandle the encoder handle
 * @return              the last direction the encoder value changed
 */
HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status);

/**
 * Gets the current distance traveled by the encoder.
 *
 * This is the encoder count scaled by the distance per pulse set for the
 * encoder.
 *
 * @param encoderHandle the encoder handle
 * @return              the encoder distance (units are determined by the units
 * passed to HAL_SetEncoderDistancePerPulse)
 */
double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Gets the current rate of the encoder.
 *
 * This is the encoder period scaled by the distance per pulse set for the
 * encoder.
 *
 * @param encoderHandle the encoder handle
 * @return              the encoder rate (units are determined by the units
 * passed to HAL_SetEncoderDistancePerPulse, time value is seconds)
 */
double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Sets the minimum rate to be considered moving by the encoder.
 *
 * Units need to match what is set by HAL_SetEncoderDistancePerPulse, with time
 * as seconds.
 *
 * @param encoderHandle the encoder handle
 * @param minRate       the minimum rate to be considered moving (units are
 * determined by the units passed to HAL_SetEncoderDistancePerPulse, time value
 * is seconds)
 */
void HAL_SetEncoderMinRate(HAL_EncoderHandle encoderHandle, double minRate,
                           int32_t* status);

/**
 * Sets the distance traveled per encoder pulse. This is used as a scaling
 * factor for the rate and distance calls.
 *
 * @param encoderHandle    the encoder handle
 * @param distancePerPulse the distance traveled per encoder pulse (units user
 * defined)
 */
void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status);

/**
 * Sets if to reverse the direction of the encoder.
 *
 * Note that this is not a toggle. It is an absolute set.
 *
 * @param encoderHandle    the encoder handle
 * @param reverseDirection true to reverse the direction, false to not.
 */
void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection, int32_t* status);

/**
 * Sets the number of encoder samples to average when calculating encoder rate.
 *
 * @param encoderHandle    the encoder handle
 * @param samplesToAverage the number of samples to average
 */
void HAL_SetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                    int32_t samplesToAverage, int32_t* status);

/**
 * Gets the current samples to average value.
 *
 * @param encoderHandle the encoder handle
 * @return              the current samples to average value
 */
int32_t HAL_GetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                       int32_t* status);

/**
 * Sets the source for an index pulse on the encoder.
 *
 * The index pulse can be used to cause an encoder to reset based on an external
 * input.
 *
 * @param encoderHandle       the encoder handle
 * @param digitalSourceHandle the index source handle (either a
 * HAL_AnalogTriggerHandle of a HAL_DigitalHandle)
 * @param analogTriggerType   the analog trigger type if the source is an analog
 * trigger
 * @param type                the index triggering type
 */
void HAL_SetEncoderIndexSource(HAL_EncoderHandle encoderHandle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_EncoderIndexingType type, int32_t* status);

/**
 * Gets the FPGA index of the encoder.
 *
 * @param encoderHandle the encoder handle
 * @return              the FPGA index of the encoder
 */
int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status);

/**
 * Gets the decoding scale factor of the encoder.
 *
 * This is used to perform the scaling from raw to type scaled values.
 *
 * @param encoderHandle the encoder handle
 * @return              the scale value for the encoder
 */
double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status);

/**
 * Gets the user set distance per pulse of the encoder.
 *
 * @param encoderHandle the encoder handle
 * @return              the set distance per pulse
 */
double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status);

/**
 * Gets the encoding type of the encoder.
 *
 * @param encoderHandle the encoder handle
 * @return              the encoding type
 */
HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
