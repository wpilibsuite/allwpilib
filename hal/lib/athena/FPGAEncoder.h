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
HalFPGAEncoderHandle initializeFPGAEncoder(HalHandle digitalSourceHandleA,
                                           AnalogTriggerType analogTriggerTypeA,
                                           HalHandle digitalSourceHandleB,
                                           AnalogTriggerType analogTriggerTypeB,
                                           bool reverseDirection,
                                           int32_t* index, int32_t* status);
void freeFPGAEncoder(HalFPGAEncoderHandle fpga_encoder_handle, int32_t* status);
void resetFPGAEncoder(HalFPGAEncoderHandle fpga_encoder_handle,
                      int32_t* status);
int32_t getFPGAEncoder(HalFPGAEncoderHandle fpga_encoder_handle,
                       int32_t* status);  // Raw value
double getFPGAEncoderPeriod(HalFPGAEncoderHandle fpga_encoder_handle,
                            int32_t* status);
void setFPGAEncoderMaxPeriod(HalFPGAEncoderHandle fpga_encoder_handle,
                             double maxPeriod, int32_t* status);
bool getFPGAEncoderStopped(HalFPGAEncoderHandle fpga_encoder_handle,
                           int32_t* status);
bool getFPGAEncoderDirection(HalFPGAEncoderHandle fpga_encoder_handle,
                             int32_t* status);
void setFPGAEncoderReverseDirection(HalFPGAEncoderHandle fpga_encoder_handle,
                                    bool reverseDirection, int32_t* status);
void setFPGAEncoderSamplesToAverage(HalFPGAEncoderHandle fpga_encoder_handle,
                                    uint32_t samplesToAverage, int32_t* status);
uint32_t getFPGAEncoderSamplesToAverage(
    HalFPGAEncoderHandle fpga_encoder_handle, int32_t* status);
void setFPGAEncoderIndexSource(HalFPGAEncoderHandle fpga_encoder_handle,
                               HalHandle digitalSourceHandle,
                               AnalogTriggerType analogTriggerType,
                               bool activeHigh, bool edgeSensitive,
                               int32_t* status);
}
