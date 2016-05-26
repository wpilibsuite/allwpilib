/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <stdint.h>

#pragma once

extern "C" {
void* initializeCompressor(uint8_t module);
bool checkCompressorModule(uint8_t module);

bool getCompressor(void* pcm_pointer, int32_t* status);

void setClosedLoopControl(void* pcm_pointer, bool value, int32_t* status);
bool getClosedLoopControl(void* pcm_pointer, int32_t* status);

bool getPressureSwitch(void* pcm_pointer, int32_t* status);
float getCompressorCurrent(void* pcm_pointer, int32_t* status);

bool getCompressorCurrentTooHighFault(void* pcm_pointer, int32_t* status);
bool getCompressorCurrentTooHighStickyFault(void* pcm_pointer, int32_t* status);
bool getCompressorShortedStickyFault(void* pcm_pointer, int32_t* status);
bool getCompressorShortedFault(void* pcm_pointer, int32_t* status);
bool getCompressorNotConnectedStickyFault(void* pcm_pointer, int32_t* status);
bool getCompressorNotConnectedFault(void* pcm_pointer, int32_t* status);
void clearAllPCMStickyFaults(void* pcm_pointer, int32_t* status);
}
