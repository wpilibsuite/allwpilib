/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Threads.h"

#include <pthread.h>
#include <sched.h>

#include "HAL/Errors.h"

extern "C" {
/**
 * Get the thread priority for the specified thread.
 *
 * @param handle Native handle pointer to the thread to get the priority for
 * @param isRealTime Set to true if thread is realtime, otherwise false
 * @param status Error status variable. 0 on success
 * @return The current thread priority. Scaled 1-99, with 1 being highest.
 */
int32_t HAL_GetThreadPriority(NativeThreadHandle handle, HAL_Bool* isRealTime,
                              int32_t* status) {
  sched_param sch;
  int policy;
  int success = pthread_getschedparam(*handle, &policy, &sch);
  if (success == 0) {
    *status = 0;
  } else {
    *status = HAL_THREAD_PRIORITY_ERROR;
    return -1;
  }
  if (policy == SCHED_FIFO || policy == SCHED_RR) {
    *isRealTime = true;
    return sch.sched_priority;
  } else {
    *isRealTime = false;
    // 0 is the only suppored priority for non-realtime, so scale to 1
    return 1;
  }
}

/**
 * Get the thread priority for the current thread.
 *
 * @param handle Native handle pointer to the thread to get the priority for
 * @param isRealTime Set to true if thread is realtime, otherwise false
 * @param status Error status variable. 0 on success
 * @return The current thread priority. Scaled 1-99, with 1 being highest.
 */
int32_t HAL_GetCurrentThreadPriority(HAL_Bool* isRealTime, int32_t* status) {
  auto thread = pthread_self();
  return HAL_GetThreadPriority(&thread, isRealTime, status);
}

/**
 * Sets the thread priority for the specified thread
 *
 * @param thread Reference to the thread to set the priority of
 * @param realTime Set to true to set a realtime priority, false for standard
 * priority
 * @param priority Priority to set the thread to. Scaled 1-99, with 1 being
 * highest
 * @param status Error status variable. 0 on success
 *
 * @return The success state of setting the priority
 */
HAL_Bool HAL_SetThreadPriority(NativeThreadHandle handle, HAL_Bool realTime,
                               int32_t priority, int32_t* status) {
  if (handle == nullptr) {
    *status = NULL_PARAMETER;
    return false;
  }

  int scheduler = realTime ? SCHED_FIFO : SCHED_OTHER;
  if (realTime) {
    // We don't support setting priorities for non RT threads
    // so we don't need to check for proper range
    if (priority < sched_get_priority_min(scheduler) ||
        priority > sched_get_priority_max(scheduler)) {
      *status = HAL_THREAD_PRIORITY_RANGE_ERROR;
      return false;
    }
  }

  sched_param sch;
  int policy;
  pthread_getschedparam(*handle, &policy, &sch);
  if (scheduler == SCHED_FIFO || scheduler == SCHED_RR)
    sch.sched_priority = priority;
  else
    // Only need to set 0 priority for non RT thread
    sch.sched_priority = 0;
  if (pthread_setschedparam(*handle, scheduler, &sch)) {
    *status = HAL_THREAD_PRIORITY_ERROR;
    return false;
  } else {
    *status = 0;
    return true;
  }
}

/**
 * Sets the thread priority for the current thread
 *
 * @param thread Reference to the thread to set the priority of
 * @param realTime Set to true to set a realtime priority, false for standard
 * priority
 * @param priority Priority to set the thread to. Scaled 1-99, with 1 being
 * highest
 * @param status Error status variable. 0 on success
 *
 * @return The success state of setting the priority
 */
HAL_Bool HAL_SetCurrentThreadPriority(HAL_Bool realTime, int32_t priority,
                                      int32_t* status) {
  auto thread = pthread_self();
  return HAL_SetThreadPriority(&thread, realTime, priority, status);
}
}
