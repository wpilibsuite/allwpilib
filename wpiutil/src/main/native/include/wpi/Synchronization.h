// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <climits>  // NOLINT

#ifdef __cplusplus
#include <initializer_list>

#include "wpi/span.h"
#endif

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

namespace wpi {

/** Constant representing an invalid handle. */
constexpr unsigned int kInvalidHandle = 0;

/**
 * Standard types for handles.
 * @{
 */
constexpr int kHandleTypeEvent = 1;
constexpr int kHandleTypeSemaphore = 2;
constexpr int kHandleTypeCSBase = 3;
constexpr int kHandleTypeNTBase = 16;
constexpr int kHandleTypeHALBase = 32;
constexpr int kHandleTypeUserBase = 64;
/** @} */

/**
 * Creates an event.  Events have binary state (signaled or not signaled) and
 * may be either automatically reset or manually reset.  Automatic-reset events
 * go to non-signaled state when a WaitForObject is woken up by the event;
 * manual-reset events require ResetEvent() to be called to set the event to
 * non-signaled state; if ResetEvent() is not called, any waiter on that event
 * will immediately wake when called.
 *
 * @param manualReset true for manual reset, false for automatic reset
 * @param initialState true to make the event initially in signaled state
 * @return Event handle
 */
WPI_EventHandle CreateEvent(bool manualReset = false,
                            bool initialState = false);

/**
 * Destroys an event.  Destruction wakes up any waiters.
 *
 * @param handle event handle
 */
void DestroyEvent(WPI_EventHandle handle);

/**
 * Sets an event to signaled state.
 *
 * @param handle event handle
 */
void SetEvent(WPI_EventHandle handle);

/**
 * Sets an event to non-signaled state.
 *
 * @param handle event handle
 */
void ResetEvent(WPI_EventHandle handle);

/**
 * Creates a semaphore.  Semaphores keep an internal counter.  Releasing the
 * semaphore increases the count.  A semaphore with a non-zero count is
 * considered signaled.  When a waiter wakes up it atomically decrements the
 * count by 1.  This is generally useful in a single-supplier,
 * multiple-consumer scenario.
 *
 * @param initialCount initial value for the semaphore's internal counter
 * @param maximumCount maximum value for the samephore's internal counter
 * @return Semaphore handle
 */
WPI_SemaphoreHandle CreateSemaphore(int initialCount = 0,
                                    int maximumCount = INT_MAX);

/**
 * Destroys a semaphore.  Destruction wakes up any waiters.
 *
 * @param handle semaphore handle
 */
void DestroySemaphore(WPI_SemaphoreHandle handle);

/**
 * Releases N counts of a semaphore.
 *
 * @param handle semaphore handle
 * @param releaseCount amount to add to semaphore's internal counter;
 *        must be positive
 * @param prevCount if non-null, previous count (output parameter)
 * @return True on successful release, false on failure (e.g. release count
 *         would exceed maximum value, or handle invalid)
 */
bool ReleaseSemaphore(WPI_SemaphoreHandle handle, int releaseCount = 1,
                      int* prevCount = nullptr);

/**
 * Waits for an handle to be signaled.
 *
 * @param handle handle to wait on
 * @return True if handle was signaled, false otherwise (e.g. object was
 *         destroyed)
 */
bool WaitForObject(WPI_Handle handle);

/**
 * Waits for an handle to be signaled, with timeout.
 *
 * @param handle handle to wait on
 * @param timeout timeout in seconds
 * @param timedOut if non-null, set to true if timeout reached without handle
 *        being signaled; set to false otherwise (output)
 * @return True if handle was signaled, false otherwise (e.g. object was
 *         destroyed or timed out)
 */
bool WaitForObject(WPI_Handle handle, double timeout, bool* timedOut);

/**
 * Waits for one or more handles to be signaled.
 *
 * Invalid handles are treated as signaled; the returned array will have the
 * handle error bit set for any invalid handles.
 *
 * @param handles array of handles to wait on
 * @param signaled output array for storage of signaled handles; must be at
 *        least the size of the handles input array
 * @return array of signaled handles (points into signaled array)
 */
wpi::span<WPI_Handle> WaitForObjects(wpi::span<const WPI_Handle> handles,
                                     wpi::span<WPI_Handle> signaled);

/**
 * Waits for one or more handles to be signaled.
 *
 * Invalid handles are treated as signaled; the returned array will have the
 * handle error bit set for any invalid handles.
 *
 * @param handles array of handles to wait on
 * @param signaled output array for storage of signaled handles; must be at
 *        least the size of the handles input array
 * @return array of signaled handles (points into signaled array)
 */
inline wpi::span<WPI_Handle> WaitForObjects(
    std::initializer_list<WPI_Handle> handles, wpi::span<WPI_Handle> signaled) {
  return WaitForObjects(wpi::span{handles.begin(), handles.size()}, signaled);
}

/**
 * Waits for one or more handles to be signaled, with timeout.
 *
 * Invalid handles are treated as signaled; the returned array will have the
 * handle error bit set for any invalid handles.
 *
 * @param handles array of handles to wait on
 * @param signaled output array for storage of signaled handles; must be at
 *        least the size of the handles input array
 * @param timeout timeout in seconds
 * @param timedOut if non-null, set to true if timeout reached without any
 *        handle being signaled; set to false otherwise (output)
 * @return array of signaled handles (points into signaled array)
 */
wpi::span<WPI_Handle> WaitForObjects(wpi::span<const WPI_Handle> handles,
                                     wpi::span<WPI_Handle> signaled,
                                     double timeout, bool* timedOut);
/**
 * Waits for one or more handles to be signaled, with timeout.
 *
 * Invalid handles are treated as signaled; the returned array will have the
 * handle error bit set for any invalid handles.
 *
 * @param handles array of handles to wait on
 * @param signaled output array for storage of signaled handles; must be at
 *        least the size of the handles input array
 * @param timeout timeout in seconds
 * @param timedOut if non-null, set to true if timeout reached without any
 *        handle being signaled; set to false otherwise (output)
 * @return array of signaled handles (points into signaled array)
 */
inline wpi::span<WPI_Handle> WaitForObjects(
    std::initializer_list<WPI_Handle> handles, wpi::span<WPI_Handle> signaled,
    double timeout, bool* timedOut) {
  return WaitForObjects(wpi::span{handles.begin(), handles.size()}, signaled,
                        timeout, timedOut);
}

/**
 * Sets up signaling for an arbitrary handle.  With this function, any handle
 * can operate like an event handle.
 *
 * @param handle handle
 * @param manualReset true for manual reset, false for automatic reset
 * @param initialState true to make the handle initially in signaled state
 * @return Event handle
 */
void CreateSignalObject(WPI_Handle handle, bool manualReset = false,
                        bool initialState = false);

/**
 * Sets a handle to signaled state.
 *
 * @param handle handle
 */
void SetSignalObject(WPI_Handle handle);

/**
 * Sets a handle to non-signaled state.
 *
 * @param handle handle
 */
void ResetSignalObject(WPI_Handle handle);

/**
 * Cleans up signaling for a handle.  Destruction wakes up any waiters.
 *
 * @param handle handle
 */
void DestroySignalObject(WPI_Handle handle);

/**
 * An atomic signaling event for synchronization.
 *
 * Events have binary state (signaled or not signaled) and may be either
 * automatically reset or manually reset.  Automatic-reset events go to
 * non-signaled state when a WaitForObject is woken up by the event;
 * manual-reset events require Reset() to be called to set the event to
 * non-signaled state; if Reset() is not called, any waiter on that event
 * will immediately wake when called.
 */
class Event final {
 public:
  /**
   * Constructor.
   *
   * @param manualReset true for manual reset, false for automatic reset
   * @param initialState true to make the event initially in signaled state
   */
  explicit Event(bool manualReset = false, bool initialState = false)
      : m_handle{CreateEvent(manualReset, initialState)} {}
  ~Event() {
    if (m_handle != 0) {
      DestroyEvent(m_handle);
    }
  }

  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;

