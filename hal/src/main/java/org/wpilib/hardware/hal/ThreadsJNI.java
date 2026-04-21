// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Threads HAL JNI Functions.
 *
 * @see "wpi/hal/Threads.h"
 */
public class ThreadsJNI extends JNIWrapper {
  /**
   * Gets the current thread's priority.
   *
   * <p>Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time, and 99 is
   * highest priority. See "man 7 sched" for details.
   *
   * @return The current thread's priority.
   * @see "HAL_GetCurrentThreadPriority"
   */
  public static native int getCurrentThreadPriority();

  /**
   * Sets the current thread's priority.
   *
   * <p>Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time, and 99 is
   * highest priority. See "man 7 sched" for details.
   *
   * @param priority The priority.
   * @return True on success.
   * @see "HAL_SetCurrentThreadPriority"
   */
  public static native boolean setCurrentThreadPriority(int priority);

  /** Utility class. */
  private ThreadsJNI() {}
}
