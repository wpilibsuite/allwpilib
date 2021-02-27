// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <wpi/nodiscard.h>

#include "hal/Types.h"

/**
 * @defgroup hal_notifier Notifier Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a notifier.
 *
 * A notifier is an FPGA controller timer that triggers at requested intervals
 * based on the FPGA time. This can be used to make precise control loops.
 *
 * @return the created notifier
 */
HAL_NotifierHandle HAL_InitializeNotifier(int32_t* status);

/**
 * Sets the name of a notifier.
 *
 * @param notifierHandle the notifier handle
 * @param name name
 */
void HAL_SetNotifierName(HAL_NotifierHandle notifierHandle, const char* name,
                         int32_t* status);

/**
 * Stops a notifier from running.
 *
 * This will cause any call into HAL_WaitForNotifierAlarm to return.
 *
 * @param notifierHandle the notifier handle
 */
void HAL_StopNotifier(HAL_NotifierHandle notifierHandle, int32_t* status);

/**
 * Cleans a notifier.
 *
 * Note this also stops a notifier if it is already running.
 *
 * @param notifierHandle the notifier handle
 */
void HAL_CleanNotifier(HAL_NotifierHandle notifierHandle, int32_t* status);

/**
 * Updates the trigger time for a notifier.
 *
 * Note that this time is an absolute time relative to HAL_GetFPGATime()
 *
 * @param notifierHandle the notifier handle
 * @param triggerTime    the updated trigger time
 */
void HAL_UpdateNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             uint64_t triggerTime, int32_t* status);

/**
 * Cancels the next notifier alarm.
 *
 * This does not cause HAL_WaitForNotifierAlarm to return.
 *
 * @param notifierHandle the notifier handle
 */
void HAL_CancelNotifierAlarm(HAL_NotifierHandle notifierHandle,
                             int32_t* status);

/**
 * Waits for the next alarm for the specific notifier.
 *
 * This is a blocking call until either the time elapses or HAL_StopNotifier
 * gets called. If the latter occurs, this function will return zero and any
 * loops using this function should exit. Failing to do so can lead to
 * use-after-frees.
 *
 * @param notifierHandle the notifier handle
 * @return               the FPGA time the notifier returned
 */
WPI_NODISCARD
uint64_t HAL_WaitForNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
