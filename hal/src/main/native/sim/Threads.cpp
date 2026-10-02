// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Threads.h"

#include <stdint.h>

#include "wpi/hal/Types.h"

namespace wpi::hal::init {
void InitializeThreads() {}
}  // namespace wpi::hal::init

HAL_Status HAL_GetThreadPriority(NativeThreadHandle handle, int32_t* priority) {
  *priority = 0;
  return 0;
}

HAL_Status HAL_GetCurrentThreadPriority(int32_t* priority) {
  *priority = 0;
  return 0;
}

HAL_Status HAL_SetThreadPriority(NativeThreadHandle handle, int32_t priority) {
  return 0;
}

HAL_Status HAL_SetCurrentThreadPriority(int32_t priority) {
  return 0;
}
