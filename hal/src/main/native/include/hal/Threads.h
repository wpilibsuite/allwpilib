/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#define NativeThreadHandle const void*

#include "hal/Types.h"

/**
 * @defgroup hal_threads Threads Functions
 * @ingroup hal_capi
 * @{
 */

extern "C" {
/**
 * Gets the thread priority for the specified thread.
 *
 * @param handle     Native handle pointer to the thread to get the priority for
 * @param isRealTime Set to true if thread is realtime, otherwise false
 * @param status     Error status variable. 0 on success
 * @return           The current thread priority. Scaled 1-99, with 1 being
 * highest.
 */
int32_t HAL_GetThreadPriority(NativeThreadHandle handle, HAL_Bool* isRealTime,
                              int32_t* status);

/**
 * Gets the thread priority for the current thread.
 *
 * @param handle     Native handle pointer to the thread to get the priority for
 * @param isRealTime Set to true if thread is realtime, otherwise false
 * @param status     Error status variable. 0 on success
 * @return           The current thread priority. Scaled 1-99, with 1 being
 * highest.
 */
int32_t HAL_GetCurrentThreadPriority(HAL_Bool* isRealTime, int32_t* status);

/**
 * Sets the thread priority for the specified thread.
 *
 * @param thread   Reference to the thread to set the priority of
 * @param realTime Set to true to set a realtime priority, false for standard
 * priority
 * @param priority Priority to set the thread to. Scaled 1-99, with 1 being
 * highest
 * @param status   Error status variable. 0 on success
 * @return         The success state of setting the priority
 */
HAL_Bool HAL_SetThreadPriority(NativeThreadHandle handle, HAL_Bool realTime,
                               int32_t priority, int32_t* status);

/**
 * Sets the thread priority for the current thread.
 *
 * @param thread   Reference to the thread to set the priority of
 * @param realTime Set to true to set a realtime priority, false for standard
 * priority
 * @param priority Priority to set the thread to. Scaled 1-99, with 1 being
 * highest
 * @param status   Error status variable. 0 on success
 * @return         The success state of setting the priority
 */
HAL_Bool HAL_SetCurrentThreadPriority(HAL_Bool realTime, int32_t priority,
                                      int32_t* status);
}  // extern "C"
/** @} */
