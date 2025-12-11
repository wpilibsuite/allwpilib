// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <string_view>
#endif

#include "wpi/hal/Types.h"
#include "wpi/util/string.h"

/**
 * @defgroup hal_notifier Notifier Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a notifier.
 *
 * A notifier is an timer that alarms at an initial and (optionally) repeated
 * intervals. This can be used to make precise control loops.
 *
 * @param[out] status Error status variable. 0 on success.
 * @return the created notifier
 */
HAL_NotifierHandle HAL_CreateNotifier(int32_t* status);

/**
 * Sets the HAL notifier thread priority.
 *
 * The HAL notifier thread is responsible for managing the system's notifier
 * interrupt and waking up user's Notifiers when it's their time to run.
 * Giving the HAL notifier thread real-time priority helps ensure the user's
 * real-time Notifiers, if any, are notified to run in a timely manner.
 *
 * @param[in] realTime Set to true to set a real-time priority, false for
 *                     standard priority.
 * @param[in] priority Priority to set the thread to. For real-time, this is
 *                     1-99 with 99 being highest. For non-real-time, this is
 *                     forced to 0. See "man 7 sched" for more details.
 * @param[out] status  Error status variable. 0 on success.
 * @return True on success.
 */
HAL_Bool HAL_SetNotifierThreadPriority(HAL_Bool realTime, int32_t priority,
                                       int32_t* status);

/**
 * Sets the name of a notifier.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[in] name name
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetNotifierName(HAL_NotifierHandle notifierHandle,
                         const struct WPI_String* name, int32_t* status);

/**
 * Destroys a notifier.
 *
 * Destruction wakes up any waiters.
 *
 * @param[in] notifierHandle the notifier handle
 */
void HAL_DestroyNotifier(HAL_NotifierHandle notifierHandle);

/**
 * Updates the initial and interval alarm times for a notifier.
 *
 * The alarmTime is an absolute time (using the WPI_Now() time base) if
 * absolute is true, or relative to the current time if absolute is false.
 *
 * If intervalTime is non-zero, the notifier will alarm periodically following
 * alarmTime at the given interval.
 *
 * If an absolute alarmTime is in the past, the notifier will alarm immediately.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[in] alarmTime      the first alarm time (in microseconds)
 * @param[in] intervalTime   the periodic interval time (in microseconds)
 * @param[in] absolute       true if the alarm time is absolute
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetNotifierAlarm(HAL_NotifierHandle notifierHandle, uint64_t alarmTime,
                          uint64_t intervalTime, HAL_Bool absolute,
                          int32_t* status);

/**
 * Cancels all future notifier alarms for a notifier.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status);

/**
 * Indicates the notifier alarm has been serviced and optionally sets a new
 * alarm time. This must be called before waiting for the next alarm.
 *
 * The alarmTime is an absolute time (using the WPI_Now() time base) if
 * absolute is true, or relative to the current time if absolute is false.
 *
 * If intervalTime is non-zero, the notifier will alarm periodically following
 * alarmTime at the given interval.
 *
 * If an absolute alarmTime is in the past, the notifier will alarm immediately.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[in] setAlarm       true to set a new alarm time, false to leave the
 *                           alarm unchanged
 * @param[in] alarmTime      the first alarm time (in microseconds)
 * @param[in] intervalTime   the periodic interval time (in microseconds)
 * @param[in] absolute       true if the alarm time is absolute
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AcknowledgeNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  HAL_Bool setAlarm, uint64_t alarmTime,
                                  uint64_t intervalTime, HAL_Bool absolute,
                                  int32_t* status);

/**
 * Gets the overrun count for a notifier.
 *
 * An overrun occurs when a notifier's alarm is not serviced before the next
 * scheduled alarm time.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[out] status Error status variable. 0 on success.
 * @return overrun count
 */
int32_t HAL_GetNotifierOverrun(HAL_NotifierHandle notifierHandle,
                               int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

#ifdef __cplusplus
/**
 * Sets the name of a notifier.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[in] name name
 * @param[out] status Error status variable. 0 on success.
 */
inline void HAL_SetNotifierName(HAL_NotifierHandle notifierHandle,
                                std::string_view name, int32_t* status) {
  WPI_String nameStr = wpi::util::make_string(name);
  HAL_SetNotifierName(notifierHandle, &nameStr, status);
}
#endif
