/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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

typedef void (*HAL_NotifierProcessFunction)(uint64_t currentTime,
                                            HAL_NotifierHandle handle);

HAL_NotifierHandle HAL_InitializeNotifier(HAL_NotifierProcessFunction process,
                                          void* param, int32_t* status);
HAL_NotifierHandle HAL_InitializeNotifierThreaded(
    HAL_NotifierProcessFunction process, void* param, int32_t* status);
void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle, int32_t* status);
void* HAL_GetNotifierParam(HAL_NotifierHandle notifierHandle, int32_t* status);
void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status);
void HAL_StopNotifierAlarm(HAL_NotifierHandle notifierHandle, int32_t* status);
#ifdef __cplusplus
}
#endif
