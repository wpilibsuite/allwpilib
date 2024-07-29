// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Threads.h"

namespace hal::init {
void InitializeThreads() {}
}  // namespace hal::init

int32_t HAL_GetThreadPriority(NativeThreadHandle handle, HAL_Bool* isRealTime,
                              int32_t* status) {
  return 0;
}
int32_t HAL_GetCurrentThreadPriority(HAL_Bool* isRealTime, int32_t* status) {
  return 0;
}
HAL_Bool HAL_SetThreadPriority(NativeThreadHandle handle, HAL_Bool realTime,
                               int32_t priority, int32_t* status) {
  return true;
}
HAL_Bool HAL_SetCurrentThreadPriority(HAL_Bool realTime, int32_t priority,
                                      int32_t* status) {
  return true;
}
