// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_analoggyro Analog Gyro Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes an analog gyro.
 *
 * @param[in] handle handle to the analog input port
 * @param[in] allocationLocation the location where the allocation is occurring
 *                                (can be null)
 * @param[out] status the error code, or 0 for success
 * @return the initialized gyro handle
 */
HAL_GyroHandle HAL_InitializeAnalogGyro(HAL_AnalogInputHandle handle,
                                        const char* allocationLocation,
                                        int32_t* status);

/**
 * Sets up an analog gyro with the proper offsets and settings for the KOP
 * analog gyro.
 *
 * @param[in] handle the gyro handle
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetupAnalogGyro(HAL_GyroHandle handle, int32_t* status);

/**
 * Frees an analog gyro.
 *
 * @param handle the gyro handle
 */
void HAL_FreeAnalogGyro(HAL_GyroHandle handle);

/**
 * Sets the analog gyro parameters to the specified values.
 *
 * This is meant to be used if you want to reuse the values from a previous
 * calibration.
 *
 * @param[in] handle                  the gyro handle
 * @param[in] voltsPerDegreePerSecond the gyro volts scaling
 * @param[in] offset                  the gyro offset
 * @param[in] center                  the gyro center
 * @param[out] status                  the error code, or 0 for success
 */
void HAL_SetAnalogGyroParameters(HAL_GyroHandle handle,
                                 double voltsPerDegreePerSecond, double offset,
                                 int32_t center, int32_t* status);

/**
 * Sets the analog gyro volts per degrees per second scaling.
 *
 * @param[in] handle                  the gyro handle
 * @param[in] voltsPerDegreePerSecond the gyro volts scaling
 * @param[out] status                  the error code, or 0 for success
 */
void HAL_SetAnalogGyroVoltsPerDegreePerSecond(HAL_GyroHandle handle,
                                              double voltsPerDegreePerSecond,
                                              int32_t* status);

/**
 * Resets the analog gyro value to 0.
 *
 * @param[in] handle the gyro handle
 * @param[out] status the error code, or 0 for success
 */
void HAL_ResetAnalogGyro(HAL_GyroHandle handle, int32_t* status);

/**
 * Calibrates the analog gyro.
 *
 * This happens by calculating the average value of the gyro over 5 seconds, and
 * setting that as the center. Note that this call blocks for 5 seconds to
 * perform this.
 *
 * @param[in] handle the gyro handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_CalibrateAnalogGyro(HAL_GyroHandle handle, int32_t* status);

/**
 * Sets the deadband of the analog gyro.
 *
 * @param[in] handle the gyro handle
 * @param[in] volts  the voltage deadband
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetAnalogGyroDeadband(HAL_GyroHandle handle, double volts,
                               int32_t* status);

/**
 * Gets the gyro angle in degrees.
 *
 * @param[in] handle the gyro handle
 * @param[out] status Error status variable. 0 on success.
 * @return the gyro angle in degrees
 */
double HAL_GetAnalogGyroAngle(HAL_GyroHandle handle, int32_t* status);

/**
 * Gets the gyro rate in degrees/second.
 *
 * @param[in] handle the gyro handle
 * @param[out] status Error status variable. 0 on success.
 * @return the gyro rate in degrees/second
 */
double HAL_GetAnalogGyroRate(HAL_GyroHandle handle, int32_t* status);

/**
 * Gets the calibrated gyro offset.
 *
 * Can be used to not repeat a calibration but reconstruct the gyro object.
 *
 * @param[in] handle the gyro handle
 * @param[out] status Error status variable. 0 on success.
 * @return the gyro offset
 */
double HAL_GetAnalogGyroOffset(HAL_GyroHandle handle, int32_t* status);

/**
 * Gets the calibrated gyro center.
 *
 * Can be used to not repeat a calibration but reconstruct the gyro object.
 *
 * @param[in] handle the gyro handle
 * @param[out] status Error status variable. 0 on success.
 * @return the gyro center
 */
int32_t HAL_GetAnalogGyroCenter(HAL_GyroHandle handle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
