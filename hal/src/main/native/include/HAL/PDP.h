/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
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

HAL_PDPHandle HAL_InitializePDP(int32_t module, int32_t* status);
void HAL_CleanPDP(HAL_PDPHandle handle);
HAL_Bool HAL_CheckPDPChannel(int32_t channel);
HAL_Bool HAL_CheckPDPModule(int32_t module);
double HAL_GetPDPTemperature(HAL_PDPHandle handle, int32_t* status);
double HAL_GetPDPVoltage(HAL_PDPHandle handle, int32_t* status);
double HAL_GetPDPChannelCurrent(HAL_PDPHandle handle, int32_t channel,
                                int32_t* status);
double HAL_GetPDPTotalCurrent(HAL_PDPHandle handle, int32_t* status);
double HAL_GetPDPTotalPower(HAL_PDPHandle handle, int32_t* status);
double HAL_GetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status);
void HAL_ResetPDPTotalEnergy(HAL_PDPHandle handle, int32_t* status);
void HAL_ClearPDPStickyFaults(HAL_PDPHandle handle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
