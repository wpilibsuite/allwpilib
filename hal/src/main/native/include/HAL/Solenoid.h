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

HAL_SolenoidHandle HAL_InitializeSolenoidPort(HAL_PortHandle portHandle,
                                              int32_t* status);
void HAL_FreeSolenoidPort(HAL_SolenoidHandle solenoidPortHandle);
HAL_Bool HAL_CheckSolenoidModule(int32_t module);
HAL_Bool HAL_CheckSolenoidChannel(int32_t channel);
HAL_Bool HAL_GetSolenoid(HAL_SolenoidHandle solenoidPortHandle,
                         int32_t* status);
int32_t HAL_GetAllSolenoids(int32_t module, int32_t* status);
void HAL_SetSolenoid(HAL_SolenoidHandle solenoidPortHandle, HAL_Bool value,
                     int32_t* status);
void HAL_SetAllSolenoids(int32_t module, int32_t state, int32_t* status);
int32_t HAL_GetPCMSolenoidBlackList(int32_t module, int32_t* status);
HAL_Bool HAL_GetPCMSolenoidVoltageStickyFault(int32_t module, int32_t* status);
HAL_Bool HAL_GetPCMSolenoidVoltageFault(int32_t module, int32_t* status);
void HAL_ClearAllPCMStickyFaults(int32_t module, int32_t* status);
void HAL_SetOneShotDuration(HAL_SolenoidHandle solenoidPortHandle,
                            int32_t durMS, int32_t* status);
void HAL_FireOneShot(HAL_SolenoidHandle solenoidPortHandle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
