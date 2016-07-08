/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "cpp/priority_condition_variable.h"
#include "cpp/priority_mutex.h"

typedef priority_mutex* MUTEX_ID;
typedef priority_condition_variable* MULTIWAIT_ID;
typedef priority_condition_variable::native_handle_type NATIVE_MULTIWAIT_ID;

extern "C" {
MUTEX_ID HAL_InitializeMutexNormal();
void HAL_DeleteMutex(MUTEX_ID sem);
void HAL_TakeMutex(MUTEX_ID sem);
bool HAL_TryTakeMutex(MUTEX_ID sem);
void HAL_GiveMutex(MUTEX_ID sem);

MULTIWAIT_ID HAL_InitializeMultiWait();
void HAL_DeleteMultiWait(MULTIWAIT_ID sem);
void HAL_TakeMultiWait(MULTIWAIT_ID sem, MUTEX_ID m);
void HAL_GiveMultiWait(MULTIWAIT_ID sem);
}
