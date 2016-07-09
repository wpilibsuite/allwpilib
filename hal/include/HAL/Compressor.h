/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Handles.h"

extern "C" {
HAL_CompressorHandle HAL_InitializeCompressor(uint8_t module, int32_t* status);
bool HAL_CheckCompressorModule(uint8_t module);

bool HAL_GetCompressor(HAL_CompressorHandle compressor_handle, int32_t* status);

void HAL_SetClosedLoopControl(HAL_CompressorHandle compressor_handle,
                              bool value, int32_t* status);
bool HAL_GetClosedLoopControl(HAL_CompressorHandle compressor_handle,
                              int32_t* status);

bool HAL_GetPressureSwitch(HAL_CompressorHandle compressor_handle,
                           int32_t* status);
float HAL_GetCompressorCurrent(HAL_CompressorHandle compressor_handle,
                               int32_t* status);

bool HAL_GetCompressorCurrentTooHighFault(
    HAL_CompressorHandle compressor_handle, int32_t* status);
bool HAL_GetCompressorCurrentTooHighStickyFault(
    HAL_CompressorHandle compressor_handle, int32_t* status);
bool HAL_GetCompressorShortedStickyFault(HAL_CompressorHandle compressor_handle,
                                         int32_t* status);
bool HAL_GetCompressorShortedFault(HAL_CompressorHandle compressor_handle,
                                   int32_t* status);
bool HAL_GetCompressorNotConnectedStickyFault(
    HAL_CompressorHandle compressor_handle, int32_t* status);
bool HAL_GetCompressorNotConnectedFault(HAL_CompressorHandle compressor_handle,
                                        int32_t* status);
void HAL_ClearAllPCMStickyFaults(HAL_CompressorHandle compressor_handle,
                                 int32_t* status);
}
