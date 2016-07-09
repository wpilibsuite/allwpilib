/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/AnalogTrigger.h"
#include "HAL/Handles.h"

extern "C" {
HAL_FPGAEncoderHandle HAL_InitializeFPGAEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    bool reverseDirection, int32_t* index, int32_t* status);
void HAL_FreeFPGAEncoder(HAL_FPGAEncoderHandle fpga_encoder_handle,
                         int32_t* status);
void HAL_ResetFPGAEncoder(HAL_FPGAEncoderHandle fpga_encoder_handle,
                          int32_t* status);
int32_t HAL_GetFPGAEncoder(HAL_FPGAEncoderHandle fpga_encoder_handle,
                           int32_t* status);  // Raw value
double HAL_GetFPGAEncoderPeriod(HAL_FPGAEncoderHandle fpga_encoder_handle,
                                int32_t* status);
void HAL_SetFPGAEncoderMaxPeriod(HAL_FPGAEncoderHandle fpga_encoder_handle,
                                 double maxPeriod, int32_t* status);
bool HAL_GetFPGAEncoderStopped(HAL_FPGAEncoderHandle fpga_encoder_handle,
                               int32_t* status);
bool HAL_GetFPGAEncoderDirection(HAL_FPGAEncoderHandle fpga_encoder_handle,
                                 int32_t* status);
void HAL_SetFPGAEncoderReverseDirection(
    HAL_FPGAEncoderHandle fpga_encoder_handle, bool reverseDirection,
    int32_t* status);
void HAL_SetFPGAEncoderSamplesToAverage(
    HAL_FPGAEncoderHandle fpga_encoder_handle, uint32_t samplesToAverage,
    int32_t* status);
uint32_t HAL_GetFPGAEncoderSamplesToAverage(
    HAL_FPGAEncoderHandle fpga_encoder_handle, int32_t* status);
void HAL_SetFPGAEncoderIndexSource(HAL_FPGAEncoderHandle fpga_encoder_handle,
                                   HAL_Handle digitalSourceHandle,
                                   HAL_AnalogTriggerType analogTriggerType,
                                   bool activeHigh, bool edgeSensitive,
                                   int32_t* status);
}
