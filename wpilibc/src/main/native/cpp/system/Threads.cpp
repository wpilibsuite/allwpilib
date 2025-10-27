// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Threads.hpp"

#include <wpi/hal/Threads.h>

#include "wpi/system/Errors.hpp"

namespace wpi {

int GetThreadPriority(std::thread& thread, bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto native = thread.native_handle();
  auto ret = HAL_GetThreadPriority(&native, &rt, &status);
  WPILIB_CheckErrorStatus(status, "GetThreadPriority");
  *isRealTime = rt;
  return ret;
}

int GetCurrentThreadPriority(bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto ret = HAL_GetCurrentThreadPriority(&rt, &status);
  WPILIB_CheckErrorStatus(status, "GetCurrentThreadPriority");
  *isRealTime = rt;
  return ret;
}

bool SetThreadPriority(std::thread& thread, bool realTime, int priority) {
  int32_t status = 0;
  auto native = thread.native_handle();
  auto ret = HAL_SetThreadPriority(&native, realTime, priority, &status);
  WPILIB_CheckErrorStatus(status, "SetThreadPriority");
  return ret;
}

bool SetCurrentThreadPriority(bool realTime, int priority) {
  int32_t status = 0;
  auto ret = HAL_SetCurrentThreadPriority(realTime, priority, &status);
  WPILIB_CheckErrorStatus(status, "SetCurrentThreadPriority");
  return ret;
}

}  // namespace wpi
