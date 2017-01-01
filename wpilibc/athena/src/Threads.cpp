/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Threads.h"
#include "Threads.h"

#include "ErrorBase.h"
#include "HAL/HAL.h"

namespace frc {
/**
 * Get the thread priority for the specified thread.
 *
 * @param thread Reference to the thread to get the priority for
 * @param isRealTime Set to true if thread is realtime, otherwise false
 * @return The current thread priority. Scaled 1-99, with 1 being highest.
 */
int GetThreadPriority(std::thread& thread, bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto native = thread.native_handle();
  auto ret = HAL_GetThreadPriority(&native, &rt, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  *isRealTime = rt;
  return ret;
}

/**
 * Get the thread priority for the current thread
 *
 * @param isRealTime Set to true if thread is realtime, otherwise false
 * @return The current thread priority. Scaled 1-99.
 */
int GetCurrentThreadPriority(bool* isRealTime) {
  int32_t status = 0;
  HAL_Bool rt = false;
  auto ret = HAL_GetCurrentThreadPriority(&rt, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  *isRealTime = rt;
  return ret;
}

/**
 * Sets the thread priority for the specified thread
 *
 * @param thread Reference to the thread to set the priority of
 * @param realTime Set to true to set a realtime priority, false for standard
 * priority
 * @param priority Priority to set the thread to. Scaled 1-99, with 1 being
 * highest. On RoboRIO, priority is ignored for non realtime setting
 *
 * @return The success state of setting the priority
 */
bool SetThreadPriority(std::thread& thread, bool realTime, int priority) {
  int32_t status = 0;
  auto native = thread.native_handle();
  auto ret = HAL_SetThreadPriority(&native, realTime, priority, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return ret;
}

/**
 * Sets the thread priority for the current thread
 *
 * @param realTime Set to true to set a realtime priority, false for standard
 * priority
 * @param priority Priority to set the thread to. Scaled 1-99, with 1 being
 * highest. On RoboRIO, priority is ignored for non realtime setting
 *
 * @return The success state of setting the priority
 */
bool SetCurrentThreadPriority(bool realTime, int priority) {
  int32_t status = 0;
  auto ret = HAL_SetCurrentThreadPriority(realTime, priority, &status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return ret;
}
}  // namespace frc
