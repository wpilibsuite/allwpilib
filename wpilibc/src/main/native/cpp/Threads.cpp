// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Threads.h"

#include <hal/FRCUsageReporting.h>
#include <hal/Threads.h>

#include "frc/ErrorBase.h"

namespace frc {

int GetThreadPriority(std::thread& thread, bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto native = thread.native_handle();
  auto ret = HAL_GetThreadPriority(&native, &rt, &status);
  wpi_setGlobalHALError(status);
  *isRealTime = rt;
  return ret;
}

int GetCurrentThreadPriority(bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto ret = HAL_GetCurrentThreadPriority(&rt, &status);
  wpi_setGlobalHALError(status);
  *isRealTime = rt;
  return ret;
}

bool SetThreadPriority(std::thread& thread, bool realTime, int priority) {
  int32_t status = 0;
  auto native = thread.native_handle();
  auto ret = HAL_SetThreadPriority(&native, realTime, priority, &status);
  wpi_setGlobalHALError(status);
  return ret;
}

bool SetCurrentThreadPriority(bool realTime, int priority) {
  int32_t status = 0;
  auto ret = HAL_SetCurrentThreadPriority(realTime, priority, &status);
  wpi_setGlobalHALError(status);
  return ret;
}

}  // namespace frc
