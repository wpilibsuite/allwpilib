/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void HAL_InitializePDP(int32_t module);
double HAL_GetPDPTemperature(int32_t module, int32_t* status);
double HAL_GetPDPVoltage(int32_t module, int32_t* status);
double HAL_GetPDPChannelCurrent(int32_t module, int32_t channel,
                                int32_t* status);
double HAL_GetPDPTotalCurrent(int32_t module, int32_t* status);
double HAL_GetPDPTotalPower(int32_t module, int32_t* status);
double HAL_GetPDPTotalEnergy(int32_t module, int32_t* status);
void HAL_ResetPDPTotalEnergy(int32_t module, int32_t* status);
void HAL_ClearPDPStickyFaults(int32_t module, int32_t* status);
}
