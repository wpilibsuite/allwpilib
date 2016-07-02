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
HalCompressorHandle initializeCompressor(uint8_t module, int32_t* status);
bool checkCompressorModule(uint8_t module);

bool getCompressor(HalCompressorHandle compressor_handle, int32_t* status);

void setClosedLoopControl(HalCompressorHandle compressor_handle, bool value,
                          int32_t* status);
bool getClosedLoopControl(HalCompressorHandle compressor_handle,
                          int32_t* status);

bool getPressureSwitch(HalCompressorHandle compressor_handle, int32_t* status);
float getCompressorCurrent(HalCompressorHandle compressor_handle,
                           int32_t* status);

bool getCompressorCurrentTooHighFault(HalCompressorHandle compressor_handle,
                                      int32_t* status);
bool getCompressorCurrentTooHighStickyFault(
    HalCompressorHandle compressor_handle, int32_t* status);
bool getCompressorShortedStickyFault(HalCompressorHandle compressor_handle,
                                     int32_t* status);
bool getCompressorShortedFault(HalCompressorHandle compressor_handle,
                               int32_t* status);
bool getCompressorNotConnectedStickyFault(HalCompressorHandle compressor_handle,
                                          int32_t* status);
bool getCompressorNotConnectedFault(HalCompressorHandle compressor_handle,
                                    int32_t* status);
void clearAllPCMStickyFaults(HalCompressorHandle compressor_handle,
                             int32_t* status);
}
