/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Threads.h"

#include <hal/HAL.h>
#include <hal/Threads.h>

#include "frc/ErrorBase.h"

using namespace frc;

int GetThreadPriority(std::thread& thread, bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto native = thread.native_handle();
  auto ret = HAL_GetThreadPriority(&native, &rt, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  *isRealTime = rt;
  return ret;
}

int GetCurrentThreadPriority(bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto ret = HAL_GetCurrentThreadPriority(&rt, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  *isRealTime = rt;
  return ret;
}

bool SetThreadPriority(std::thread& thread, bool realTime, int priority) {
  int32_t status = 0;
  auto native = thread.native_handle();
  auto ret = HAL_SetThreadPriority(&native, realTime, priority, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return ret;
}

bool SetCurrentThreadPriority(bool realTime, int priority) {
  int32_t status = 0;
  auto ret = HAL_SetCurrentThreadPriority(realTime, priority, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return ret;
}
