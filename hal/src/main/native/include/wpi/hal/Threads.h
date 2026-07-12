// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

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
 * Gets the specified thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @param[in] handle Native thread handle.
 * @param[out] priority The specified thread's priority.
 * @return Error status variable. 0 on success.
 */
HAL_Status HAL_GetThreadPriority(NativeThreadHandle handle, int32_t* priority);

/**
 * Gets the current thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @param[out] priority The current thread's priority.
 * @return Error status variable. 0 on success.
 */
HAL_Status HAL_GetCurrentThreadPriority(int32_t* priority);

/**
 * Sets the specified thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @param[in] handle The native thread handle.
 * @param[in] priority The priority.
 * @return Error status variable. 0 on success.
 */
HAL_Status HAL_SetThreadPriority(NativeThreadHandle handle, int32_t priority);

/**
 * Sets the current thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @param[in] priority The priority.
 * @return Error status variable. 0 on success.
 */
HAL_Status HAL_SetCurrentThreadPriority(int32_t priority);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
