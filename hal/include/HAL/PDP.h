/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void HAL_InitializePDP(uint8_t module);
double HAL_GetPDPTemperature(uint8_t module, int32_t* status);
double HAL_GetPDPVoltage(uint8_t module, int32_t* status);
double HAL_GetPDPChannelCurrent(uint8_t module, uint8_t channel,
                                int32_t* status);
double HAL_GetPDPTotalCurrent(uint8_t module, int32_t* status);
double HAL_GetPDPTotalPower(uint8_t module, int32_t* status);
double HAL_GetPDPTotalEnergy(uint8_t module, int32_t* status);
void HAL_ResetPDPTotalEnergy(uint8_t module, int32_t* status);
void HAL_ClearPDPStickyFaults(uint8_t module, int32_t* status);
}
