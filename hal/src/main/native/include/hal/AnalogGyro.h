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
 * @param handle handle to the analog port
 * @return       the initialized gyro handle
 */
HAL_GyroHandle HAL_InitializeAnalogGyro(HAL_AnalogInputHandle handle,
                                        int32_t* status);

/**
 * Sets up an analog gyro with the proper offsets and settings for the KOP
 * analog gyro.
 *
 * @param handle the gyro handle
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
 * @param handle                  the gyro handle
 * @param voltsPerDegreePerSecond the gyro volts scaling
 * @param offset                  the gyro offset
 * @param center                  the gyro center
 */
void HAL_SetAnalogGyroParameters(HAL_GyroHandle handle,
                                 double voltsPerDegreePerSecond, double offset,
                                 int32_t center, int32_t* status);

/**
 * Sets the analog gyro volts per degrees per second scaling.
 *
 * @param handle                  the gyro handle
 * @param voltsPerDegreePerSecond the gyro volts scaling
 */
void HAL_SetAnalogGyroVoltsPerDegreePerSecond(HAL_GyroHandle handle,
                                              double voltsPerDegreePerSecond,
                                              int32_t* status);

/**
 * Resets the analog gyro value to 0.
 *
 * @param handle the gyro handle
 */
void HAL_ResetAnalogGyro(HAL_GyroHandle handle, int32_t* status);

/**
 * Calibrates the analog gyro.
 *
 * This happens by calculating the average value of the gyro over 5 seconds, and
 * setting that as the center. Note that this call blocks for 5 seconds to
 * perform this.
 *
 * @param handle the gyro handle
 */
void HAL_CalibrateAnalogGyro(HAL_GyroHandle handle, int32_t* status);

/**
 * Sets the deadband of the analog gyro.
 *
 * @param handle the gyro handle
 * @param volts  the voltage deadband
 */
void HAL_SetAnalogGyroDeadband(HAL_GyroHandle handle, double volts,
                               int32_t* status);

/**
 * Gets the gyro angle in degrees.
 *
 * @param handle the gyro handle
 * @return the gyro angle in degrees
 */
double HAL_GetAnalogGyroAngle(HAL_GyroHandle handle, int32_t* status);

/**
 * Gets the gyro rate in degrees/second.
 *
 * @param handle the gyro handle
 * @return the gyro rate in degrees/second
 */
double HAL_GetAnalogGyroRate(HAL_GyroHandle handle, int32_t* status);

/**
 * Gets the calibrated gyro offset.
 *
 * Can be used to not repeat a calibration but reconstruct the gyro object.
 *
 * @param handle the gyro handle
 * @return the gryo offset
 */
double HAL_GetAnalogGyroOffset(HAL_GyroHandle handle, int32_t* status);

/**
 * Gets the calibrated gyro center.
 *
 * Can be used to not repeat a calibration but reconstruct the gyro object.
 *
 * @param handle the gyro handle
 * @return the gyro center
 */
int32_t HAL_GetAnalogGyroCenter(HAL_GyroHandle handle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
