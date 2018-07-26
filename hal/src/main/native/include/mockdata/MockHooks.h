/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include "hal/Types.h"

extern "C" {
void HALSIM_WaitForProgramStart(void);
void HALSIM_SetProgramStarted(void);
HAL_Bool HALSIM_GetProgramStarted(void);
void HALSIM_RestartTiming(void);
}  // extern "C"

#endif
