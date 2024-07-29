// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"

/**
 * @defgroup hal_threads Threads Functions
 * @ingroup hal_capi
 * @{
 */

typedef const void* NativeThreadHandle;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the thread priority for the specified thread.
 *
 * @param[in] handle      Native handle pointer to the thread to get the
 *                        priority for.
 * @param[out] isRealTime Set to true if thread is real-time, otherwise false.
 * @param[out] status     Error status variable. 0 on success.
 * @return The current thread priority. For real-time, this is 1-99 with 99
 *         being highest. For non-real-time, this is 0. See "man 7 sched" for
 *         details.
 */
int32_t HAL_GetThreadPriority(NativeThreadHandle handle, HAL_Bool* isRealTime,
                              int32_t* status);

/**
 * Gets the thread priority for the current thread.
 *
 * @param[out] isRealTime Set to true if thread is real-time, otherwise false.
 * @param[out] status     Error status variable. 0 on success.
 * @return The current thread priority. For real-time, this is 1-99 with 99
 *         being highest. For non-real-time, this is 0. See "man 7 sched" for
 *         details.
 */
int32_t HAL_GetCurrentThreadPriority(HAL_Bool* isRealTime, int32_t* status);

/**
 * Sets the thread priority for the specified thread.
 *
 * @param[in] handle   Reference to the thread to set the priority of.
 * @param[in] realTime Set to true to set a real-time priority, false for
 *                     standard priority.
 * @param[in] priority Priority to set the thread to. For real-time, this is
 *                     1-99 with 99 being highest. For non-real-time, this is
 *                     forced to 0. See "man 7 sched" for more details.
 * @param[out] status  Error status variable. 0 on success.
 * @return True on success.
 */
HAL_Bool HAL_SetThreadPriority(NativeThreadHandle handle, HAL_Bool realTime,
                               int32_t priority, int32_t* status);

/**
 * Sets the thread priority for the current thread.
 *
 * @param[in] realTime Set to true to set a real-time priority, false for
 *                     standard priority.
 * @param[in] priority Priority to set the thread to. For real-time, this is
 *                     1-99 with 99 being highest. For non-real-time, this is
 *                     forced to 0. See "man 7 sched" for more details.
 * @param[out] status  Error status variable. 0 on success.
 * @return True on success.
 */
HAL_Bool HAL_SetCurrentThreadPriority(HAL_Bool realTime, int32_t priority,
                                      int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
