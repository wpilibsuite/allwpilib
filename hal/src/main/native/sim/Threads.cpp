/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Threads.h"

namespace hal {
namespace init {
void InitializeThreads() {}
}  // namespace init
}  // namespace hal

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
