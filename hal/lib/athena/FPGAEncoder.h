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
HalFPGAEncoderHandle initializeFPGAEncoder(
    uint8_t port_a_module, uint32_t port_a_pin, bool port_a_analog_trigger,
    uint8_t port_b_module, uint32_t port_b_pin, bool port_b_analog_trigger,
    bool reverseDirection, int32_t* index,
    int32_t* status);  // TODO: fix routing
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
                               uint32_t pin, bool analogTrigger,
                               bool activeHigh, bool edgeSensitive,
                               int32_t* status);
}
