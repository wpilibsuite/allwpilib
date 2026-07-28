// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Threads.h"

#include <stdint.h>

#include <pthread.h>
#include <sched.h>

#include "wpi/hal/Errors.h"
#include "wpi/hal/Types.h"

namespace wpi::hal::init {
void InitializeThreads() {}
}  // namespace wpi::hal::init

extern "C" {

HAL_Status HAL_GetThreadPriority(NativeThreadHandle handle, int32_t* priority) {
  if (handle == nullptr) {
    return HAL_NULL_PARAMETER;
  }

  sched_param sch;
  int policy;
  if (pthread_getschedparam(*reinterpret_cast<const pthread_t*>(handle),
                            &policy, &sch) == 0) {
    *priority = sch.sched_priority;
    return 0;
  } else {
    return HAL_THREAD_PRIORITY_ERROR;
  }
}

HAL_Status HAL_GetCurrentThreadPriority(int32_t* priority) {
  auto thread = pthread_self();
  return HAL_GetThreadPriority(&thread, priority);
}

HAL_Status HAL_SetThreadPriority(NativeThreadHandle handle, int32_t priority) {
  if (handle == nullptr) {
    return HAL_NULL_PARAMETER;
  }

  if (priority < 0 || priority > 99) {
    return HAL_THREAD_PRIORITY_RANGE_ERROR;
  }

  int scheduler = priority > 0 ? SCHED_RR : SCHED_OTHER;

  sched_param sch;
  int policy;
  pthread_getschedparam(*reinterpret_cast<const pthread_t*>(handle), &policy,
                        &sch);
  sch.sched_priority = priority;

  if (pthread_setschedparam(*reinterpret_cast<const pthread_t*>(handle),
                            scheduler, &sch)) {
    return HAL_THREAD_PRIORITY_ERROR;
  } else {
    return 0;
  }
}

HAL_Status HAL_SetCurrentThreadPriority(int32_t priority) {
  auto thread = pthread_self();
  return HAL_SetThreadPriority(&thread, priority);
}

}  // extern "C"
