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

HAL_NotifierHandle HAL_InitializeNotifier(int32_t* status);
void HAL_StopNotifier(HAL_NotifierHandle notifierHandle, int32_t* status);
void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle, int32_t* status);
void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status);
void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status);
uint64_t HAL_WaitForNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
