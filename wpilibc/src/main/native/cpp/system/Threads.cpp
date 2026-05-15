// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Threads.hpp"

#include "wpi/hal/Threads.h"
#include "wpi/system/Errors.hpp"

namespace wpi {

int GetThreadPriority(std::thread& thread) {
  auto native = thread.native_handle();
  int32_t priority = 0;
  HAL_Status status = HAL_GetThreadPriority(&native, &priority);
  WPILIB_CheckErrorStatus(status, "GetThreadPriority");
  return priority;
}

int GetCurrentThreadPriority() {
  int32_t priority = 0;
  HAL_Status status = HAL_GetCurrentThreadPriority(&priority);
  WPILIB_CheckErrorStatus(status, "GetCurrentThreadPriority");
  return priority;
}

bool SetThreadPriority(std::thread& thread, int priority) {
  auto native = thread.native_handle();
  HAL_Status status = HAL_SetThreadPriority(&native, priority);
  WPILIB_CheckErrorStatus(status, "SetThreadPriority");
  return status != 0;
}

bool SetCurrentThreadPriority(int priority) {
  HAL_Status status = HAL_SetCurrentThreadPriority(priority);
  WPILIB_CheckErrorStatus(status, "SetCurrentThreadPriority");
  return status != 0;
}

}  // namespace wpi
