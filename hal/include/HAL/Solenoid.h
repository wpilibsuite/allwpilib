/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

extern "C" {
HAL_SolenoidHandle HAL_InitializeSolenoidPort(HAL_PortHandle port_handle,
                                              int32_t* status);
void HAL_FreeSolenoidPort(HAL_SolenoidHandle solenoid_port_handle);
HAL_Bool HAL_CheckSolenoidModule(int32_t module);
HAL_Bool HAL_CheckSolenoidPin(int32_t pin);
HAL_Bool HAL_GetSolenoid(HAL_SolenoidHandle solenoid_port_handle,
                         int32_t* status);
int32_t HAL_GetAllSolenoids(int32_t module, int32_t* status);
void HAL_SetSolenoid(HAL_SolenoidHandle solenoid_port_handle, HAL_Bool value,
                     int32_t* status);
int32_t HAL_GetPCMSolenoidBlackList(int32_t module, int32_t* status);
HAL_Bool HAL_GetPCMSolenoidVoltageStickyFault(int32_t module, int32_t* status);
HAL_Bool HAL_GetPCMSolenoidVoltageFault(int32_t module, int32_t* status);
void HAL_ClearAllPCMStickyFaults(int32_t module, int32_t* status);
}
