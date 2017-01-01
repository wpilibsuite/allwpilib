/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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

HAL_CompressorHandle HAL_InitializeCompressor(int32_t module, int32_t* status);
HAL_Bool HAL_CheckCompressorModule(int32_t module);

HAL_Bool HAL_GetCompressor(HAL_CompressorHandle compressorHandle,
                           int32_t* status);

void HAL_SetCompressorClosedLoopControl(HAL_CompressorHandle compressorHandle,
                                        HAL_Bool value, int32_t* status);
HAL_Bool HAL_GetCompressorClosedLoopControl(
    HAL_CompressorHandle compressorHandle, int32_t* status);

HAL_Bool HAL_GetCompressorPressureSwitch(HAL_CompressorHandle compressorHandle,
                                         int32_t* status);
double HAL_GetCompressorCurrent(HAL_CompressorHandle compressorHandle,
                                int32_t* status);

HAL_Bool HAL_GetCompressorCurrentTooHighFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);
HAL_Bool HAL_GetCompressorCurrentTooHighStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);
HAL_Bool HAL_GetCompressorShortedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);
HAL_Bool HAL_GetCompressorShortedFault(HAL_CompressorHandle compressorHandle,
                                       int32_t* status);
HAL_Bool HAL_GetCompressorNotConnectedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);
HAL_Bool HAL_GetCompressorNotConnectedFault(
    HAL_CompressorHandle compressorHandle, int32_t* status);
#ifdef __cplusplus
}
#endif
