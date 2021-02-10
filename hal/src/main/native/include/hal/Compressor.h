// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_compressor Compressor Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a compressor on the given PCM module.
 *
 * @param module the module number
 * @return       the created handle
 */
HAL_CompressorHandle HAL_InitializeCompressor(int32_t module, int32_t* status);

/**
 * Gets if a compressor module is valid.
 *
 * @param module the module number
 * @return       true if the module is valid, otherwise false
 */
HAL_Bool HAL_CheckCompressorModule(int32_t module);

/**
 * Gets the compressor state (on or off).
 *
 * @param compressorHandle the compressor handle
 * @return                 true if the compressor is on, otherwise false
 */
HAL_Bool HAL_GetCompressor(HAL_CompressorHandle compressorHandle,
                           int32_t* status);

/**
 * Sets the compressor to closed loop mode.
 *
 * @param compressorHandle the compressor handle
 * @param value            true for closed loop mode, false for off
 */
void HAL_SetCompressorClosedLoopControl(HAL_CompressorHandle compressorHandle,
                                        HAL_Bool value, int32_t* status);

/**
 * Gets if the compressor is in closed loop mode.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if the compressor is in closed loop mode,
 * otherwise false
 */
HAL_Bool HAL_GetCompressorClosedLoopControl(
    HAL_CompressorHandle compressorHandle, int32_t* status);

/**
 * Gets the compressor pressure switch state.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if the pressure switch is triggered, otherwise
 * false
 */
HAL_Bool HAL_GetCompressorPressureSwitch(HAL_CompressorHandle compressorHandle,
                                         int32_t* status);

/**
 * Gets the compressor current.
 *
 * @param compressorHandle the compressor handle
 * @return                 the compressor current in amps
 */
double HAL_GetCompressorCurrent(HAL_CompressorHandle compressorHandle,
                                int32_t* status);

/**
 * Gets if the compressor is faulted because of too high of current.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if falted, otherwise false
 */
HAL_Bool HAL_GetCompressorCurrentTooHighFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);

/**
 * Gets if a sticky fauly is triggered because of too high of current.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if falted, otherwise false
 */
HAL_Bool HAL_GetCompressorCurrentTooHighStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);

/**
 * Gets if a sticky fauly is triggered because of a short.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if falted, otherwise false
 */
HAL_Bool HAL_GetCompressorShortedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);

/**
 * Gets if the compressor is faulted because of a short.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if shorted, otherwise false
 */
HAL_Bool HAL_GetCompressorShortedFault(HAL_CompressorHandle compressorHandle,
                                       int32_t* status);

/**
 * Gets if a sticky fault is triggered of the compressor not connected.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if falted, otherwise false
 */
HAL_Bool HAL_GetCompressorNotConnectedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);

/**
 * Gets if the compressor is not connected.
 *
 * @param compressorHandle the compressor handle
 * @return                 true if not connected, otherwise false
 */
HAL_Bool HAL_GetCompressorNotConnectedFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
