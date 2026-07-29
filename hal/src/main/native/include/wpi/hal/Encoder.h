// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

/**
 * @defgroup hal_encoder Encoder Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The type of index pulse for the encoder.
 */
HAL_ENUM(HAL_EncoderIndexingType) {
  HAL_ENCODER_INDEX_RESET_WHILE_HIGH,
  HAL_ENCODER_INDEX_RESET_WHILE_LOW,
  HAL_ENCODER_INDEX_RESET_ON_FALLING_EDGE,
  HAL_ENCODER_INDEX_RESET_ON_RISING_EDGE
};

/**
 * The encoding scaling of the encoder.
 */
HAL_ENUM(HAL_EncoderEncodingType) {
  HAL_ENCODER_1X_ENCODING,
  HAL_ENCODER_2X_ENCODING,
  HAL_ENCODER_4X_ENCODING
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes an encoder.
 *
 * @param[in] aChannel             the A channel
 * @param[in] bChannel             the B channel
 * @param[in] reverseDirection     true to reverse the counting direction from
 *                                 standard, otherwise false
 * @param[in] encodingType         the encoding type
 * @param[out] status              Error status variable. 0 on success.
 * @return the created encoder handle
 */
HAL_EncoderHandle HAL_InitializeEncoder(int32_t aChannel, int32_t bChannel,
                                        HAL_Bool reverseDirection,
                                        HAL_EncoderEncodingType encodingType,
                                        int32_t* status);

/**
 * Frees an encoder.
 *
 * @param[in] encoderHandle the encoder handle
 */
void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle);

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
 * This is scaled by the value passed during initialization to encodingType.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status Error status variable. 0 on success.
 * @return the current scaled count
 */
int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Gets the raw counts of the encoder.
 *
 * This is not scaled by any values.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status Error status variable. 0 on success.
 * @return the raw encoder count
 */
int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Gets the scale value used to convert raw encoder counts to scaled counts.
 *
 * Raw counts divided by this value produce the scaled encoder count.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the encoder scale value
 */
int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status);

/**
 * Reads the current encoder value.
 *
 * Read the value at this instant. It may still be running, so it reflects the
 * current value. Next time it is read, it might have a different value.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Determines if the clock is stopped.
 *
 * Determines if the encoder's current rate is zero.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return true if the encoder's current rate is zero
 */
HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status);

/**
 * Gets the last direction the encoder value changed.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the last direction the encoder value changed
 */
HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status);

/**
 * Gets the current distance traveled by the encoder.
 *
 * This is the encoder count scaled by the distance per pulse set for the
 * encoder.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the encoder distance (units are determined by the units
 *                      passed to HAL_SetEncoderDistancePerPulse)
 */
double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Gets the current rate of the encoder.
 *
 * This is the encoder rate scaled by the distance per pulse set for the
 * encoder.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the encoder rate (units are determined by the units passed to
 *         HAL_SetEncoderDistancePerPulse, time value is seconds)
 */
double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Sets the distance traveled per encoder pulse. This is used as a scaling
 * factor for the rate and distance calls.
 *
 * @param[in] encoderHandle    the encoder handle
 * @param[in] distancePerPulse the distance traveled per encoder pulse (units
 *                             user defined)
 * @param[out] status          Error status variable. 0 on success.
 */
void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status);

/**
 * Sets if to reverse the direction of the encoder.
 *
 * Note that this is not a toggle. It is an absolute set.
 *
 * @param[in] encoderHandle    the encoder handle
 * @param[in] reverseDirection true to reverse the direction, false to not.
 * @param[out] status          Error status variable. 0 on success.
 */
void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection, int32_t* status);

/**
 * Gets the FPGA index of the encoder.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the FPGA index of the encoder
 */
int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status);

/**
 * Gets the decoding scale factor of the encoder.
 *
 * This is used to perform the scaling from raw to type scaled values.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the scale value for the encoder
 */
double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status);

/**
 * Gets the user set distance per pulse of the encoder.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the set distance per pulse
 */
double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status);

/**
 * Gets the encoding type of the encoder.
 *
 * @param[in] encoderHandle the encoder handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the encoding type
 */
HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
