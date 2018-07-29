/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

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
 * gets called.
 *
 * @param notifierHandle the notifier handle
 * @return               the FPGA time the notifier returned
 */
uint64_t HAL_WaitForNotifierAlarm(HAL_NotifierHandle notifierHandle,
                                  int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
