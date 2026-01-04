// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

/**
 * Generic handle for all WPI handle-based interfaces.
 *
 * Handle data layout:
 * - Bits 0-23:  Type-specific
 * - Bits 24-30: Type
 * - Bit 31:     Error
 */
typedef unsigned int WPI_Handle;  // NOLINT

/** An event handle. */
typedef WPI_Handle WPI_EventHandle;  // NOLINT

/** A semaphore handle. */
typedef WPI_Handle WPI_SemaphoreHandle;  // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates an event.  Events have binary state (signaled or not signaled) and
 * may be either automatically reset or manually reset.  Automatic-reset events
 * go to non-signaled state when a WaitForObject is woken up by the event;
 * manual-reset events require ResetEvent() to be called to set the event to
 * non-signaled state; if ResetEvent() is not called, any waiter on that event
 * will immediately wake when called.
 *
 * @param manual_reset true for manual reset, false for automatic reset
 * @param initial_state true to make the event initially in signaled state
 * @return Event handle
 */
WPI_EventHandle WPI_CreateEvent(int manual_reset, int initial_state);

/**
 * Destroys an event.  Destruction wakes up any waiters.
 *
 * @param handle event handle
 */
void WPI_DestroyEvent(WPI_EventHandle handle);

/**
 * Sets an event to signaled state.
 *
 * @param handle event handle
 */
void WPI_SetEvent(WPI_EventHandle handle);

/**
 * Sets an event to non-signaled state.
 *
 * @param handle event handle
 */
void WPI_ResetEvent(WPI_EventHandle handle);

/**
 * Creates a semaphore.  Semaphores keep an internal counter.  Releasing the
 * semaphore increases the count.  A semaphore with a non-zero count is
 * considered signaled.  When a waiter wakes up it atomically decrements the
 * count by 1.  This is generally useful in a single-supplier,
 * multiple-consumer scenario.
 *
 * @param initial_count initial value for the semaphore's internal counter
 * @param maximum_count maximum value for the semaphore's internal counter
 * @return Semaphore handle
 */
WPI_SemaphoreHandle WPI_CreateSemaphore(int initial_count, int maximum_count);

/**
 * Destroys a semaphore.  Destruction wakes up any waiters.
 *
 * @param handle semaphore handle
 */
void WPI_DestroySemaphore(WPI_SemaphoreHandle handle);

/**
 * Releases N counts of a semaphore.
 *
 * @param handle semaphore handle
 * @param release_count amount to add to semaphore's internal counter;
 *        must be positive
 * @param prev_count if non-null, previous count (output parameter)
 * @return Non-zero on successful release, zero on failure (e.g. release count
 *         would exceed maximum value, or handle invalid)
 */
int WPI_ReleaseSemaphore(WPI_SemaphoreHandle handle, int release_count,
                         int* prev_count);

/**
 * Waits for an handle to be signaled.
 *
 * @param handle handle to wait on
 * @return Non-zero if handle was signaled, zero otherwise (e.g. object was
 *         destroyed)
 */
int WPI_WaitForObject(WPI_Handle handle);

/**
 * Waits for an handle to be signaled, with timeout.
 *
 * @param handle handle to wait on
 * @param timeout timeout in seconds
 * @param timed_out if non-null, set to non-zero if timeout reached without
 *        handle being signaled; set to zero otherwise (output)
 * @return Non-zero if handle was signaled, zero otherwise (e.g. object was
 *         destroyed or timed out)
 */
int WPI_WaitForObjectTimeout(WPI_Handle handle, double timeout, int* timed_out);

/**
 * Waits for one or more handles to be signaled.
 *
 * Invalid handles are treated as signaled; the returned array will have the
 * handle error bit set for any invalid handles.
 *
 * @param handles array of handles to wait on
 * @param handles_count length of the handles array
 * @param signaled output array for storage of signaled handles; must be at
 *        least the size of the handles input array
 * @return number of signaled handles
 */
int WPI_WaitForObjects(const WPI_Handle* handles, int handles_count,
                       WPI_Handle* signaled);

/**
 * Waits for one or more handles to be signaled, with timeout.
 *
 * Invalid handles are treated as signaled; the returned array will have the
 * handle error bit set for any invalid handles.
 *
 * @param handles array of handles to wait on
 * @param handles_count length of the handles array
 * @param signaled output array for storage of signaled handles; must be at
 *        least the size of the handles input array
 * @param timeout timeout in seconds
 * @param timed_out if non-null, set to non-zero if timeout reached without any
 *        handle being signaled; set to zero otherwise (output)
 * @return number of signaled handles
 */
int WPI_WaitForObjectsTimeout(const WPI_Handle* handles, int handles_count,
                              WPI_Handle* signaled, double timeout,
                              int* timed_out);

/**
 * Sets up signaling for an arbitrary handle.  With this function, any handle
 * can operate like an event handle.
 *
 * @param handle handle
 * @param manual_reset true for manual reset, false for automatic reset
 * @param initial_state true to make the handle initially in signaled state
 */
void WPI_CreateSignalObject(WPI_Handle handle, int manual_reset,
                            int initial_state);

/**
 * Sets a handle to signaled state.
 *
 * @param handle handle
 */
void WPI_SetSignalObject(WPI_Handle handle);

/**
 * Sets a handle to non-signaled state.
 *
 * @param handle handle
 */
void WPI_ResetSignalObject(WPI_Handle handle);

/**
 * Cleans up signaling for a handle.  Destruction wakes up any waiters.
 *
 * @param handle handle
 */
void WPI_DestroySignalObject(WPI_Handle handle);

#ifdef __cplusplus
}  // extern "C"
#endif