  Event(Event&& rhs) : m_handle{rhs.m_handle} { rhs.m_handle = 0; }
  Event& operator=(Event&& rhs) {
    if (m_handle != 0) {
      DestroyEvent(m_handle);
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = 0;
    return *this;
  }

  /**
   * Gets the event handle (e.g. for WaitForObject).
   *
   * @return handle
   */
  explicit operator WPI_Handle() const { return m_handle; }

  /**
   * Gets the event handle (e.g. for WaitForObject).
   *
   * @return handle
   */
  WPI_EventHandle GetHandle() const { return m_handle; }

  /**
   * Sets the event to signaled state.
   */
  void Set() { SetEvent(m_handle); }

  /**
   * Sets the event to non-signaled state.
   */
  void Reset() { ResetEvent(m_handle); }

 private:
  WPI_EventHandle m_handle;
};

/**
 * A semaphore for synchronization.
 *
 * Semaphores keep an internal counter.  Releasing the semaphore increases
 * the count.  A semaphore with a non-zero count is considered signaled.
 * When a waiter wakes up it atomically decrements the count by 1.  This
 * is generally useful in a single-supplier, multiple-consumer scenario.
 */
class Semaphore final {
 public:
  /**
   * Constructor.
   *
   * @param initialCount initial value for the semaphore's internal counter
   * @param maximumCount maximum value for the samephore's internal counter
   */
  explicit Semaphore(int initialCount = 0, int maximumCount = INT_MAX)
      : m_handle{CreateSemaphore(initialCount, maximumCount)} {}
  ~Semaphore() {
    if (m_handle != 0) {
      DestroySemaphore(m_handle);
    }
  }

