// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

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
 * @param[out] notifierHandle The created notifier handle.
 * @return Error status. 0 on success.
 */
HAL_Status HAL_CreateNotifier(HAL_NotifierHandle* notifierHandle);

/**
 * Sets the name of a notifier.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[in] name name
 * @return Error status. 0 on success.
 */
HAL_Status HAL_SetNotifierName(HAL_NotifierHandle notifierHandle,
                               const struct WPI_String* name);

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
 * @param[in] ack            true to acknowledge any prior alarm
 * @return Error status. 0 on success.
 */
HAL_Status HAL_SetNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                uint64_t alarmTime, uint64_t intervalTime,
                                HAL_Bool absolute, HAL_Bool ack);

/**
 * Cancels all future notifier alarms for a notifier.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[in] ack            true to acknowledge any prior alarm
 * @return Error status. 0 on success.
 */
HAL_Status HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                   HAL_Bool ack);

/**
 * Indicates the notifier alarm has been serviced. Makes no change to future
 * alarms.
 *
 * One of HAL_SetNotifierAlarm (with ack=true), HAL_CancelNotifierAlarm (with
 * ack=true), or this function must be called before waiting for the next alarm.
 *
 * @param[in] notifierHandle the notifier handle
 * @return Error status. 0 on success.
 */
HAL_Status HAL_AcknowledgeNotifierAlarm(HAL_NotifierHandle notifierHandle);

/**
 * Gets the overrun count for a notifier.
 *
 * An overrun occurs when a notifier's alarm is not serviced before the next
 * scheduled alarm time.
 *
 * @param[in] notifierHandle the notifier handle
 * @param[out] count overrun count
 * @return Error status. 0 on success.
 */
HAL_Status HAL_GetNotifierOverrun(HAL_NotifierHandle notifierHandle,
                                  int32_t* count);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
