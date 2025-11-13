// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

/**
 * @defgroup hal_counter Counter Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a counter.
 *
 * @param[in] channel               the dio channel
 * @param[in] risingEdge            true to count on rising edge, false for
 * falling
 * @param[in] allocationLocation    the location where the allocation is
 * occurring (can be null)
 * @param[out] status     Error status variable. 0 on success.
 * @return the created handle
 */
HAL_CounterHandle HAL_InitializeCounter(int channel, HAL_Bool risingEdge,
                                        const char* allocationLocation,
                                        int32_t* status);

/**
 * Frees a counter.
 *
 * @param[in] counterHandle the counter handle
 */
void HAL_FreeCounter(HAL_CounterHandle counterHandle);

/**
 * Sets the up source to either detect rising edges or falling edges.
 *
 * Note that both are allowed to be set true at the same time without issues.
 *
 * @param[in] counterHandle  the counter handle
 * @param[in] risingEdge     true to trigger on rising
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetCounterEdgeConfiguration(HAL_CounterHandle counterHandle,
                                     HAL_Bool risingEdge, int32_t* status);

/**
 * Resets the Counter to zero.
 *
 * Sets the counter value to zero. This does not effect the running state of the
 * counter, just sets the current value to zero.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Reads the current counter value.
 *
 * Reads the value at this instant. It may still be running, so it reflects the
 * current value. Next time it is read, it might have a different value.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the current counter value
 */
int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Gets the Period of the most recent count.
 *
 * Returns the time interval of the most recent count. This can be used for
 * velocity calculations to determine shaft speed.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the period of the last two pulses in units of seconds
 */
double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Sets the maximum period where the device is still considered "moving".
 *
 * Sets the maximum period where the device is considered moving. This value is
 * used to determine the "stopped" state of the counter using the
 * HAL_GetCounterStopped method.
 *
 * @param[in] counterHandle the counter handle
 * @param[in] maxPeriod     the maximum period where the counted device is
 *                          considered moving in seconds
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status);

/**
 * Determines if the clock is stopped.
 *
 * Determine if the clocked input is stopped based on the MaxPeriod value set
 * using the SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the
 * device (and counter) are assumed to be stopped and it returns true.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return true if the most recent counter period exceeds the MaxPeriod value
 *         set by SetMaxPeriod
 */
HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
