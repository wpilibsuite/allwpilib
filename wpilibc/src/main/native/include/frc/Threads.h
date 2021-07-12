// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <thread>

namespace frc {

/**
 * Get the thread priority for the specified thread.
 *
 * @param thread     Reference to the thread to get the priority for.
 * @param isRealTime Set to true if thread is real-time, otherwise false.
 * @return           The current thread priority. For real-time, this is 1-99
 *                   with 99 being highest. For non-real-time, this is 0. See
 *                   "man 7 sched" for details.
 */
int GetThreadPriority(std::thread& thread, bool* isRealTime);

/**
 * Get the thread priority for the current thread.
 *
 * @param isRealTime Set to true if thread is real-time, otherwise false.
 * @return           The current thread priority. For real-time, this is 1-99
 *                   with 99 being highest. For non-real-time, this is 0. See
 *                   "man 7 sched" for details.
 */
int GetCurrentThreadPriority(bool* isRealTime);

/**
 * Sets the thread priority for the specified thread.
 *
 * @param thread   Reference to the thread to set the priority of.
 * @param realTime Set to true to set a real-time priority, false for standard
 *                 priority.
 * @param priority Priority to set the thread to. For real-time, this is 1-99
 *                 with 99 being highest. For non-real-time, this is forced to
 *                 0. See "man 7 sched" for more details.
 * @return         True on success.
 */
bool SetThreadPriority(std::thread& thread, bool realTime, int priority);

/**
 * Sets the thread priority for the current thread.
 *
 * @param realTime Set to true to set a real-time priority, false for standard
 *                 priority.
 * @param priority Priority to set the thread to. For real-time, this is 1-99
 *                 with 99 being highest. For non-real-time, this is forced to
 *                 0. See "man 7 sched" for more details.
 * @return         True on success.
 */
bool SetCurrentThreadPriority(bool realTime, int priority);

}  // namespace frc
