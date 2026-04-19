// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <thread>

namespace wpi {

/**
 * Gets the specified thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @param thread The thread.
 * @return The specified thread's priority.
 */
int GetThreadPriority(std::thread& thread);

/**
 * Gets the current thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @return The current thread's priority.
 */
int GetCurrentThreadPriority();

/**
 * Sets the specified thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @param thread The thread.
 * @param priority The priority.
 * @return True on success.
 * @deprecated Incorrect usage of real-time priority can lead to system lockups.
 *     Only use this function if you are trained in real-time software
 *     development.
 */
[[deprecated(
    "Incorrect usage of real-time priority can lead to system lockups. Only "
    "use this function if you are trained in real-time software development.")]]
bool SetThreadPriority(std::thread& thread, int priority);

/**
 * Sets the current thread's priority.
 *
 * Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time,
 * and 99 is highest priority. See "man 7 sched" for details.
 *
 * @param priority The priority.
 * @return True on success.
 * @deprecated Incorrect usage of real-time priority can lead to system lockups.
 *     Only use this function if you are trained in real-time software
 *     development.
 */
[[deprecated(
    "Incorrect usage of real-time priority can lead to system lockups. Only "
    "use this function if you are trained in real-time software development.")]]
bool SetCurrentThreadPriority(int priority);

}  // namespace wpi
