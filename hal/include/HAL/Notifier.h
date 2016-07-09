/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

extern "C" {
HAL_NotifierHandle HAL_InitializeNotifier(void (*process)(uint64_t, void*),
                                          void* param, int32_t* status);
void HAL_CleanNotifier(HAL_NotifierHandle notifier_handle, int32_t* status);
void* HAL_GetNotifierParam(HAL_NotifierHandle notifier_handle, int32_t* status);
void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifier_handle,
                             uint64_t triggerTime, int32_t* status);
void HAL_StopNotifierAlarm(HAL_NotifierHandle notifier_handle, int32_t* status);
}