  Semaphore(const Semaphore&) = delete;
  Semaphore& operator=(const Semaphore&) = delete;

  Semaphore(Semaphore&& rhs) : m_handle{rhs.m_handle} { rhs.m_handle = 0; }
  Semaphore& operator=(Semaphore&& rhs) {
    if (m_handle != 0) {
      DestroySemaphore(m_handle);
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = 0;
    return *this;
  }

  /**
   * Gets the semaphore handle (e.g. for WaitForObject).
   *
   * @return handle
   */
  explicit operator WPI_Handle() const { return m_handle; }

  /**
   * Gets the semaphore handle (e.g. for WaitForObject).
   *
   * @return handle
   */
  WPI_SemaphoreHandle GetHandle() const { return m_handle; }

  /**
   * Releases N counts of the semaphore.
   *
   * @param releaseCount amount to add to semaphore's internal counter;
   *        must be positive
   * @param prevCount if non-null, previous count (output parameter)
   * @return True on successful release, false on failure (e.g. release count
   *         would exceed maximum value, or handle invalid)
   */
  bool Release(int releaseCount = 1, int* prevCount = nullptr) {
    return ReleaseSemaphore(m_handle, releaseCount, prevCount);
  }

 private:
  WPI_SemaphoreHandle m_handle;
};

/**
 * RAII wrapper for signaling handles.
 *
 * Sets up signaling for an arbitrary handle.  This enables any handle
 * to operate like an event handle.
 */
template <typename T>
class SignalObject final {
 public:
  /**
   * Constructor.
   *
   * @param handle handle
   * @param manualReset true for manual reset, false for automatic reset
   * @param initialState true to make the handle initially in signaled state
   */
  explicit SignalObject(T handle = 0, bool manualReset = false,
                        bool initialState = false)
      : m_handle{handle} {
    CreateSignalObject(handle, manualReset, initialState);
  }
  ~SignalObject() {
    if (m_handle != 0) {
      DestroySignalObject(m_handle);
    }
  }

  SignalObject(const SignalObject&) = delete;
  SignalObject& operator=(const SignalObject&) = delete;

  SignalObject(SignalObject&& rhs) : m_handle{rhs.m_handle} {
    rhs.m_handle = 0;
  }
  SignalObject& operator=(SignalObject&& rhs) {
    if (m_handle != 0) {
      DestroySemaphore(m_handle);
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = 0;
    return *this;
  }

  /**
   * Gets the handle.
   *
   * @return handle
   */
  /*implicit*/ operator T() const { return m_handle; }  // NOLINT

  /**
   * Gets the handle (e.g. for WaitForObject).
   *
   * @return handle
   */
  T GetHandle() const { return m_handle; }

  /**
   * Sets the handle to signaled state.
   */
  void Set() { SetSignalObject(m_handle); }

  /**
   * Sets the handle to non-signaled state.
   */
  void Reset() { ResetSignalObject(m_handle); }

 private:
  T m_handle;
};

}  // namespace wpi

extern "C" {

#endif  // __cplusplus

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
 * @param maximum_count maximum value for the samephore's internal counter
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
