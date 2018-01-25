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

HAL_DigitalHandle HAL_InitializePWMPort(HAL_PortHandle portHandle,
                                        int32_t* status);
void HAL_FreePWMPort(HAL_DigitalHandle pwmPortHandle, int32_t* status);

HAL_Bool HAL_CheckPWMChannel(int32_t channel);

void HAL_SetPWMConfig(HAL_DigitalHandle pwmPortHandle, double maxPwm,
                      double deadbandMaxPwm, double centerPwm,
                      double deadbandMinPwm, double minPwm, int32_t* status);
void HAL_SetPWMConfigRaw(HAL_DigitalHandle pwmPortHandle, int32_t maxPwm,
                         int32_t deadbandMaxPwm, int32_t centerPwm,
                         int32_t deadbandMinPwm, int32_t minPwm,
                         int32_t* status);
void HAL_GetPWMConfigRaw(HAL_DigitalHandle pwmPortHandle, int32_t* maxPwm,
                         int32_t* deadbandMaxPwm, int32_t* centerPwm,
                         int32_t* deadbandMinPwm, int32_t* minPwm,
                         int32_t* status);
void HAL_SetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                 HAL_Bool eliminateDeadband, int32_t* status);
HAL_Bool HAL_GetPWMEliminateDeadband(HAL_DigitalHandle pwmPortHandle,
                                     int32_t* status);
void HAL_SetPWMRaw(HAL_DigitalHandle pwmPortHandle, int32_t value,
                   int32_t* status);
void HAL_SetPWMSpeed(HAL_DigitalHandle pwmPortHandle, double speed,
                     int32_t* status);
void HAL_SetPWMPosition(HAL_DigitalHandle pwmPortHandle, double position,
                        int32_t* status);
void HAL_SetPWMDisabled(HAL_DigitalHandle pwmPortHandle, int32_t* status);
int32_t HAL_GetPWMRaw(HAL_DigitalHandle pwmPortHandle, int32_t* status);
double HAL_GetPWMSpeed(HAL_DigitalHandle pwmPortHandle, int32_t* status);
double HAL_GetPWMPosition(HAL_DigitalHandle pwmPortHandle, int32_t* status);
void HAL_LatchPWMZero(HAL_DigitalHandle pwmPortHandle, int32_t* status);
void HAL_SetPWMPeriodScale(HAL_DigitalHandle pwmPortHandle, int32_t squelchMask,
                           int32_t* status);
int32_t HAL_GetPWMLoopTiming(int32_t* status);
uint64_t HAL_GetPWMCycleStartTime(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
