// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/IMUTypes.h"

/**
 * @defgroup hal_imu IMU Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the acceleration along the axes of the IMU in meters per second squared.
 *
 * @param[out] accel the acceleration data
 * @param[out] status the error code, or 0 for success
 */
void HAL_GetIMUAcceleration(HAL_Acceleration3d* accel, int32_t* status);

/**
 * Get the angular rate about the axes of the IMU in radians per second.
 *
 * @param[out] rates the angular rate data
 * @param[out] status the error code, or 0 for success
 */
void HAL_GetIMUGyroRates(HAL_GyroRate3d* rates, int32_t* status);

/**
 * Get the angle about the axes of the IMU in radians.
 *
 * @param[out] angles the angle data
 * @param[out] status the error code, or 0 for success
 */
void HAL_GetIMUEulerAngles(HAL_EulerAngles3d* angles, int32_t* status);

/**
 * Get the orientation of the IMU as a quaternion.
 *
 * @param[out] quat the quaternion
 * @param[out] status the error code, or 0 for success
 */
void HAL_GetIMUQuaternion(HAL_Quaternion* quat, int32_t* status);

/**
 * Get the yaw value, in radians, of the IMU in the "flat" orientation.
 *
 * @param[out] timestamp the timestamp of the sample
 * @return flat orientation yaw
 */
double HAL_GetIMUYawFlat(int64_t* timestamp);

/**
 * Get the yaw value, in radians, of the IMU in the "landscape" orientation.
 *
 * @param[out] timestamp the timestamp of the sample
 * @return landscape orientation yaw
 */
double HAL_GetIMUYawLandscape(int64_t* timestamp);

/**
 * Get the yaw value, in radians, of the IMU in the "portrait" orientation.
 *
 * @param[out] timestamp the timestamp of the sample
 * @return portrait orientation yaw
 */
double HAL_GetIMUYawPortrait(int64_t* timestamp);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
